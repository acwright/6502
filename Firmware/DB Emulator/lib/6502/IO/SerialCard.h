#ifndef _SERIAL_CARD_H
#define _SERIAL_CARD_H

#include "IO.h"
#include "constants.h"

#define SC_CMD_DTR                0b00000001
#define SC_CMD_IRD                0b00000010
#define SC_CMD_TIC0               0b00000100
#define SC_CMD_TIC1               0b00001000
#define SC_CMD_REM                0b00010000
#define SC_CMD_PME                0b00100000
#define SC_CMD_PMC0               0b01000000
#define SC_CMD_PMC1               0b10000000

#define SC_CTRL_SBR0              0b00000001
#define SC_CTRL_SBR1              0b00000010
#define SC_CTRL_SBR2              0b00000100
#define SC_CTRL_SBR3              0b00001000
#define SC_CTRL_RCS               0b00010000
#define SC_CTRL_WL0               0b00100000
#define SC_CTRL_WL1               0b01000000
#define SC_CTRL_SBN               0b10000000

#define SC_STATUS_PARITY_ERROR    0b00000001
#define SC_STATUS_FRAMING_ERROR   0b00000010
#define SC_STATUS_OVERRUN         0b00000100
#define SC_STATUS_RX_REG_FULL     0b00001000
#define SC_STATUS_TX_REG_EMPTY    0b00010000
#define SC_STATUS_DCD             0b00100000
#define SC_STATUS_DSR             0b01000000
#define SC_STATUS_IRQ             0b10000000

class SerialCard: public IO {
  private:
    // Registers
    uint8_t tx;
    uint8_t rx;
    uint8_t cmd;
    uint8_t ctrl;
    uint8_t status;

  public:
    SerialCard();
    ~SerialCard() {}

    uint8_t id() { return IO_SERIAL_CARD; }
    String  description() override { return "Serial Card"; }
    bool    passthrough() override { return false; }
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick() override;
    void    reset() override;
};

#endif