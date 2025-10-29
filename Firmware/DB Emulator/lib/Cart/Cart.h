#ifndef _CART_H
#define _CART_H

#include <Arduino.h>

class Cart {
  private:
    uint16_t size;
    uint8_t *data;
    
  public:
    Cart(uint16_t size);
    ~Cart();
    
    bool enabled = false;
    String file = "None";

    uint8_t read(uint16_t index);
    void write(uint16_t index, uint8_t value);
};

#endif