
#include "SerialCard.h"

SerialCard::SerialCard() {
  this->reset();
}

uint32_t SerialCard::getBaudRate() {
  // Extract baud rate select bits (SBR0-3)
  uint8_t sbrBits = this->ctrl & 0x0F;
  
  // Standard 6551 ACIA baud rate table
  static const uint32_t baudRates[16] = {
    115200, // 0000: 16x external clock (special case, using high speed)
    50,     // 0001: 50 baud
    75,     // 0010: 75 baud
    110,    // 0011: 109.92 baud (rounded to 110)
    135,    // 0100: 134.58 baud (rounded to 135)
    150,    // 0101: 150 baud
    300,    // 0110: 300 baud
    600,    // 0111: 600 baud
    1200,   // 1000: 1200 baud
    1800,   // 1001: 1800 baud
    2400,   // 1010: 2400 baud
    3600,   // 1011: 3600 baud
    4800,   // 1100: 4800 baud
    7200,   // 1101: 7200 baud
    9600,   // 1110: 9600 baud
    19200   // 1111: 19200 baud
  };
  
  return baudRates[sbrBits];
}

uint32_t SerialCard::getBitsPerByte() {
  // Start bit: always 1
  uint8_t bits = 1;
  
  // Data bits: determined by WL0 and WL1
  uint8_t wordLength = (this->ctrl & (SC_CTRL_WL0 | SC_CTRL_WL1)) >> 5;
  switch (wordLength) {
    case 0b00: bits += 8; break; // 8 data bits
    case 0b01: bits += 7; break; // 7 data bits
    case 0b10: bits += 6; break; // 6 data bits
    case 0b11: bits += 5; break; // 5 data bits
  }
  
  // Parity bit: determined by parity mode (PMC0, PMC1) in command register
  // If parity is enabled (not disabled/off), add 1 bit
  uint8_t parityMode = (this->cmd & (SC_CMD_PMC0 | SC_CMD_PMC1)) >> 6;
  if (parityMode != 0b00) {  // 00 = odd parity disabled in some modes
    // For simplicity, we'll assume parity adds a bit when not in mode 00
    // Actual behavior depends on PME (Parity Mode Enable) bit
    if ((this->cmd & SC_CMD_PME) != 0) {
      bits += 1;
    }
  }
  
  // Stop bits: determined by SBN (Stop Bit Number)
  if ((this->ctrl & SC_CTRL_SBN) != 0) {
    bits += 2; // 2 stop bits
  } else {
    bits += 1; // 1 stop bit
  }
  
  return bits;
}

uint32_t SerialCard::getCyclesPerByte(uint32_t cpuFrequency) {
  uint32_t baudRate = getBaudRate();
  uint32_t bitsPerByte = getBitsPerByte();
  
  // Calculate how many CPU cycles it takes to transmit/receive one byte
  // cycles = (cpuFrequency / baudRate) * bitsPerByte
  // Use 64-bit arithmetic to avoid overflow
  uint64_t cycles = ((uint64_t)cpuFrequency * (uint64_t)bitsPerByte) / (uint64_t)baudRate;
  
  return (uint32_t)cycles;
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
      this->txPending = true;
      this->txCycleCounter = 0; // Reset counter when new byte is queued
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

uint8_t SerialCard::tick(uint32_t cpuFrequency) {
  uint32_t cyclesPerByte = getCyclesPerByte(cpuFrequency);
  
  // Handle receiving data with baud rate timing
  if (SerialUSB1.available()) {
    rxCycleCounter++;
    
    // Check if enough cycles have passed to receive a byte
    if (rxCycleCounter >= cyclesPerByte) {
      rxCycleCounter = 0;
      
      if ((this->status & SC_STATUS_RX_REG_FULL) != 0x00) {
        // Overrun: new data arrived before previous data was read
        this->status |= SC_STATUS_OVERRUN;
        // Discard the new byte (don't read it from buffer yet)
      } else {
        this->rx = SerialUSB1.read();
        this->status |= SC_STATUS_RX_REG_FULL;

        // Receiver Echo Mode
        if ((this->cmd & SC_CMD_REM) != 0x00) {
          SerialUSB1.write(this->rx);
        }

        // Receiver Interrupt (if not disabled)
        if ((this->cmd & SC_CMD_IRD) == 0x00) {
          this->status |= SC_STATUS_IRQ;
        }
      }
    }
  } else {
    // Reset RX counter when no data available
    rxCycleCounter = 0;
  }

  // Handle transmitting data with baud rate timing
  if (txPending) {
    txCycleCounter++;
    
    // Check if enough cycles have passed to transmit a byte
    if (txCycleCounter >= cyclesPerByte) {
      SerialUSB1.write(this->tx);
      this->status |= SC_STATUS_TX_REG_EMPTY;
      txPending = false;
      txCycleCounter = 0;
      
      // Transmitter Interrupt based on TIC bits
      if ((this->cmd & SC_CMD_IRD) == 0x00) {
        uint8_t tic = (this->cmd & (SC_CMD_TIC0 | SC_CMD_TIC1)) >> 2;
        // TIC = 01 (0x01): Interrupt on TX register empty
        if (tic == 0x01) {
          this->status |= SC_STATUS_IRQ;
        }
      }
    }
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
  
  this->txCycleCounter = 0;
  this->rxCycleCounter = 0;
  this->txPending = false;
}