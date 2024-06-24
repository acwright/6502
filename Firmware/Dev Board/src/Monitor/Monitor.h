#ifndef _MONITOR_H
#define _MONITOR_H

#include <Arduino.h>

/*                                                    Monitor Registers                                                                                 */
/*                                                                                                                                                      */
/*                            | BIT 7 | BIT 6 | BIT 5 | BIT 4 | BIT 3 | BIT 2 | BIT 1 | BIT 0 |                                                         */
#define MON_DATA   0       /* |                         ASCII KEY CODE                        |  Monitor Data:     [ Extended Ascii Code (Bit 7-0) ]    */

/* Notes:                                                                                                                                               */
/* - Outputs character to Serial terminal.                                                                                                              */

#define MON_START  MON_DATA  + 0x00         // Defaults to $9C00
#define MON_END    MON_START + MON_DATA     // Defaults to $9C00

class Monitor {

public:

  Monitor();

  void begin();
  void update();
  byte read(byte reg);
  void write(byte reg, byte data);

};

#endif