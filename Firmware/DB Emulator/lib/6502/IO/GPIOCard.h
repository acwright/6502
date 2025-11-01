#ifndef _GPIO_CARD_H
#define _GPIO_CARD_H

#include "IO.h"
#include "constants.h"

#define GPIOCARD_ATTACHMENT_GPIO_HELPER   0
#define GPIOCARD_ATTACHMENT_KEYBOARD      1
#define GPIOCARD_ATTACHMENT_KEYPAD_HELPER 2

class GPIOCard: public IO {
  private:
    uint8_t attachment;

  public:
    GPIOCard(uint8_t attachment = GPIOCARD_ATTACHMENT_KEYBOARD);
    ~GPIOCard() {}

    uint8_t id() override;
    String  description() override;
    bool    passthrough() override { return false; }
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick() { return 0x00; }
    void    reset() override;

    void    updateJoystick(uint32_t buttons);
};

#endif