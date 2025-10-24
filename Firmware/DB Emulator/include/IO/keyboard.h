#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <Arduino.h>
#include <USBHost_t36.h>
#include "../macros.h"
#include "../pins.h"

extern USBHost usb;

/*                                                   Keyboard Registers                                                                                 */
/*                                                                                                                                                      */
/*                            | BIT 7 | BIT 6 | BIT 5 | BIT 4 | BIT 3 | BIT 2 | BIT 1 | BIT 0 |                                                         */
#define KBD_DATA    0      /* |                        EXT ASCII CODE                         |  Keyboard Data: [ Extended Ascii Key Code (Bit 7-0) ]   */
#define KBD_ISR     1      /* |  INT  |                      UNUSED                           |  Keyboard Interrupt Status: [ Bit 7 ]                   */

/* Notes:                                                                                                                                               */
/* - Keyboard raises an interrupt on key press                                                                                                          */
/* - Keyboard interrupt can be cleared by reading the KBD_ISR register                                                                                  */

#define KBD_START  (KBD_DATA  + 0x01)            // Defaults to $8801
#define KBD_END    (KBD_ISR  +  0x01)            // Defaults to $8802

namespace Keyboard {

  void begin();
  void update();
  byte read(byte reg);
  void write(byte reg, byte data);
  void onKeyPress(int key);

}

#endif