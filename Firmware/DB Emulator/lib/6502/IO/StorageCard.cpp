#include "StorageCard.h"

StorageCard::StorageCard() {
  this->reset();
}

uint8_t StorageCard::read(uint16_t address) { 
  switch(address & 0x0007) {
    case 0x00: // Data Register
      uint8_t data = this->buffer[this->bufferIndex];
      if (this->bufferIndex < 0x1FF) {
        this->bufferIndex++;
      } else {
        this->bufferIndex = 0;
        // TODO: Load next sector into buffer if sectorCount > 1
      }
      return data;
    case 0x01: // Error Register
      return this->error;
    case 0x02: // Sector Count Register
      return this->sectorCount;
    case 0x03: // LBA0 Register
      return this->lba0;
    case 0x04: // LBA1 Register
      return this->lba1;
    case 0x05: // LBA2 Register
      return this->lba2;
    case 0x06: // LBA3 Register
      return this->lba3;
    case 0x07: // Status Register
      return this->status;
    default:
      return 0x00; // Shouldn't happen
  }
}

void StorageCard::write(uint16_t address, uint8_t value) {
  switch(address & 0x0007) {
    case 0x00: // Data Register
      this->buffer[this->bufferIndex] = value;
      if (this->bufferIndex < 0x1FF) {
        this->bufferIndex++;
      } else {
        this->bufferIndex = 0;
        // TODO: Save buffer to SD card
      }
      break;
    case 0x01: // Feature Register
      this->feature = value;
      break;
    case 0x02: // Sector Count Register
      this->sectorCount = value;
      break;
    case 0x03: // LBA0 Register
      this->lba0 = value;
      break;
    case 0x04: // LBA1 Register
      this->lba1 = value;
      break;
    case 0x05: // LBA2 Register
      this->lba2 = value;
      break;
    case 0x06: // LBA3 Register
      this->lba3 = (value & 0x0F) | 0xE0;
      break;
    case 0x07: // Command Register
      this->command = value;
      // TODO: Do command
      break;
  }
}

void StorageCard::reset() {
  this->bufferIndex = 0x0000;

  this->error = 0x00;
  this->feature = 0x00;
  this->sectorCount = 0x00;
  this->lba0 = 0x00;
  this->lba1 = 0x00;
  this->lba2 = 0x00;
  this->lba3 = 0xE0;
  this->status = 0x00 | ST_STATUS_RDY;
  this->command = 0x00;

  for (int i = 0; i < 0x200; i++) {
    this->buffer[i] = 0x00;
  }
}