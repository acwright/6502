#include "SoundCard.h"

SoundCard::SoundCard() : v3Accumulator(0), v3NoiseShift(0x7FFFF8) {
  this->reset();
}

uint8_t SoundCard::read(uint16_t address) {
  uint8_t reg = address & 0x1F;

  if (reg == 0x1B) {
    // OSC3: live voice 3 waveform output (required for BIOS ProbeSID)
    uint8_t ctrl = this->registers[0x12];
    if (ctrl & 0x80) { // Noise
      return (this->v3NoiseShift >> 8) & 0xFF;
    }
    if (ctrl & 0x20) { // Sawtooth
      return (this->v3Accumulator >> 16) & 0xFF;
    }
    if (ctrl & 0x10) { // Triangle
      uint8_t msb = (this->v3Accumulator >> 23) & 1;
      uint8_t upper = (this->v3Accumulator >> 16) & 0xFF;
      return msb ? ~upper : upper;
    }
    if (ctrl & 0x40) { // Pulse
      uint16_t pw = ((this->registers[0x11] & 0x0F) << 8) | this->registers[0x10];
      uint16_t phase = (this->v3Accumulator >> 12) & 0xFFF;
      return phase < pw ? 0xFF : 0x00;
    }
    return 0x00;
  }

  if (reg == 0x1C) {
    // ENV3: envelope not simulated in firmware
    return 0x00;
  }

  // Paddle X/Y
  if (reg >= 0x19 && reg <= 0x1A) {
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
  // Advance voice 3 phase accumulator for OSC3 readback
  uint16_t freq = ((uint16_t)this->registers[0x0F] << 8) | this->registers[0x0E];
  uint32_t prevAcc = this->v3Accumulator;
  this->v3Accumulator = (this->v3Accumulator + freq) & 0xFFFFFF;

  // Clock noise LFSR on bit 19 transition (0->1)
  if (!(prevAcc & 0x080000) && (this->v3Accumulator & 0x080000)) {
    uint32_t bit0 = this->v3NoiseShift & 1;
    uint32_t bit1 = (this->v3NoiseShift >> 2) & 1;
    this->v3NoiseShift = (this->v3NoiseShift >> 1) | ((bit0 ^ bit1) << 22);
  }

  return 0x00;
}

void SoundCard::reset() {
  memset(this->registers, 0, sizeof(this->registers));
  this->v3Accumulator = 0;
  this->v3NoiseShift = 0x7FFFF8;
  avSend(AV_TYPE_RESET, 0x00, 0x00);
}
