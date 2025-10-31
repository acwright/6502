#ifndef _ROM_H
#define _ROM_H

#include <Arduino.h>
#include "constants.h"

class ROM {
  private:
    uint8_t *data;
    
  public:
    ROM();
    ~ROM();

    bool enabled = true;
    String file = "None";
    
    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t value);
    void load(uint16_t address, uint8_t value);
};

#endif