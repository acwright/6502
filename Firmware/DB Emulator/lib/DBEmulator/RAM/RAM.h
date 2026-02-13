#ifndef _RAM_H
#define _RAM_H

#include <Arduino.h>
#include "constants.h"

class RAM {
  public:
    RAM();
    ~RAM();

    uint8_t *data;
    bool enabled = true;
    String file = "None";

    uint8_t read(uint16_t index);
    void write(uint16_t index, uint8_t value);
};

#endif