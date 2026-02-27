#ifndef _RAM_CARD_H
#define _RAM_CARD_H

#include "IO.h"
#include "constants.h"

#define RC_BLOCK_SIZE   0x400
#ifdef MEM_EXTMEM
#define RC_BLOCK_COUNT  0x100
#else
#define RC_BLOCK_COUNT  0x10
#endif

class RAMCard: public IO {
  private:
    uint8_t *data;
    uint8_t bank;

  public:
    RAMCard(uint8_t *data);
    ~RAMCard();

    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick(uint32_t cpuFrequency) override { return 0x00; }
    void    reset() override {}
};

#endif