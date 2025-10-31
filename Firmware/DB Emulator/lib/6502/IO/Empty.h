#ifndef _EMPTY_H
#define _EMPTY_H

#include "IO.h"
#include "constants.h"

class Empty: public IO {
  public:
    Empty() {}
    ~Empty() {}

    uint8_t id() { return IO_EMPTY; }
    String  description() { return "Empty"; }
    bool    passthrough() { return true; }
    uint8_t read(uint16_t address) { return 0x00; }
    void    write(uint16_t address, uint8_t value) {}
    uint8_t tick() { return 0x00; }
    void    reset() {}
};

#endif