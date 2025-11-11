#include "Emulator.h"

Emulator::Emulator() {
  this->reset();
}

uint8_t Emulator::read(uint16_t address) {
  time_t time = now();

  switch(address & 0x000F) {
    #ifdef DEVBOARD_0
    case 0x00: // Print Data
      break;
    case 0x01: // Scratch Register
      return this->scratch;
    case 0x02: // GPIO Data Register
      this->gpioData &= ~EMU_GPIO0 | (digitalReadFast(GPIO0) << 0);
      this->gpioData &= ~EMU_GPIO1 | (digitalReadFast(GPIO1) << 1);
      this->gpioData &= ~EMU_GPIO2 | (digitalReadFast(GPIO2) << 2);
      this->gpioData &= ~EMU_GPIO3 | (digitalReadFast(GPIO3) << 3);
      return this->gpioData;
    case 0x03: // GPIO Data Direction Register
      return this->gpioDataDir;
    #endif

    #ifdef DEVBOARD_1
    case 0x00: // Print Data
      break;
    case 0x01: // SPI Transfer Register
      this->spiStatus &= ~EMU_SPI_STATUS_INT;
      return this->spiTransfer;
    case 0x02: // SPI Status Register
      this->spiStatus &= ~EMU_SPI_STATUS_INT;
      return this->spiStatus;
    case 0x03: // GPIO Data Direction Register
      return this->spiClock;
    #endif

    case 0x04: {
      uint8_t data = this->keyboardData; 
      this->keyboardData&= 0b01111111; // Reading from KB data clears key available bit
      return data;
    }
    case 0x05:
      return this->mouseXData;
    case 0x06:
      return this->mouseYData;
    case 0x07:
      return this->mouseWData;
    case 0x08:
      return this->mouseBtnsData;
    case 0x09:
      return this->joystickData;
    case 0x0A:
      return this->joystickLData;
    case 0x0B:
      return this->joystickHData;
    case 0x0C:
      return second(time);
    case 0x0D:
      return minute(time);
    case 0x0E:
      return hour(time);
    case 0x0F:
      return day(time);
    case 0x10:
      return month(time);
    case 0x11:
      return year(time) % 100; // Get the last two digits of year
  }

  return 0x00;
}

void Emulator::write(uint16_t address, uint8_t value) {
  switch(address & 0x000F) {
    #ifdef DEVBOARD_0
    case 0x00: // Print Data
      Serial.print((char)value);
      break;
    case 0x01: // Scratch Register
      this->scratch = value;
      break;
    case 0x02: // GPIO Data Register
      this->gpioData = 0x0F & value;
      digitalWriteFast(GPIO0, (this->gpioData & EMU_GPIO0) >> 0);
      digitalWriteFast(GPIO1, (this->gpioData & EMU_GPIO1) >> 1);
      digitalWriteFast(GPIO2, (this->gpioData & EMU_GPIO2) >> 2);
      digitalWriteFast(GPIO3, (this->gpioData & EMU_GPIO3) >> 3);
      break;
    case 0x03: // GPIO Data Direction Register
      this->gpioDataDir = 0x0F & value;
      pinMode(GPIO0, (this->gpioDataDir & EMU_GPIO0) >> 0);
      pinMode(GPIO1, (this->gpioDataDir & EMU_GPIO1) >> 1);
      pinMode(GPIO2, (this->gpioDataDir & EMU_GPIO2) >> 2);
      pinMode(GPIO3, (this->gpioDataDir & EMU_GPIO3) >> 3);
      break;
    #endif

    #ifdef DEVBOARD_1
    case 0x00: // Print Data
      Serial.print((char)value);
      break;
    case 0x01: // SPI Transfer Register
      this->spiTransfer = value;
      this->spiTransferPending = true;
      break;
    case 0x02: // SPI Control Register
      this->spiControl = value & ~EMU_SPI_CTRL_X;
      break;
    case 0x03: // GPIO Data Direction Register
      this->spiClock = value;
      break;
    #endif

    default:
      break;
  }
}

uint8_t Emulator::tick() {  
  #ifdef DEVBOARD_1
  if (this->spiTransferPending) {
    this->spiStatus |= EMU_SPI_STATUS_BSY;

    uint8_t bitOrder = (this->spiControl & EMU_SPI_CTRL_DO) >> 5;
    uint8_t mode = ((this->spiControl & EMU_SPI_CTRL_M0) >> 3) | ((this->spiControl & EMU_SPI_CTRL_M1) >> 3);

    SPI1.beginTransaction(SPISettings(this->spiClock * 1000000, bitOrder, mode));
    digitalWriteFast(CS0, !((this->spiControl & EMU_SPI_CTRL_CS0) >> 0));
    digitalWriteFast(CS1, !((this->spiControl & EMU_SPI_CTRL_CS1) >> 1));
    digitalWriteFast(CS2, !((this->spiControl & EMU_SPI_CTRL_CS2) >> 2));
    this->spiTransfer = SPI1.transfer(this->spiTransfer);
    digitalWriteFast(CS0, (this->spiControl & EMU_SPI_CTRL_CS0) >> 0);
    digitalWriteFast(CS1, (this->spiControl & EMU_SPI_CTRL_CS1) >> 1);
    digitalWriteFast(CS2, (this->spiControl & EMU_SPI_CTRL_CS2) >> 2);
    SPI1.endTransaction();

    this->spiTransferPending = false;
    this->spiStatus &= ~EMU_SPI_STATUS_BSY;

    if ((this->spiControl & EMU_SPI_CTRL_IE) > 0) {
      this->spiStatus |= EMU_SPI_STATUS_INT;
      return 0x80;
    }
  }
  #endif

  return 0x00;
}

void Emulator::reset() {
  #ifdef DEVBOARD_0
  this->scratch = 0x00;
  this->gpioData = 0x00;
  this->gpioDataDir = 0x00;
  #endif

  #ifdef DEVBOARD_1
  this->spiTransfer = 0x00;
  this->spiControl = 0x20;  // IE = DISABLED, DO = MSBFIRST, M = MODE0, CS0-2 = LOW
  this->spiStatus = 0x20;   // IE = DISABLED, DO = MSBFIRST, M = MODE0, CS0-2 = LOW
  this->spiClock = 0x04;    // 4MHz
  this->spiTransferPending = false;
  #endif

  this->keyboardData = 0x00;
  this->mouseXData = 0x00;
  this->mouseYData = 0x00;
  this->mouseWData = 0x00;
  this->mouseBtnsData = 0x00;
  this->joystickData = 0x00;
  this->joystickLData = 0x00;
  this->joystickHData = 0x00;
}

void Emulator::updateKeyboard(uint8_t ascii) {
  this->keyboardData = ascii & 0b01111111;  // Mask out ascii value (0-127)
  this->keyboardData |= 0b10000000;         // Set key available bit
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

  this->joystickLData = buttons & 0xFF;
  this->joystickHData = (buttons >> 8) & 0xFF;
  this->joystickData  = (this->joystickLData & 0xF0) | (this->joystickHData & 0x0F);
}