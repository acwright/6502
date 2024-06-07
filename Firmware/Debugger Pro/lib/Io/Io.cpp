#include "Io.h"

Io IO;

void Io::init() {
  unsigned int i;
  
  for (i = 0; i < (IO_END - IO_START + 1); i++) {
    _IO[i] = 0x00; 
  }
} 