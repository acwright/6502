#include "GPIOKeyboardEncoderAttachment.h"
#include "utilities.h"

GPIOKeyboardEncoderAttachment::GPIOKeyboardEncoderAttachment(uint8_t priority)
  : GPIOAttachment(priority, true, true, true, true),  // Uses CA1, CA2, CB1, CB2
    asciiDataA(0x00), dataReadyA(false), interruptPendingA(false), enabledA(false),
    asciiDataB(0x00), dataReadyB(false), interruptPendingB(false), enabledB(false) {
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

void GPIOKeyboardEncoderAttachment::updateKey(uint8_t usbHidKeycode, bool pressed) {
  // Only process key presses, not releases (encoder only reports keypress events)
  if (!pressed) {
    return;
  }
  
  // Convert USB HID keycode to ASCII
  uint8_t asciiKey = USB_HID_TO_ASCII[usbHidKeycode];
  
  // Ignore keys with no ASCII mapping
  if (asciiKey != 0x00) {
    // Update both ports with the same data
    asciiDataA = asciiKey;
    asciiDataB = asciiKey;
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
}

bool GPIOKeyboardEncoderAttachment::hasDataReadyA() const {
  return dataReadyA && enabledA;
}

bool GPIOKeyboardEncoderAttachment::hasDataReadyB() const {
  return dataReadyB && enabledB;
}
