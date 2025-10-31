#include "SerialCard.h"

SerialCard::SerialCard() {}
uint8_t SerialCard::read(uint16_t address) { return 0x00; }
void SerialCard::write(uint16_t address, uint8_t value) {}

uint8_t SerialCard::tick() {
  if (SerialUSB1.available()) {
    // TODO: Do something with SerialUSB1 receive
  }

  return 0x00;
}

void SerialCard::reset() {}