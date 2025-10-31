#ifndef _SERIAL_CARD_H
#define _SERIAL_CARD_H

#include "IO.h"
#include "constants.h"

class SerialCard: public IO {
  private:

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