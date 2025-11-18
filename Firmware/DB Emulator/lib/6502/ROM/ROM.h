#ifndef _ROM_H
#define _ROM_H

#include <Arduino.h>
#include "constants.h"

class ROM {
  public:
    ROM();
    ~ROM();

    uint8_t *data;
    bool enabled = true;
    String file = "None";
    
    uint8_t read(uint16_t index);
    void write(uint16_t index, uint8_t value);
};

#endif