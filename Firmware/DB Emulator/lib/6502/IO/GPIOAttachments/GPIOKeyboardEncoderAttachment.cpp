#include "GPIOKeyboardEncoderAttachment.h"

GPIOKeyboardEncoderAttachment::GPIOKeyboardEncoderAttachment(uint8_t priority)
  : GPIOAttachment(priority, false, false, true, true),  // Uses CB1, CB2
    asciiData(0x00), dataReady(false), interruptPending(false) {
  reset();
}

void GPIOKeyboardEncoderAttachment::reset() {
  GPIOAttachment::reset();
  asciiData = 0x00;
  dataReady = false;
  interruptPending = false;
}

uint8_t GPIOKeyboardEncoderAttachment::readPortB(uint8_t ddrB, uint8_t orB) {
  // Only provide data when enabled and data is ready
  if (enabled && dataReady) {
    // Reading the port will clear data ready flag (done via clearInterrupts)
    return asciiData;
  }
  
  // No data to provide
  return 0xFF;
}

void GPIOKeyboardEncoderAttachment::updateControlLines(bool ca1, bool ca2, bool cb1, bool cb2) {
  stateCA1 = ca1;
  stateCA2 = ca2;
  stateCB1 = cb1;
  stateCB2 = cb2;
  
  // Enabled when CB2 is LOW
  enabled = !cb2;
}

bool GPIOKeyboardEncoderAttachment::hasCB1Interrupt() const {
  return interruptPending && enabled;
}

void GPIOKeyboardEncoderAttachment::clearInterrupts(bool ca1, bool ca2, bool cb1, bool cb2) {
  if (cb1) {
    interruptPending = false;
    dataReady = false;  // Clear data ready flag when port is read
  }
}

void GPIOKeyboardEncoderAttachment::updateKeyboard(uint8_t ascii) {
  if (ascii != 0x00) {  // Ignore null characters
    asciiData = ascii;
    dataReady = true;
    
    // Trigger CB1 interrupt if enabled
    if (enabled) {
      interruptPending = true;
    }
  }
}

bool GPIOKeyboardEncoderAttachment::hasDataReady() const {
  return dataReady && enabled;
}
