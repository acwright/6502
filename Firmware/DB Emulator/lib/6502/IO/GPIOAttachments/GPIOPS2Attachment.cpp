#include "GPIOPS2Attachment.h"

GPIOPS2Attachment::GPIOPS2Attachment(uint8_t priority)
  : GPIOAttachment(priority, true, true, false, false),  // Uses CA1, CA2
    asciiData(0x00), dataReady(false), interruptPending(false) {
  reset();
}

void GPIOPS2Attachment::reset() {
  GPIOAttachment::reset();
  asciiData = 0x00;
  dataReady = false;
  interruptPending = false;
}

uint8_t GPIOPS2Attachment::readPortA(uint8_t ddrA, uint8_t orA) {
  // Only provide data when enabled and data is ready
  if (enabled && dataReady) {
    // Reading the port will clear data ready flag (done via clearInterrupts)
    return asciiData;
  }
  
  // No data to provide
  return 0xFF;
}

void GPIOPS2Attachment::updateControlLines(bool ca1, bool ca2, bool cb1, bool cb2) {
  stateCA1 = ca1;
  stateCA2 = ca2;
  stateCB1 = cb1;
  stateCB2 = cb2;
  
  // Enabled when CA2 is LOW
  enabled = !ca2;
}

bool GPIOPS2Attachment::hasCA1Interrupt() const {
  return interruptPending && enabled;
}

void GPIOPS2Attachment::clearInterrupts(bool ca1, bool ca2, bool cb1, bool cb2) {
  if (ca1) {
    interruptPending = false;
    dataReady = false;  // Clear data ready flag when port is read
  }
}

void GPIOPS2Attachment::updatePS2Keyboard(uint8_t ascii) {
  if (ascii != 0x00) {  // Ignore null characters
    asciiData = ascii;
    dataReady = true;
    
    // Trigger CA1 interrupt if enabled
    if (enabled) {
      interruptPending = true;
    }
  }
}

bool GPIOPS2Attachment::hasDataReady() const {
  return dataReady && enabled;
}
