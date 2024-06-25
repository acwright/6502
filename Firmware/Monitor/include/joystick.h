#ifndef _JOYSTICK_H
#define _JOYSTICK_H

#include <Arduino.h>
#include <USBHost_t36.h>

extern USBHost usb;

/*                                                   Joystick Registers                                                                                                                                                         */
/*                                                                                                                                                                                                                              */
/*                            | BIT 7 | BIT 6 | BIT 5 | BIT 4 | BIT 3 | BIT 2 | BIT 1 | BIT 0 |                                                                                                                                 */
#define JOY_BTNL    0      /* |   Y   |   X   |   B   |   A   | LFUNC | RFUNC |       NA      |  Joystick Btn L:           Y -> X -> B -> A -> L Function (View) -> R Function (Menu) -> (Bit 1-0 unused)                       */
#define JOY_BTNH    1      /* | RABTN | LABTN |  RBTN |  LBTN | RIGHT |  LEFT |  DOWN |   UP  |  Joystick Btn H:           Right Analog Btn -> Left Analog Btn -> Right Btn -> Left Btn -> D-Right -> D-Left -> D-Down -> D-Up  */
#define JOY_LAXL    2      /* |                   LEFT ANALOG X (LOW BYTE)                    |  Joystick Left Analog X:   -32768(Left) to +32768(Right) (Low Byte)                                                             */
#define JOY_LAXH    3      /* |                   LEFT ANALOG X (HIGH BYTE)                   |  Joystick Left Analog X:   -32768(Left) to +32768(Right) (High Byte)                                                            */
#define JOY_LAYL    4      /* |                   LEFT ANALOG Y (LOW BYTE)                    |  Joystick Left Analog Y:   -32768(Down) to +32768(Up) (Low Byte)                                                                */
#define JOY_LAYH    5      /* |                   LEFT ANALOG Y (HIGH BYTE)                   |  Joystick Left Analog Y:   -32768(Down) to +32768(Up) (High Byte)                                                               */
#define JOY_RAXL    6      /* |                  RIGHT ANALOG X (LOW BYTE)                    |  Joystick Right Analog X:  -32768(Left) to +32768(Right) (Low Byte)                                                             */
#define JOY_RAXH    7      /* |                  RIGHT ANALOG X (HIGH BYTE)                   |  Joystick Right Analog X:  -32768(Left) to +32768(Right) (High Byte)                                                            */
#define JOY_RAYL    8      /* |                  RIGHT ANALOG Y (LOW BYTE)                    |  Joystick Right Analog Y:  -32768(Down) to +32768(Up) (Low Byte)                                                                */
#define JOY_RAYH    9      /* |                  RIGHT ANALOG Y (HIGH BYTE)                   |  Joystick Right Analog Y:  -32768(Down) to +32768(Up) (High Byte)                                                               */
#define JOY_LTRL   10      /* |                   LEFT TRIGGER (LOW BYTE)                     |  Joystick Left Trigger:    +0 to +1024 (Low Byte)                                                                               */
#define JOY_LTRH   11      /* |                       NA                      |    LTRIG(H)   |  Joystick Left Trigger:    +0 to +1024 (High Byte) (Bit 7-2 unused)                                                             */
#define JOY_RTRL   12      /* |                  RIGHT TRIGGER (LOW BYTE)                     |  Joystick Right Trigger:   +0 to +1024 (Low Byte)                                                                               */
#define JOY_RTRH   13      /* |                       NA                      |    RTRIG(H)   |  Joystick Right Trigger:   +0 to +1024 (High Byte) (Bit 7-2 unused)                                                             */

/* Notes:                                                                                                                                                                                                                       */
/* - XBOX 360 and XBOX One controllers supported for now...                                                                                                                                                                     */

#define JOY_START  JOY_BTNL  + 0x16  // Defaults to $9C16
#define JOY_END    JOY_RTRH  + 0x16  // Defaults to $9C23

namespace Joystick {

  void begin();
  void update();
  byte read(byte reg);
  void write(byte reg, byte data);

};

#endif