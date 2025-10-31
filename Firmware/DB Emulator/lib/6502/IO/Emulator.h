#ifndef _EMULATOR_H
#define _EMULATOR_H

#include "IO.h"
#include "constants.h"

class Emulator: public IO {
  private:

  public:
    Emulator();
    ~Emulator() {}

    uint8_t id() { return IO_EMULATOR; }
    String  description() override { return "Emulator"; }
    bool    passthrough() override { return false; }
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick() { return 0x00; }
    void    reset() override;
};

#endif