#ifndef _RAM_CARD_H
#define _RAM_CARD_H

#include "IO.h"

class RAMCard: public IO {
  public:
    RAMCard() {}
    ~RAMCard() {}

    String  description() override { return "RAM Card"; }
    bool    passthrough() override { return false; }
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick() { return 0x00; }
    void    reset() {}
};

#endif