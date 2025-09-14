#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <Arduino.h>

/*                                                    Terminal Registers                                                                                */
/*                                                                                                                                                      */
/*                            | BIT 7 | BIT 6 | BIT 5 | BIT 4 | BIT 3 | BIT 2 | BIT 1 | BIT 0 |                                                         */
#define TERM_DATA   0      /* |                      EXT ASCII KEY CODE                       |  Terminal Data:     [ Extended Ascii Code (Bit 7-0) ]   */

/* Notes:                                                                                                                                               */
/* - Outputs character to the Teensy serial terminal.                                                                                                              */

#define TERM_START  (TERM_DATA  + 0x00)           // Defaults to $8800
#define TERM_END    (TERM_START + TERM_DATA)      // Defaults to $8800

namespace Terminal {

  void begin();
  void update();
  byte read(byte reg);
  void write(byte reg, byte data);

};

#endif