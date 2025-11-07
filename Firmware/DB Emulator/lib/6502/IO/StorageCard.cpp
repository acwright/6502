#include "StorageCard.h"

StorageCard::StorageCard() {
  this->reset();
}

void StorageCard::begin() {
  if (!SD.exists(ST_STORAGE_FILE_NAME)) {
    File file = SD.open(ST_STORAGE_FILE_NAME, FILE_WRITE);
    
    for (int i = 0; i < ST_STORAGE_SIZE; i++) {
      file.write(0x00);
    }

    file.close();
  }
  if (!SD.exists(ST_IDENTITY_FILE_NAME)) {
    File file = SD.open(ST_IDENTITY_FILE_NAME, FILE_WRITE);
    uint8_t identity[ST_SECTOR_SIZE];

    this->generateIdentity(identity);

    for (int i = 0; i < ST_SECTOR_SIZE; i++) {
      file.write(identity[i]);
    }

    file.close();
  }
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

  if (!SD.mediaPresent()) {
    // Abort with general error
    this->status |= ST_STATUS_ERR;
    this->error |= ST_ERR_AMNF;
    return;
  } else if (this->isTransferring || this->isIdentifying) {
    // Abort with error if already executing a command
    this->status |= ST_STATUS_ERR;
    this->error |= ST_ERR_ABRT;
    return;
  }

  switch (this->command) {
    case 0xC0: { // Erase sector
        File file = SD.open(ST_STORAGE_FILE_NAME, FILE_WRITE);

        if (file) {
          file.seek(this->sectorIndex() * 512);
          for (int i = 0; i < ST_SECTOR_SIZE; i++) {
            file.write(0x00);
          }
          file.close();
        }

        break;
      }
    case 0xEC: { // Identify drive
        File identity = SD.open(ST_IDENTITY_FILE_NAME);

        if (identity) {
          for (int i = 0; i < ST_SECTOR_SIZE; i++) {
            this->buffer[i] = identity.read();
          }
          identity.close();
        }

        this->commandDataSize = 0x200; // Identify drive fills buffer with 512 bytes of identity data
        this->status |= ST_STATUS_DRQ;
        this->isIdentifying = true;
        break;
      }
    case 0x20: // Read sector
    case 0x21:
      // Check to see if media present if not abort with error
      if (!this->sectorValid()) {
        // Not in sector range so abort with IDNF flag
        this->status |= ST_STATUS_ERR;
        this->error |= ST_ERR_ABRT | ST_ERR_IDNF;
      } else {
        // Otherwise load the buffer with first sector and set the DRQ flag and continue
        File storage = SD.open(ST_STORAGE_FILE_NAME);

        if (storage) {
          storage.seek(this->sectorIndex() * 512);
          for(int i = 0; i < ST_SECTOR_SIZE; i++) {
            this->buffer[i] = storage.read();
          }
          storage.close();
        }

        this->status |= ST_STATUS_DRQ;
        this->isTransferring = true;
      }
      break;
    case 0xEF: // Set features
      break; // We don't support setting features but we accept them without error
    case 0x30: // Write sector
    case 0x31:
      // Check to see if media present if not abort with error
      if (!this->sectorValid()) {
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
        File storage = SD.open(ST_STORAGE_FILE_NAME);

        if (storage) {
          storage.seek(this->sectorIndex() * 512 + (512 * this->sectorOffset));
          for(int i = 0; i < ST_SECTOR_SIZE; i++) {
            this->buffer[i] = storage.read();
          }
          storage.close();
        }
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
      File file = SD.open(ST_STORAGE_FILE_NAME, FILE_WRITE);

      if (file) {
        file.seek(this->sectorIndex() * 512 + (512 * this->sectorOffset));
        for (int i = 0; i < ST_SECTOR_SIZE; i++) {
          file.write(buffer[i]);
        }
        file.close();
      }

      this->sectorOffset++;
    } else {
      this->isTransferring = false;
    }
  }
}

uint32_t StorageCard::sectorIndex() {
  return ((this->lba3 & 0x0F) << 24) | (this->lba2 << 16) | (this->lba1 << 8) | this->lba0;
}

bool StorageCard::sectorValid() {
  return this->sectorIndex() < ST_SECTOR_COUNT;
}

