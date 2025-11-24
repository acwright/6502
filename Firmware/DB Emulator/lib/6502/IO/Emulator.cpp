#include "Emulator.h"

Emulator::Emulator() {
  this->reset();
}

Emulator::~Emulator() {
  delete[] this->pramData;
}

uint8_t Emulator::read(uint16_t address) {
  time_t time = now();

  switch(address & 0x0F) {
    case 0x00: {
      uint8_t data = this->serialData;
      this->serialStatus &= ~EMU_SER_STATUS_DA; // Reading from serial data clears data available bit
      return data;
    }
    case 0x01:
      return this->serialStatus;
    case 0x02: {
      uint8_t data = this->keyboardData; 
      this->keyboardData &= ~EMU_KEY_AVAILABLE; // Reading from KB data clears key available bit
      return data;
    }
    case 0x03:
      return this->mouseXData;
    case 0x04:
      return this->mouseYData;
    case 0x05:
      return this->mouseWData;
    case 0x06:
      return this->mouseBtnsData;
    case 0x07:
      return this->joystickData;
    case 0x08:
      return second(time);
    case 0x09:
      return minute(time);
    case 0x0A:
      return hour(time);
    case 0x0B:
      return day(time);
    case 0x0C:
      return month(time);
    case 0x0D:
      return year(time) % 100; // Get the last two digits of year
    case 0x0E: 
      return this->pramData[this->pramAddress];
    case 0x0F:
      return this->pramAddress;
  }

  return 0x00;
}

void Emulator::write(uint16_t address, uint8_t value) {
  switch(address & 0x0F) {
    case 0x00:
      transmitSerial(value);
      break;
    case 0x01:
      configureSerial(value);
      break;
    case 0x02:
      this->keyboardCmd = value;
      break;
    case 0x0E: {
      this->pramData[this->pramAddress] = value;
      
      File file = SD.open(EMU_PRAM_FILE_NAME, FILE_WRITE);
      file.seek(this->pramAddress);
      file.write(this->pramData[this->pramAddress]);
      file.close();

      break;
    }
    case 0x0F:
      this->pramAddress = value;
      break;
    default:
      break;
  }
}

uint8_t Emulator::tick() {
  uint8_t result = 0x00;

  // TODO: Check for incoming data depending on serial target
  switch (this->target) {
    case EMU_TARGET_SERIAL:
      break; // Do Nothing
    case EMU_TARGET_SPI:
      break; // Do Nothing
    case EMU_TARGET_I2C:
      if (this->i2cRead) {
        #ifdef DEVBOARD_0
        if (Wire1.available() && (this->serialStatus & EMU_SER_STATUS_DA) == 0) {
          this->serialData = Wire1.read();
          this->serialStatus |= EMU_SER_STATUS_DA;
        }
        #endif
        #ifdef DEVBOARD_1_1
        if (Wire2.available() && (this->serialStatus & EMU_SER_STATUS_DA) == 0) {
          this->serialData = Wire2.read();
          this->serialStatus |= EMU_SER_STATUS_DA;
        }
        #endif
      }
      break;
    case EMU_TARGET_TXRX:
      #ifdef DEVBOARD_0
      if (Serial4.available() && (this->serialStatus & EMU_SER_STATUS_DA) == 0) {
        this->serialData = Serial4.read();
        this->serialStatus |= EMU_SER_STATUS_DA;
      }
      #endif
      #ifdef DEVBOARD_1
      if (Serial7.available() && (this->serialStatus & EMU_SER_STATUS_DA) == 0) {
        this->serialData = Serial7.read();
        this->serialStatus |= EMU_SER_STATUS_DA;
      }
      #endif
      #ifdef DEVBOARD_1_1
      if (Serial6.available() && (this->serialStatus & EMU_SER_STATUS_DA) == 0) {
        this->serialData = Serial6.read();
        this->serialStatus |= EMU_SER_STATUS_DA;
      }
      #endif
      break;
    default:
      break;
  }

  // Keyboard Interrupt
  result |= (this->keyboardCmd & EMU_KEY_INT) & (this->keyboardData & EMU_KEY_AVAILABLE);

  // Serial Interrupt
  result |= (this->serialControl & EMU_SER_CTRL_IE) & (this->serialStatus & EMU_SER_STATUS_DA);

  return result;
}

void Emulator::reset() {
  this->serialData = 0x00;
  this->serialControl = 0x00;
  this->serialStatus = 0x00;
  this->keyboardCmd = 0x00;
  this->keyboardData = 0x00;
  this->mouseXData = 0x00;
  this->mouseYData = 0x00;
  this->mouseWData = 0x00;
  this->mouseBtnsData = 0x00;
  this->joystickData = 0x00;
  
  this->pramData = new uint8_t[EMU_PRAM_SIZE];
  this->pramAddress = 0x00;

  this->target = EMU_TARGET_SERIAL;

  this->i2cAddress = 0x00;
  this->i2cRead = false;
  
  if (!SD.exists(EMU_PRAM_FILE_NAME)) {
    File file = SD.open(EMU_PRAM_FILE_NAME, FILE_WRITE);
    for (int i = 0; i < EMU_PRAM_SIZE; i++) {
      file.write(0x00);
    }
    file.close();
  }

  File file = SD.open(EMU_PRAM_FILE_NAME);
  for (int i = 0; i < EMU_PRAM_SIZE; i++) {
    this->pramData[i] = file.read();
  }
  file.close();
}

