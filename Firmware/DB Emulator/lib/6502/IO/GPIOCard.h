#ifndef _GPIO_CARD_H
#define _GPIO_CARD_H

#include "IO.h"
#include "constants.h"

class GPIOCard: public IO {
  private:

  public:
    GPIOCard();
    ~GPIOCard() {}

    uint8_t id() { return IO_GPIO_CARD_KB; }
    String  description() override { return "GPIO Card"; }
    bool    passthrough() override { return false; }
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick() { return 0x00; }
    void    reset() override;

    void    updateJoystick(uint32_t buttons);
};

#endif