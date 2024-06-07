#ifndef _IO_H
#define _IO_H

#include <Arduino.h>
#include "memory.h"

class Io {
  private:
    byte _IO[IO_END - IO_START + 1];
    
  public:
    void init();
};

extern Io IO;

#endif