void Emulator::updateKeyboard(uint8_t ascii) {
  this->keyboardData = ascii & ~EMU_KEY_AVAILABLE;  // Mask out ascii value (0-127)
  this->keyboardData |= EMU_KEY_AVAILABLE;         // Set key available bit
}

void Emulator::updateMouse(int mouseX, int mouseY, int mouseWheel, uint8_t mouseButtons) {
  this->mouseXData = mouseX;
  this->mouseYData = mouseY;
  this->mouseWData = mouseWheel;
  this->mouseBtnsData = mouseButtons;
}

void Emulator::updateJoystick(uint32_t buttons) {
  // XBOX Button Values
  // LFUNC (View): 0x8 1000
  // RFUNC (Menu): 0x4 0100
  // A: 0x10 0001 0000
  // B: 0x20 0010 0000
  // X: 0x40 0100 0000
  // Y: 0x80 1000 0000
  // UP: 0x100 0001 0000 0000
  // DOWN: 0x200 0010 0000 0000
  // LEFT: 0x400 0100 0000 0000
  // RIGHT: 0x800 1000 0000 0000
  // LBTN: 0x1000 0001 0000 0000 0000
  // RBTN: 0x2000 0010 0000 0000 0000
  // LABTN: 0x4000 0100 0000 0000 0000
  // RABTN: 0x8000 1000 0000 0000 0000

  uint8_t joystickLData = buttons & 0xFF;
  uint8_t joystickHData = (buttons >> 8) & 0xFF;
  this->joystickData  = (joystickLData & 0xF0) | (joystickHData & 0x0F);
}

// 
// PRIVATE METHODS
//

void Emulator::configureSerial(uint8_t value) {
  this->serialControl = value;
  
  uint8_t mode = value & 0b00001111; // Mask out the mode
  uint8_t target = (value & (EMU_SER_CTRL_T0 | EMU_SER_CTRL_T1)) >> 4; // Mask out the target

  if (target != this->target) { // New target selected...
    switch (this->target) { // So... End old target
      case EMU_TARGET_SERIAL:
        break;
      case EMU_TARGET_SPI:
        #ifdef DEVBOARD_1
        modeToSPIChipSelect(mode);
        SPI1.endTransaction();
        SPI1.end();
        #endif
        #ifdef DEVBOARD_1_1
        modeToSPIChipSelect(mode);
        SPI1.endTransaction();
        SPI1.end();
        #endif
        break;
      case EMU_TARGET_I2C:
        #ifdef DEVBOARD_0
        if (!this->i2cRead) {
          Wire1.endTransmission();
        }
        Wire1.end();
        this->i2cAddress = 0x00;
        this->i2cRead = false;
        #endif
        #ifdef DEVBOARD_1_1
        if (this->i2cReadWrite == EMU_I2C_WRITE) {
          Wire2.endTransmission();
        }
        Wire2.end();
        this->i2cAddress = 0x00;
        this->i2cRead = false;
        #endif
        break;
      case EMU_TARGET_TXRX:
        #ifdef DEVBOARD_0
        Serial4.clear();
        Serial4.end();
        #endif
        #ifdef DEVBOARD_1
        Serial7.clear();
        Serial7.end();
        #endif
        #ifdef DEVBOARD_1_1
        Serial6.clear();
        Serial6.end();
        #endif
        break;
      default:
        break;
    }

    this->target = target;

    switch (this->target) { // And... Start new target
      case EMU_TARGET_SERIAL:
        break;
      case EMU_TARGET_SPI:
        #ifdef DEVBOARD_1
        SPI1.begin();
        SPI1.beginTransaction(SPISettings(modeToSPISpeed(mode), MSBFIRST, SPI_MODE0));
        modeToSPIChipSelect(mode);
        #endif
        #ifdef DEVBOARD_1_1
        SPI1.begin();
        SPI1.beginTransaction(SPISettings(modeToSPISpeed(mode), MSBFIRST, SPI_MODE0));
        modeToSPIChipSelect(mode);
        #endif
        break;
      case EMU_TARGET_I2C:
        #ifdef DEVBOARD_0
        Wire1.begin();
        Wire1.setClock(modeToI2CSpeed(mode));
        #endif
        #ifdef DEVBOARD_1_1
        Wire2.begin();
        Wire2.setClock(modeToI2CSpeed(mode));
        #endif
        break;
      case EMU_TARGET_TXRX:
        #ifdef DEVBOARD_0
        Serial4.begin(modeToBaudRate(mode));
        #endif
        #ifdef DEVBOARD_1
        Serial7.begin(modeToBaudRate(mode));
        #endif
        #ifdef DEVBOARD_1_1
        Serial6.begin(modeToBaudRate(mode));;
        #endif
        break;
      default:
        break;
    }
  }

  #ifdef DEVBOARD_1_1
  digitalWriteFast(CE, (value & EMU_SER_CTRL_CE) >> 6 ? LOW : HIGH);
  #endif
}

