#include "SoundCard.h"

SoundCard::SoundCard() {
  this->reset();
}

uint8_t SoundCard::read(uint16_t address) {
  uint8_t reg = address & 0x1F;

  // Only registers $19-$1C are readable on the SID
  if (reg >= 0x19 && reg <= 0x1C) {
    return this->registers[reg];
  }

  return 0x00;
}

void SoundCard::write(uint16_t address, uint8_t value) {
  uint8_t reg = address & 0x1F;
  this->registers[reg] = value;
  avSend(AV_TYPE_SOUND, reg, value);
}

uint8_t SoundCard::tick(uint32_t cpuFrequency) {
  return 0x00;
}

void SoundCard::reset() {
  memset(this->registers, 0, sizeof(this->registers));
  avSend(AV_TYPE_RESET, 0x00, 0x00);
}
