#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <Arduino.h>
#include <USBHost_t36.h>
#include <CircularBuffer.hpp>

extern USBHost usb;

/*                                                   Keyboard Registers                                                                                 */
/*                                                                                                                                                      */
/*                            | BIT 7 | BIT 6 | BIT 5 | BIT 4 | BIT 3 | BIT 2 | BIT 1 | BIT 0 |                                                         */
#define KBD_DATA    0      /* |                        EXT ASCII CODE                         |  Keyboard Data: [ Extended Ascii Key Code (Bit 7-0) ]   */

/* Notes:                                                                                                                                               */
/* - Keyboard data is stored in a 256 byte circular buffer                                                                                              */
/* - Keyboard data register can be read repeatably until NULL (0x0) encountered at which time the buffer is empty                                       */

#define KBD_START  KBD_DATA  + 0x100            // Defaults to $9D00
#define KBD_END    KBD_DATA  + 0x100            // Defaults to $9D00

namespace Keyboard {

  void begin();
  void update();
  byte read(byte reg);
  void write(byte reg, byte data);
  void onKeyPress(int key);

}

#endif