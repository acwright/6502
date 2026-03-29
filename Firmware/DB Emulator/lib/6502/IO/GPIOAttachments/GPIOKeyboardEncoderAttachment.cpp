#include "GPIOKeyboardEncoderAttachment.h"
#include "utilities.h"

GPIOKeyboardEncoderAttachment::GPIOKeyboardEncoderAttachment(uint8_t priority)
  : GPIOAttachment(priority, true, true, true, true),  // Uses CA1, CA2, CB1, CB2
    asciiDataA(0x00), dataReadyA(false), interruptPendingA(false), enabledA(false),
    asciiDataB(0x00), dataReadyB(false), interruptPendingB(false), enabledB(false),
    shiftPressed(false), ctrlPressed(false) {
  reset();
}

void GPIOKeyboardEncoderAttachment::reset() {
  GPIOAttachment::reset();
  asciiDataA = 0x00;
  dataReadyA = false;
  interruptPendingA = false;
  enabledA = false;
  asciiDataB = 0x00;
  dataReadyB = false;
  interruptPendingB = false;
  enabledB = false;
  shiftPressed = false;
  ctrlPressed = false;
}

uint8_t GPIOKeyboardEncoderAttachment::readPortA(uint8_t ddrA, uint8_t orA) {
  // Only provide data when enabled and data is ready
  if (enabledA && dataReadyA) {
    // Reading the port will clear data ready flag (done via clearInterrupts)
    return asciiDataA;
  }
  
  // No data to provide
  return 0xFF;
}

uint8_t GPIOKeyboardEncoderAttachment::readPortB(uint8_t ddrB, uint8_t orB) {
  // Only provide data when enabled and data is ready
  if (enabledB && dataReadyB) {
    // Reading the port will clear data ready flag (done via clearInterrupts)
    return asciiDataB;
  }
  
  // No data to provide
  return 0xFF;
}

void GPIOKeyboardEncoderAttachment::updateControlLines(bool ca1, bool ca2, bool cb1, bool cb2) {
  stateCA1 = ca1;
  stateCA2 = ca2;
  stateCB1 = cb1;
  stateCB2 = cb2;
  
  // Enabled when CA2 is LOW for Port A
  enabledA = !ca2;
  
  // Enabled when CB2 is LOW for Port B
  enabledB = !cb2;
}

bool GPIOKeyboardEncoderAttachment::hasCA1Interrupt() const {
  return interruptPendingA && enabledA;
}

bool GPIOKeyboardEncoderAttachment::hasCB1Interrupt() const {
  return interruptPendingB && enabledB;
}

void GPIOKeyboardEncoderAttachment::clearInterrupts(bool ca1, bool ca2, bool cb1, bool cb2) {
  if (ca1) {
    interruptPendingA = false;
    dataReadyA = false;  // Clear data ready flag when Port A is read
  }
  if (cb1) {
    interruptPendingB = false;
    dataReadyB = false;  // Clear data ready flag when Port B is read
  }
}

uint8_t GPIOKeyboardEncoderAttachment::mapKeyWithModifiers(uint8_t usbHidKeycode) {
  // Get base ASCII character from USB HID keycode
  uint8_t baseChar = USB_HID_TO_ASCII[usbHidKeycode];

  // If no base mapping, return 0x00
  if (baseChar == 0x00) {
    return 0x00;
  }

  // Priority 1: Ctrl (overrides Shift)
  if (ctrlPressed) {
    // Ctrl with letters produces control codes 0x01-0x1A
    if (baseChar >= 0x41 && baseChar <= 0x5A) {  // A-Z (uppercase from table)
      return baseChar - 0x41 + 0x01;
    }

    // Ctrl with other special keys
    switch (baseChar) {
      case '2': return 0x00;  // Ctrl+2 = NUL
      case '6': return 0x1E;  // Ctrl+6 = RS
      case '-': return 0x1F;  // Ctrl+- = US
      case '[': return 0x1B;  // Ctrl+[ = ESC
      case '\\': return 0x1C;  // Ctrl+\ = FS
      case ']': return 0x1D;  // Ctrl+] = GS
      default: break;
    }
  }

  // Priority 2: Shift (symbols only — letters are always uppercase)
  if (shiftPressed && !ctrlPressed) {
    switch (baseChar) {
      case '1': return '!';
      case '2': return '@';
      case '3': return '#';
      case '4': return '$';
      case '5': return '%';
      case '6': return '^';
      case '7': return '&';
      case '8': return '*';
      case '9': return '(';
      case '0': return ')';
      case '-': return '_';
      case '=': return '+';
      case '[': return '{';
      case ']': return '}';
      case '\\': return '|';
      case ';': return ':';
      case '\'': return '"';
      case ',': return '<';
      case '.': return '>';
      case '/': return '?';
      case '`': return '~';
      default: break;
    }
  }

  // Priority 3: Base character (already uppercase for letters)
  return baseChar;
}

void GPIOKeyboardEncoderAttachment::updateKey(uint8_t usbHidKeycode, bool pressed) {
  updateKeyPortA(usbHidKeycode, pressed);
  updateKeyPortB(usbHidKeycode, pressed);
}

void GPIOKeyboardEncoderAttachment::updateKeyPortA(uint8_t usbHidKeycode, bool pressed) {
  // Handle modifier keys - update state
  switch (usbHidKeycode) {
    case 0xE0:  // Left Ctrl
    case 0xE4:  // Right Ctrl
      ctrlPressed = pressed;
      return;
    case 0xE1:  // Left Shift
    case 0xE5:  // Right Shift
      shiftPressed = pressed;
      return;
    default:
      break;
  }

  // Only process key presses, not releases
  if (!pressed) return;

  uint8_t mappedValue = mapKeyWithModifiers(usbHidKeycode);

  // Ignore unmapped keys (0x00 is valid only when Ctrl is held for Ctrl+2 = NUL)
  if (mappedValue == 0x00 && !ctrlPressed) return;

  asciiDataA = mappedValue;
  dataReadyA = true;
  if (enabledA) interruptPendingA = true;
}

void GPIOKeyboardEncoderAttachment::updateKeyPortB(uint8_t usbHidKeycode, bool pressed) {
  // Handle modifier keys - update state
  switch (usbHidKeycode) {
    case 0xE0:  // Left Ctrl
    case 0xE4:  // Right Ctrl
      ctrlPressed = pressed;
      return;
    case 0xE1:  // Left Shift
    case 0xE5:  // Right Shift
      shiftPressed = pressed;
      return;
    default:
      break;
  }

  // Only process key presses, not releases
  if (!pressed) return;

  uint8_t mappedValue = mapKeyWithModifiers(usbHidKeycode);

  // Ignore unmapped keys (0x00 is valid only when Ctrl is held for Ctrl+2 = NUL)
  if (mappedValue == 0x00 && !ctrlPressed) return;

  asciiDataB = mappedValue;
  dataReadyB = true;
  if (enabledB) interruptPendingB = true;
}

bool GPIOKeyboardEncoderAttachment::hasDataReadyA() const {
  return dataReadyA && enabledA;
}

bool GPIOKeyboardEncoderAttachment::hasDataReadyB() const {
  return dataReadyB && enabledB;
}
