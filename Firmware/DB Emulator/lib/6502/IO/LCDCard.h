#ifndef _LCD_CARD_H
#define _LCD_CARD_H

#include "IO.h"
#include "constants.h"

class LCDCard: public IO {
  private:

  public:
    LCDCard();
    ~LCDCard() {}

    uint8_t id() { return IO_LCD_CARD; }
    String  description() override { return "LCD Card"; }
    bool    passthrough() override { return false; }
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick() { return 0x00; }
    void    reset() override;
};

#endif