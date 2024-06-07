#ifndef _RAM_H
#define _RAM_H

#include <Arduino.h>
#include "memory.h"

class Ram {
  private:
    byte _RAM[RAM_END - RAM_START + 1];
    
  public:
    void init();
};

extern Ram RAM;

#endif