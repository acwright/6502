#include "VideoCard.h"

VideoCard::VideoCard() {
  this->reset();
}

uint8_t VideoCard::read(uint16_t address) {
  switch (address & 0x01) {
    case 0x00: {
      // Data port read: return buffered value, then load next
      uint8_t val = this->readBuf;
      this->readBuf = this->vram[this->addr & 0x3FFF];
      this->addr = (this->addr + 1) & 0x3FFF;
      this->firstByte = true;
      return val;
    }
    case 0x01: {
      // Status register read: return status and clear frame flag
      uint8_t s = this->status;
      this->status &= ~0x80;
      this->firstByte = true;
      return s;
    }
    default:
      return 0x00;
  }
}

void VideoCard::write(uint16_t address, uint8_t value) {
  switch (address & 0x01) {
    case 0x00:
      // Data port write: write to VRAM and auto-increment
      this->vram[this->addr & 0x3FFF] = value;
      avSend(AV_TYPE_VIDEO_DATA, 0x00, value);
      this->addr = (this->addr + 1) & 0x3FFF;
      this->firstByte = true;
      break;
    case 0x01:
      // Control port: two-byte latch sequence
      if (this->firstByte) {
        this->latch = value;
        this->firstByte = false;
      } else {
        if (value & 0x80) {
          // Register write: latch is the data, value bits 0-2 are register number
          this->registers[value & 0x07] = this->latch;
          avSend(AV_TYPE_VIDEO_REG, value & 0x07, this->latch);
        } else {
          // Address set
          this->addr = this->latch | ((uint16_t)(value & 0x3F) << 8);
          avSend(AV_TYPE_VIDEO_ADDR, (this->addr >> 8) & 0x3F, this->addr & 0xFF);

          // If bit 6 is clear (read mode), pre-fill read buffer
          if (!(value & 0x40)) {
            this->readBuf = this->vram[this->addr & 0x3FFF];
            this->addr = (this->addr + 1) & 0x3FFF;
          }
        }
        this->firstByte = true;
      }
      break;
  }
}

uint8_t VideoCard::tick(uint32_t cpuFrequency) {
  // Recalculate cycles per frame if frequency changes
  if (this->cyclesPerFrame == 0 && cpuFrequency > 0) {
    this->cyclesPerFrame = cpuFrequency / 60;
  }

  this->cycleCount++;

  if (this->cyclesPerFrame > 0 && this->cycleCount >= this->cyclesPerFrame) {
    this->cycleCount = 0;
    this->status |= 0x80;  // Set frame interrupt flag

    // Generate IRQ if interrupt enable bit (R1 bit 5) is set
    if (this->registers[1] & 0x20) {
      return 0x80;  // IRQ
    }
  }

  return 0x00;
}

void VideoCard::reset() {
  memset(this->vram, 0, sizeof(this->vram));
  memset(this->registers, 0, sizeof(this->registers));
  this->status = 0;
  this->addr = 0;
  this->latch = 0;
  this->firstByte = true;
  this->readBuf = 0;
  this->cycleCount = 0;
  this->cyclesPerFrame = 0;
  avSend(AV_TYPE_RESET, 0x00, 0x00);
}
