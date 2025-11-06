#include "StorageCard.h"

StorageCard::StorageCard() {
  this->reset();
}

uint8_t StorageCard::read(uint16_t address) {
  switch(address & 0x0007) {
    case 0x00: // Data Register
      return this->readBuffer();
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
      this->writeBuffer(value);
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
      this->executeCommand();
      break;
  }
}

void StorageCard::reset() {
  this->bufferIndex = 0x0000;
  this->commandDataSize = ST_SECTOR_SIZE;
  this->sectorOffset = 0;

  this->error = 0x00;
  this->feature = 0x00;
  this->sectorCount = 0x00;
  this->lba0 = 0x00;
  this->lba1 = 0x00;
  this->lba2 = 0x00;
  this->lba3 = 0xE0;
  this->status = 0x00 | ST_STATUS_RDY;
  this->command = 0x00;

  this->isTransferring = false;

  for (int i = 0; i < 0x200; i++) {
    this->buffer[i] = 0x00;
  }
}

//
// Private
//

void StorageCard::executeCommand() {
  // New command so clear errors and flags
  this->status &= ~ST_STATUS_ERR;
  this->status &= ~ST_STATUS_DRQ;
  this->error = 0x00;
  this->commandDataSize = ST_SECTOR_SIZE * this->sectorCount;
  this->bufferIndex = 0;
  this->sectorOffset = 0;

  switch (this->command) {
    case 0xC0: // Erase sector
      break;
    case 0xEC: // Identify drive
      if (!SD.mediaPresent()) {
        // Abort with general error
        this->status |= ST_STATUS_ERR;
        this->error |= ST_ERR_AMNF;
      } else if (this->isTransferring) {
        // Abort with error if already executing a command
        this->status |= ST_STATUS_ERR;
        this->error |= ST_ERR_ABRT;
      } else {
        this->isIdentifying = true;
        this->commandDataSize = 0x100; // Identify drive fills buffer with 256 bytes of identity data

        if (SD.exists(ST_IDENTITY_FILE_NAME)) {
          File identity = SD.open(ST_IDENTITY_FILE_NAME);

          for (int i = 0; i < ST_SECTOR_SIZE; i++) {
            if (i < 0x100) {
              this->buffer[i] = identity.read(); // Read 256 bytes into buffer
            } else {
              this->buffer[i] = 0x00; // Fill the rest with zeros
            }
          }
          
          identity.close();
        } else {
          for (int i = 0; i < 0x200; i++) {
            this->buffer[i] = 0x00; // Fill the buffer with zeros
          }
        }
      }
      break;
    case 0x20: // Read sector
    case 0x21:
      // Check to see if media present if not abort with error
      if (!SD.mediaPresent()) {
        // Abort with general error
        this->status |= ST_STATUS_ERR;
        this->error |= ST_ERR_AMNF;
      } else if (this->isTransferring || this->isIdentifying) {
        // Abort with error if already executing a command
        this->status |= ST_STATUS_ERR;
        this->error |= ST_ERR_ABRT;
      } else if (!this->sectorValid()) {
        // Not in sector range so abort with IDNF flag
        this->status |= ST_STATUS_ERR;
        this->error |= ST_ERR_ABRT | ST_ERR_IDNF;
      } else {
        // Otherwise load the buffer with first sector and set the DRQ flag and continue
        File storage = this->openStorage(false);
        storage.seek(this->sectorIndex() * 512);

        for(int i = 0; i < ST_SECTOR_SIZE; i++) {
          this->buffer[i] = storage.read();
        }

        storage.close();

        this->status |= ST_STATUS_DRQ;
        this->isTransferring = true;
      }
      break;
    case 0xEF: // Set features
      break; // We don't support setting features but we accept them without error
    case 0x30: // Write sector
    case 0x31:
      // Check to see if media present if not abort with error
      if (!SD.mediaPresent()) {
        // Abort with general error
        this->status |= ST_STATUS_ERR;
        this->error |= ST_ERR_AMNF;
      } else if (this->isTransferring || this->isIdentifying) {
        // Abort with error if already executing a command
        this->status |= ST_STATUS_ERR;
        this->error |= ST_ERR_ABRT;
      } else if (!this->sectorValid()) {
        // Not in sector range so abort with IDNF flag
        this->status |= ST_STATUS_ERR;
        this->error |= ST_ERR_ABRT | ST_ERR_IDNF;
      } else {
        // Otherwise set the DRQ flag and continue
        this->status |= ST_STATUS_DRQ;
        this->isTransferring = true;
      }
      break;
    default:
      // All other commands are unsupported so abort with error
      this->status |= ST_STATUS_ERR;
      this->error |= ST_ERR_ABRT;
      break;
  }
}

uint8_t StorageCard::readBuffer() {
  if (this->isIdentifying) {
    uint8_t data = this->buffer[this->bufferIndex];

    if (this->bufferIndex < this->commandDataSize - 1) {
      this->bufferIndex++;
    } else {
      this->bufferIndex = 0;
      this->isIdentifying = false;
    }

    return data;
  } else if (this->isTransferring) {
    uint8_t data = this->buffer[this->bufferIndex];

    if (this->bufferIndex < ST_SECTOR_SIZE - 1) {
      this->bufferIndex++;
    } else {
      this->bufferIndex = 0;
      this->sectorOffset++;

      if (this->sectorOffset < this->sectorCount) { // Load the next sector
        File storage = this->openStorage(false);
        storage.seek(this->sectorIndex() * 512 + (512 * this->sectorOffset));

        for(int i = 0; i < ST_SECTOR_SIZE; i++) {
          this->buffer[i] = storage.read();
        }

        storage.close();
      } else {
        this->isTransferring = false;
      }
    }

    return data;
  } else {
    return 0x00;
  }
}

void StorageCard::writeBuffer(uint8_t value) {
  this->buffer[this->bufferIndex] = value;

  if (this->bufferIndex < this->commandDataSize - 1) {
    this->bufferIndex++;
  } else {
    this->bufferIndex = 0;

    if (this->sectorOffset < this->sectorCount) { // Write the next sector
      File file = this->openStorage(true);
      file.seek(this->sectorIndex() * 512 + (512 * this->sectorOffset));

      for (int i = 0; i < ST_SECTOR_SIZE; i++) {
        file.write(buffer[i]);
      }

      file.close();

      this->sectorOffset++;
    } else {
      this->isTransferring = false;
    }
  }
}

File StorageCard::openStorage(bool isWriting) {
  File file;

  if (SD.exists(ST_STORAGE_FILE_NAME)) {
    isWriting ? file = SD.open(ST_STORAGE_FILE_NAME, FILE_WRITE) : file = SD.open(ST_STORAGE_FILE_NAME);
  } else {
    file = SD.open(ST_STORAGE_FILE_NAME, FILE_WRITE);
    
    for (int i = 0; i < ST_STORAGE_SIZE; i++) {
      file.write(0x00);
    }
    file.seek(0);

    if (!isWriting) {
      file.close();
      file = SD.open(ST_STORAGE_FILE_NAME);
    }
  }

  return file;
}

uint32_t StorageCard::sectorIndex() {
  return ((this->lba3 & 0x0F) << 24) | (this->lba2 << 16) | (this->lba1 << 8) | this->lba0;
}

bool StorageCard::sectorValid() {
  return this->sectorIndex() < ST_SECTOR_COUNT;
}