void Emulator::transmitSerial(uint8_t value) {
  this->serialData = value;

  switch (this->target) {
    case EMU_TARGET_SERIAL:
      Serial.print((char)value);
      break;
    case EMU_TARGET_SPI:
      this->serialData = SPI1.transfer(value);
      this->serialStatus |= EMU_SER_STATUS_DA;
      break;
    case EMU_TARGET_I2C:
      // First byte written to serial data register after I2C configured sets address and R/W 
      if (this->i2cAddress == 0x00) {
        this->i2cAddress = (value & 0b11111110) >> 1;
        this->i2cRead = value & 0b00000001;
        #ifdef DEVBOARD_0
        if (!this->i2cRead) {
          Wire1.beginTransmission(this->i2cAddress);
        }
        #endif
        #ifdef DEVBOARD_1_1
        if (!this->i2cRead) {
          Wire2.beginTransmission(this->i2cAddress);
        }
        #endif
      // Subsequent bytes are written if writing to I2C or sets number of bytes to read
      } else {
        #ifdef DEVBOARD_0
        if (this->i2cRead) {
          Wire1.requestFrom(this->i2cAddress, value);
        } else {
          Wire1.write(value);
        }
        #endif
        #ifdef DEVBOARD_1_1
        if (this->i2cRead) {
          Wire2.requestFrom(this->i2cAddress, value);
        } else {
          Wire2.write(value);
        }
        #endif
      }
      break;
    case EMU_TARGET_TXRX:
      Serial6.write(value);
      break;
    default:
      break;
  }
}

void Emulator::modeToSPIChipSelect(uint8_t mode) {
  #ifdef DEVBOARD_1
  uint8_t chipSelect = mode & 0b00000011;
  
  pinMode(CS0, OUTPUT);
  pinMode(CS1, OUTPUT);
  pinMode(CS2, OUTPUT);

  switch (chipSelect) {
    case 0x00:
      digitalWriteFast(CS0, HIGH);
      digitalWriteFast(CS1, HIGH);
      digitalWriteFast(CS2, HIGH);
      break;
    case 0x01:
      digitalWriteFast(CS0, LOW);
      break;
    case 0x02:
      digitalWriteFast(CS1, LOW);
      break;
    case 0x03:
      digitalWriteFast(CS2, LOW);
      break;
    default:
      break; 
  }
  #endif
  #ifdef DEVBOARD_1_1
  uint8_t chipSelect = mode & 0b00000011;

  pinMode(CS0, OUTPUT);
  pinMode(CS1, OUTPUT);
  pinMode(CS2, OUTPUT);

  switch (chipSelect) {
    case 0x00:
      digitalWriteFast(CS0, HIGH);
      digitalWriteFast(CS1, HIGH);
      digitalWriteFast(CS2, HIGH);
      break;
    case 0x01:
      digitalWriteFast(CS0, LOW);
      break;
    case 0x02:
      digitalWriteFast(CS1, LOW);
      break;
    case 0x03:
      digitalWriteFast(CS2, LOW);
      break;
    default:
      break; 
  }
  #endif
}

uint32_t Emulator::modeToSPISpeed(uint8_t mode) {
  uint8_t speed = (mode & 0b00001100) >> 2;

  switch (speed) {
    case 0x00:
      return 2000000;   // 2MHz
    case 0x01:
      return 4000000;   // 4MHz
    case 0x02:
      return 10000000;  // 10MHz
    case 0x03:
      return 25000000;  // 25MHz
    default:
      return 2000000;   // 2MHz 
  }
}

uint32_t Emulator::modeToI2CSpeed(uint8_t mode) {
  switch (mode) {
    case 0x00:
      return 100000; // Normal Mode
    case 0x01:
      return 400000; // Fast Mode
    case 0x02:
      return 1000000; // Fast Mode Plus
    case 0x03:
      return 3400000; // High Speed Mode
    case 0x04:
      return 10000; // Low Speed Mode
    default:
      return 100000; // Normal Mode
  }
}

uint32_t Emulator::modeToBaudRate(uint8_t mode) {
  switch (mode) {
    case 0x00:
      return 115200;
    case 0x01:
      return 50;
    case 0x02:
      return 75;
    case 0x03:
      return 110;
    case 0x04:
      return 135;
    case 0x05:
      return 150;
    case 0x06:
      return 300;
    case 0x07:
      return 600;
    case 0x08:
      return 1200;
    case 0x09:
      return 1800;
    case 0x0A:
      return 2400;
    case 0x0B:
      return 3600;
    case 0x0C:
      return 4800;
    case 0x0D:
      return 7200;
    case 0x0E:
      return 9600;
    case 0x0F:
      return 19200;
    default:
      return 115200;
  }
}