#ifndef _INPUT_BOARD_H
#define _INPUT_BOARD_H

#include "IO.h"
#include "constants.h"

class InputBoard: public IO {
  private:

  public:
    InputBoard();
    ~InputBoard() {}

    uint8_t id() { return IO_INPUT_BOARD; }
    String  description() override { return "Input Board"; }
    bool    passthrough() override { return false; }
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick() { return 0x00; }
    void    reset() override;

    void    updateMouse(int mouseX, int mouseY, int mouseW, uint8_t mouseButtons);
};

#endif