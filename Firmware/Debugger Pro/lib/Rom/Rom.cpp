#include "Rom.h"

Rom ROM;

void Rom::init() {
  unsigned int i;
  
  // Fill ROM with NOPs by default
  for (i = 0; i < (ROM_END - ROM_START + 1); i++) {
    _ROM[i] = 0xEA; 
  }
} 