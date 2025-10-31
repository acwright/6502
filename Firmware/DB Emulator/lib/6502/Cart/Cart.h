#ifndef _CART_H
#define _CART_H

#include <Arduino.h>
#include "constants.h"

class Cart {
  private:
    uint8_t *data;
    
  public:
    Cart();
    ~Cart();
    
    bool enabled = false;
    String file = "None";

    uint8_t read(uint16_t index);
    void write(uint16_t index, uint8_t value);
};

#endif