#ifndef _IO_H
#define _IO_H

#include <Arduino.h>

class IO {
  public:
    virtual ~IO() = default;

    virtual uint8_t read(uint16_t address) = 0;
    virtual void    write(uint16_t address, uint8_t value) = 0;
    virtual uint8_t tick(uint32_t cpuFrequency) = 0;
    virtual void    reset() = 0;
};

#endif