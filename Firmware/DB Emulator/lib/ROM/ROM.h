#ifndef _ROM_H
#define _ROM_H

#include <Arduino.h>

class ROM {
  private:
    uint16_t size;
    uint8_t *data;
    
  public:
    ROM(uint16_t size);
    ~ROM();

    bool enabled = true;
    String file = "None";
    
    uint8_t read(uint16_t index);
    void write(uint16_t index, uint8_t value);
};

#endif