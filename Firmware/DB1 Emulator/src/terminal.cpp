#include "terminal.h"

namespace Terminal {

  void begin()         { /* Do nothing... */ }
  void update()        { /* Do nothing... */ }
  byte read(byte reg)  { /* Do nothing... Terminal is write only */ return 0; }

  void write(byte reg, byte data) {
    switch(reg) {
    case TERM_DATA:
      Serial.write(data);
      break;
    default:
      break;
    }
  }

}
  