#ifndef _MOUSE_H
#define _MOUSE_H

#include <Arduino.h>
#include <USBHost_t36.h>

extern USBHost usb;

/*                                                      Mouse Registers                                                                                                               */
/*                                                                                                                                                                                    */
/*                            | BIT 7 | BIT 6 | BIT 5 | BIT 4 | BIT 3 | BIT 2 | BIT 1 | BIT 0 |                                                                                       */
#define MOUSE_X     0      /* |  DIR  |                    VELOCITY                           |  Mouse X:          Direction + Right (0) - Left (1) -> Velocity from 0-127            */
#define MOUSE_Y     1      /* |  DIR  |                    VELOCITY                           |  Mouse Y:          Direction + Down (0) - Up (1) -> Velocity from 0-127               */
#define MOUSE_W     2      /* |  DIR  |                    VELOCITY                           |  Mouse Wheel:      Direction + Down (0) - Up (1) -> Velocity from 0-127               */
#define MOUSE_BTN   3      /* |                  NA                   |  MID  | RIGHT |  LEFT |  Mouse Buttons:    (Bit 7-3 unused) -> Middle, Right, Left Buttons Pressed (Bit 2-0)  */

#define MOUSE_START  (MOUSE_X    + 0x03)  // Defaults to $8803
#define MOUSE_END    (MOUSE_BTN  + 0x03)  // Defaults to $8806

namespace Mouse {

  void begin();
  void update();
  byte read(byte reg);
  void write(byte reg, byte data);

};

#endif