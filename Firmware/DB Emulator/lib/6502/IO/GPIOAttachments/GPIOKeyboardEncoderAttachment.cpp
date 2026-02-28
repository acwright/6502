#include "GPIOKeyboardEncoderAttachment.h"
#include "utilities.h"

GPIOKeyboardEncoderAttachment::GPIOKeyboardEncoderAttachment(uint8_t priority)
  : GPIOAttachment(priority, true, true, true, true),  // Uses CA1, CA2, CB1, CB2
    asciiDataA(0x00), dataReadyA(false), interruptPendingA(false), enabledA(false),
    asciiDataB(0x00), dataReadyB(false), interruptPendingB(false), enabledB(false),
    shiftPressed(false), ctrlPressed(false), altPressed(false), menuPressed(false) {
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
  altPressed = false;
  menuPressed = false;
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
  // Handle MENU key (USB HID 0xE3 Left GUI)
  if (usbHidKeycode == 0xE3 || usbHidKeycode == 0xE7) {
    if (altPressed) {
      return 0x90;  // Alt+MENU
    }
    return 0x80;  // MENU alone
  }
  
  // Handle function keys F1-F15 (USB HID 0x3A-0x6A)
  // F1-F15 without Alt → 0x81-0x8F
  // Alt+F1-F15 → 0x91-0x9F
  if (usbHidKeycode >= 0x3A && usbHidKeycode <= 0x45) {
    // F1-F12 (0x3A-0x45)
    uint8_t fKeyOffset = usbHidKeycode - 0x3A;  // 0-11 for F1-F12
    if (altPressed) {
      return 0x91 + fKeyOffset;  // Alt+F1-F12 → 0x91-0x9C
    }
    return 0x81 + fKeyOffset;  // F1-F12 → 0x81-0x8C
  }
  if (usbHidKeycode >= 0x68 && usbHidKeycode <= 0x6A) {
    // F13-F15 (0x68-0x6A)
    uint8_t fKeyOffset = usbHidKeycode - 0x68 + 12;  // 12-14 for F13-F15
    if (altPressed) {
      return 0x91 + fKeyOffset;  // Alt+F13-F15 → 0x9D-0x9F
    }
    return 0x81 + fKeyOffset;  // F13-F15 → 0x8D-0x8F
  }
  
  // Get base ASCII character from USB HID keycode
  uint8_t baseChar = USB_HID_TO_ASCII[usbHidKeycode];
  
  // If no base mapping, return 0x00
  if (baseChar == 0x00) {
    return 0x00;
  }
  
  // Handle Ctrl combinations - control codes
  if (ctrlPressed && !altPressed) {
    // Ctrl with letters produces control codes 0x01-0x1A
    if (baseChar >= 'a' && baseChar <= 'z') {
      return baseChar - 'a' + 0x01;
    }
    if (baseChar >= 'A' && baseChar <= 'Z') {
      return baseChar - 'A' + 0x01;
    }
    
    // Ctrl with other special keys
    switch (baseChar) {
      case '2': case '@': return 0x00;  // Ctrl+2 = NUL
      case '6': case '^': return 0x1E;  // Ctrl+6 = RS (UP arrow position)
      case '-': case '_': return 0x1F;  // Ctrl+- = US (DOWN arrow position)
      case '[': case '{': return 0x1B;  // Ctrl+[ = ESC
      case '\\': case '|': return 0x1C;  // Ctrl+\ = FS (LEFT arrow position)
      case ']': case '}': return 0x1D;  // Ctrl+] = GS (RIGHT arrow position)
      default: break;
    }
  }
  
  // Handle Alt+Shift combinations - extended character set
  if (altPressed && shiftPressed) {
    switch (baseChar) {
      case '1': return 0xA1;
      case '\'': return 0xA2;
      case '3': return 0xA3;
      case '4': return 0xA4;
      case '5': return 0xA5;
      case '7': return 0xA6;
      case '9': return 0xA8;
      case '0': return 0xA9;
      case '8': return 0xAA;
      case '=': return 0xAB;
      case ';': return 0xBA;
      case ',': return 0xBC;
      case '.': return 0xBE;
      case '/': return 0xBF;
      case '2': return 0xC0;
      case 'a': case 'A': return 0xC1;
      case 'b': case 'B': return 0xC2;
      case 'c': case 'C': return 0xC3;
      case 'd': case 'D': return 0xC4;
      case 'e': case 'E': return 0xC5;
      case 'f': case 'F': return 0xC6;
      case 'g': case 'G': return 0xC7;
      case 'h': case 'H': return 0xC8;
      case 'i': case 'I': return 0xC9;
      case 'j': case 'J': return 0xCA;
      case 'k': case 'K': return 0xCB;
      case 'l': case 'L': return 0xCC;
      case 'm': case 'M': return 0xCD;
      case 'n': case 'N': return 0xCE;
      case 'o': case 'O': return 0xCF;
      case 'p': case 'P': return 0xD0;
      case 'q': case 'Q': return 0xD1;
      case 'r': case 'R': return 0xD2;
      case 's': case 'S': return 0xD3;
      case 't': case 'T': return 0xD4;
      case 'u': case 'U': return 0xD5;
      case 'v': case 'V': return 0xD6;
      case 'w': case 'W': return 0xD7;
      case 'x': case 'X': return 0xD8;
      case 'y': case 'Y': return 0xD9;
      case 'z': case 'Z': return 0xDA;
      case '[': return 0xFB;
      case '\\': return 0xFC;
      case ']': return 0xFD;
      case '`': return 0xFE;
      case '6': return 0xDE;
      case '-': return 0xDF;
      default: break;
    }
  }
  
  // Handle Alt combinations (without shift)
  if (altPressed && !shiftPressed) {
    switch (baseChar) {
      case ' ': return 0xA0;  // Alt+Space
      case '\'': return 0xA7;
      case ',': return 0xAC;
      case '-': return 0xAD;
      case '.': return 0xAE;
      case '/': return 0xAF;
      case '0': return 0xB0;
      case '1': return 0xB1;
      case '2': return 0xB2;
      case '3': return 0xB3;
      case '4': return 0xB4;
      case '5': return 0xB5;
      case '6': return 0xB6;
      case '7': return 0xB7;
      case '8': return 0xB8;
      case '9': return 0xB9;
      case ';': return 0xBB;
      case '=': return 0xBD;
      case '[': return 0xDB;
      case '\\': return 0xDC;
      case ']': return 0xDD;
      case '`': return 0xE0;
      case 'a': case 'A': return 0xE1;
      case 'b': case 'B': return 0xE2;
      case 'c': case 'C': return 0xE3;
      case 'd': case 'D': return 0xE4;
      case 'e': case 'E': return 0xE5;
      case 'f': case 'F': return 0xE6;
      case 'g': case 'G': return 0xE7;
      case 'h': case 'H': return 0xE8;
      case 'i': case 'I': return 0xE9;
      case 'j': case 'J': return 0xEA;
      case 'k': case 'K': return 0xEB;
      case 'l': case 'L': return 0xEC;
      case 'm': case 'M': return 0xED;
      case 'n': case 'N': return 0xEE;
      case 'o': case 'O': return 0xEF;
      case 'p': case 'P': return 0xF0;
      case 'q': case 'Q': return 0xF1;
      case 'r': case 'R': return 0xF2;
      case 's': case 'S': return 0xF3;
      case 't': case 'T': return 0xF4;
      case 'u': case 'U': return 0xF5;
      case 'v': case 'V': return 0xF6;
      case 'w': case 'W': return 0xF7;
      case 'x': case 'X': return 0xF8;
      case 'y': case 'Y': return 0xF9;
      case 'z': case 'Z': return 0xFA;
      case 0x7F: return 0xFF;  // Alt+DEL
      default: break;
    }
  }
  
  // Handle Shift combinations - uppercase and shifted symbols
  if (shiftPressed && !ctrlPressed && !altPressed) {
    // Letters become uppercase
    if (baseChar >= 'a' && baseChar <= 'z') {
      return baseChar - 'a' + 'A';
    }
    
    // Shifted symbols
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
  
  // No modifiers or unhandled combination - return base character
  return baseChar;
}

void GPIOKeyboardEncoderAttachment::updateKey(uint8_t usbHidKeycode, bool pressed) {
  // Handle modifier keys - update state
  switch (usbHidKeycode) {
    case 0xE0:  // Left Ctrl
    case 0xE4:  // Right Ctrl
      ctrlPressed = pressed;
      return;  // Don't generate output for modifier keys alone
      
    case 0xE1:  // Left Shift
    case 0xE5:  // Right Shift
      shiftPressed = pressed;
      return;
      
    case 0xE2:  // Left Alt
    case 0xE6:  // Right Alt
      altPressed = pressed;
      return;
      
    case 0xE3:  // Left GUI (MENU)
    case 0xE7:  // Right GUI (MENU)
      menuPressed = pressed;
      // MENU key generates output, so don't return - fall through
      break;
      
    default:
      break;
  }
  
  // Only process key presses, not releases (encoder only reports keypress events)
  if (!pressed) {
    return;
  }
  
  // Map the key with active modifiers
  uint8_t mappedValue = mapKeyWithModifiers(usbHidKeycode);
  
  // Ignore keys with no mapping (0x00 unless it's a valid control code)
  if (mappedValue == 0x00 && usbHidKeycode != 0x28) {  // 0x28 is Enter which maps to 0x0D, not 0x00
    return;
  }
  
  // Update both ports with the mapped data
  asciiDataA = mappedValue;
  asciiDataB = mappedValue;
  dataReadyA = true;
  dataReadyB = true;
  
  // Trigger interrupts on both ports if enabled
  if (enabledA) {
    interruptPendingA = true;
  }
  if (enabledB) {
    interruptPendingB = true;
  }
}

bool GPIOKeyboardEncoderAttachment::hasDataReadyA() const {
  return dataReadyA && enabledA;
}

bool GPIOKeyboardEncoderAttachment::hasDataReadyB() const {
  return dataReadyB && enabledB;
}
