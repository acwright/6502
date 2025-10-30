#ifndef _RAM_H
#define _RAM_H

#include <Arduino.h>

class RAM {
  private:
    uint16_t size;
    uint8_t *data;

  public:
    RAM(uint16_t size);
    ~RAM();

    bool enabled = true;

    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t value);
};

#endif