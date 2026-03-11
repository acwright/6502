#include "DevOutputBoard.h"

// HWSERIAL is defined in main.cpp as Serial4 or Serial7 depending on DEVBOARD
#ifdef DEVBOARD_0
#define HWSERIAL Serial4
#endif
#ifdef DEVBOARD_1
#define HWSERIAL Serial7
#endif

DevOutputBoard::DevOutputBoard() {
  this->reset();
}

uint8_t DevOutputBoard::read(uint16_t address) {
  if ((address & 0x1) == 0x00) {
    // Check if HWSERIAL is available for writing
    // If availableForWrite() returns 0, set bit 7 (busy flag)
    // if (HWSERIAL.availableForWrite() == 0) {
    //   return DOB_STATUS_BUSY;
    // } else {
    //   return 0x00;
    // }
  }
  
  return 0x00;
}

void DevOutputBoard::write(uint16_t address, uint8_t value) {
  if ((address & 0x1) == 0x00) {
    // Transmit data using HWSERIAL
    HWSERIAL.write(value);
  }
}

uint8_t DevOutputBoard::tick(uint32_t cpuFrequency) {
  // No IRQ generation for this simple device
  return 0x00;
}

void DevOutputBoard::reset() {
  // Nothing to reset for this simple device
}