void StorageCard::generateIdentity(uint8_t *identity) {
  // Generate emulated 128MB CF card identity
  // Some data taken from real Promaster 128MB CF card

  // Fill with zeros first
  memset(identity, 0, ST_SECTOR_SIZE);

  identity[0]   = 0x84;
  identity[1]   = 0x8A; // Removable Disk
  identity[2]   = 0x00;
  identity[3]   = 0x04; // # of cylinders
  identity[4]   = 0x00;
  identity[5]   = 0x00; // Reserved
  identity[6]   = 0x08;
  identity[7]   = 0x00; // # of heads
  identity[8]   = 0x00;
  identity[9]   = 0x40; // # of unformatted bytes per track
  identity[10]  = 0x00;
  identity[11]  = 0x02; // # of unformatted bytes per sector
  identity[12]  = 0x20;
  identity[13]  = 0x00; // # of sectors per track
  identity[14]  = 0x04;
  identity[15]  = 0x00;
  identity[16]  = 0x00;
  identity[17]  = 0x00; // # of sectors per card
  identity[18]  = 0x00;
  identity[19]  = 0x00; // Reserved

  identity[20]  = 'A';
  identity[21]  = 'C';
  identity[22]  = 'W';
  identity[23]  = 'D';
  identity[24]  = '6';
  identity[25]  = '5';
  identity[26]  = '0';
  identity[27]  = '2';
  identity[28]  = 'E';
  identity[29]  = 'M';
  identity[30]  = 'U';
  identity[31]  = 'C';
  identity[32]  = 'F';
  identity[33]  = '1';
  identity[34]  = '0';
  identity[35]  = '1';
  identity[36]  = '0';
  identity[37]  = '1';
  identity[38]  = '0';
  identity[39]  = '1'; // Serial # ACWD6502EMUCF1010101

  identity[40]  = 0x01;
  identity[41]  = 0x00; // Buffer type
  identity[42]  = 0x04;
  identity[43]  = 0x00; // Buffer size in 512 byte increments
  identity[44]  = 0x04;
  identity[45]  = 0x00; // # of ECC bytes passed

  identity[46]  = '1';
  identity[47]  = '.';
  identity[48]  = '0';
  identity[49]  = 0x20;
  identity[50]  = 0x20;
  identity[51]  = 0x20;
  identity[52]  = 0x20;
  identity[53]  = 0x20; // Firmware revision

  identity[54]  = 'A';
  identity[55]  = 'C';
  identity[56]  = 'W';
  identity[57]  = 'D';
  identity[58]  = '6';
  identity[59]  = '5';
  identity[60]  = '0';
  identity[61]  = '2';
  identity[62]  = 'E';
  identity[63]  = 'M';
  identity[64]  = 'U';
  identity[65]  = 'C';
  identity[66]  = 'F';
  identity[67]  = ' ';
  identity[68]  = ' ';
  identity[69]  = ' ';
  identity[70]  = ' ';
  identity[71]  = ' ';
  identity[72]  = ' ';
  identity[73]  = ' ';
  identity[74]  = ' ';
  identity[75]  = ' ';
  identity[76]  = ' ';
  identity[77]  = ' ';
  identity[78]  = ' ';
  identity[79]  = ' ';
  identity[80]  = ' ';
  identity[81]  = ' ';
  identity[82]  = ' ';
  identity[83]  = ' ';
  identity[84]  = ' ';
  identity[85]  = ' ';
  identity[86]  = ' ';
  identity[87]  = ' ';
  identity[88]  = ' ';
  identity[89]  = ' ';
  identity[90]  = ' ';
  identity[91]  = ' ';
  identity[92]  = ' ';
  identity[93]  = ' '; // Model # ACWD6502EMUCF

  identity[94]  = 0x01;
  identity[95]  = 0x00; // Max of 1 sector on R/W multiple cmd
  identity[96]  = 0x00;
  identity[97]  = 0x00; // Double word not supported
  identity[98]  = 0x00;
  identity[99]  = 0x02; // Capabilites: LBA supported
  identity[100] = 0x00;
  identity[101] = 0x00; // Reserved
  identity[102] = 0x00;
  identity[103] = 0x02; // PIO data transfer
  identity[104] = 0x00;
  identity[105] = 0x00; // DMA transfer cycle not supported
  identity[106] = 0x01;
  identity[107] = 0x00; // Field validity
  identity[108] = 0x00;
  identity[109] = 0x04; // Current # of cylinders
  identity[110] = 0x08;
  identity[111] = 0x00; // Current # of heads
  identity[112] = 0x20;
  identity[113] = 0x00; // Current sectors per track
  identity[114] = 0x00;
  identity[115] = 0x00;
  identity[116] = 0x04;
  identity[117] = 0x00; // Current capacity in sectors (LBAs)
  identity[118] = 0x01;
  identity[119] = 0x01; // Multiple sector setting
  identity[120] = 0x00;
  identity[121] = 0x00;
  identity[122] = 0x04;
  identity[123] = 0x00; // Total # of sectors in LBA mode
  
  // All zeros from here on out
}