#ifndef _ROM_H
#define _ROM_H

#include <Arduino.h>
#include "memory.h"

class Rom {
  private:
    byte _ROM[ROM_END - ROM_START + 1];
    
  public:
    void init();
};

extern Rom ROM;

#endif