#ifndef _RAM_H
#define _RAM_H

#include <Arduino.h>
#include "constants.h"

class RAM {
  private:
    uint8_t *data;

  public:
    RAM();
    ~RAM();

    bool enabled = true;

    uint8_t read(uint16_t index);
    void write(uint16_t index, uint8_t value);
};

#endif