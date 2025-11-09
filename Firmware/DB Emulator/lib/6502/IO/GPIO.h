#ifndef _GPIO_H
#define _GPIO_H

#include "IO.h"
#include "constants.h"

#define GPIO_ATTACHMENT_GPIO_HELPER     0
#define GPIO_ATTACHMENT_INPUT_BOARD     1
#define GPIO_ATTACHMENT_KEYBOARD_HELPER 2
#define GPIO_ATTACHMENT_KEYPAD_HELPER   3
#define GPIO_ATTACHMENT_LCD_CARD        4

class GPIO: public IO {
  private:
    uint8_t attachment;

  public:
    GPIO(uint8_t attachment);
    ~GPIO() {}

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