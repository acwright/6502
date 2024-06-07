#include "Ram.h"

Ram RAM;

void Ram::init() {
  unsigned int i;
  
  for (i = 0; i < (RAM_END - RAM_START + 1); i++) {
    _RAM[i] = 0x00; 
  }
} 