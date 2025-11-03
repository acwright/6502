#include "SerialCard.h"

SerialCard::SerialCard() {
  this->reset();
}

uint8_t SerialCard::read(uint16_t address) {
  uint8_t _status;

  switch(address & 0x0003) {
    case 0x00: // Receive Data Register
      this->status &= ~SC_STATUS_IRQ; // Clear interrupt
      this->status &= ~SC_STATUS_RX_REG_FULL;
      return this->rx;
    case 0x01: // Status Register
      _status = this->status;
      this->status &= ~SC_STATUS_IRQ; // Clear interrupt
      return _status;
    case 0x02: // Command Register
      return this->cmd;
    case 0x03: // Control Register
      return this->ctrl;
    default:
      return 0x00; // Shouldn't happen
  }
}

void SerialCard::write(uint16_t address, uint8_t value) {
  switch(address & 0x0003) {
    case 0x00: // Transmit Data Register
      this->tx = value;
      this->status &= ~SC_STATUS_TX_REG_EMPTY;
      break;
    case 0x01: // Programmed Reset
      this->cmd &= 0b11100000;
      this->status &= ~SC_STATUS_OVERRUN;
      break;
    case 0x02: // Command Register
      this->cmd = value;
      break;
    case 0x03: // Control Register
      this->ctrl = value;
      break;
  }
}

uint8_t SerialCard::tick() {
  if (SerialUSB1.available() && (this->status & SC_STATUS_RX_REG_FULL) == 0x00) {
    this->rx = SerialUSB1.read();
    this->status |= SC_STATUS_RX_REG_FULL;

    // Receiver Echo Mode
    if ((this->cmd & SC_CMD_REM) != 0x00) {
      SerialUSB1.write(this->rx);
    }

    // Interrupt
    if ((this->cmd & SC_CMD_IRD) == 0x00) {
      this->status |= SC_STATUS_IRQ;
    }
  }

  if ((this->status & SC_STATUS_TX_REG_EMPTY) == 0) {
    SerialUSB1.write(this->tx);
    this->status |= SC_STATUS_TX_REG_EMPTY;
  }
  
  return this->status & SC_STATUS_IRQ;
}

void SerialCard::reset() {
  SerialUSB1.clear();
  
  this->tx = 0x00;
  this->rx = 0x00;
  this->cmd = 0x00;
  this->ctrl = 0x00;
  this->status = 0x00 | SC_STATUS_TX_REG_EMPTY;
}