#ifndef _RAM_CARD_H
#define _RAM_CARD_H

#include "IO.h"
#include "constants.h"

class RAMCard: public IO {
  private:
    #ifdef MEM_EXTMEM
    EXTMEM uint8_t *data;
    #else
    uint8_t *data;
    #endif
    
    uint8_t bank;

  public:
    RAMCard();
    ~RAMCard();

    uint8_t id() { return IO_RAM_CARD; }
    String  description() override { return "RAM Card"; }
    bool    passthrough() override { return false; }
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick() { return 0x00; }
    void    reset() {}
};

#endif