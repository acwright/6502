#include "Emulator.h"

#ifdef DEVBOARD_0
Emulator::Emulator() {
  this->reset();
}

uint8_t Emulator::read(uint16_t address) {
  switch(address & 0x0003) {
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
  }

  return 0x00;
}

void Emulator::write(uint16_t address, uint8_t value) {
  switch(address & 0x0003) {
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
  }
}

uint8_t Emulator::tick() {
  return 0x00;
}

void Emulator::reset() {
  this->scratch = 0x00;
  this->gpioData = 0x00;
  this->gpioDataDir = 0x00;
}

#endif

#if DEVBOARD_1
Emulator::Emulator() {
  this->reset();
}

uint8_t Emulator::read(uint16_t address) {
  switch(address & 0x0003) {
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
  }

  return 0x00;
}

void Emulator::write(uint16_t address, uint8_t value) {
  switch(address & 0x0003) {
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
  }
}

uint8_t Emulator::tick() {
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

  return 0x00;
}

void Emulator::reset() {
  this->spiTransfer = 0x00;
  this->spiControl = 0x20;  // IE = DISABLED, DO = MSBFIRST, M = MODE0, CS0-2 = LOW
  this->spiStatus = 0x20;   // IE = DISABLED, DO = MSBFIRST, M = MODE0, CS0-2 = LOW
  this->spiClock = 0x04;    // 4MHz
  this->spiTransferPending = false;
}
#endif