#include "keyboard.h"
#include "macros.h"

namespace Keyboard {

  KeyboardController controller(usb);
  CircularBuffer<byte, 256> buffer;

  void begin() {
    controller.attachPress(onKeyPress);
  }

  void update() { /* Do nothing... */ }

  byte read(byte reg) {
    switch(reg) {
    case KBD_DATA: {
      if (buffer.size() > 0) {
        return buffer.shift();
      } else {
        return 0;
      }
    }
    default:
      return 0;
    }
  }

  void write(byte reg, byte data) { /* Do nothing... Keyboard registers are read-only. */ }

  void onKeyPress(int key) {
    if (key > 0) {
      Keyboard::buffer.push(key);
    }

    #ifdef KBD_DEBUG
    Serial.print("Key Pressed: ");
    Serial.print((char)key);

    char output[64];

    sprintf(
      output, 
      " | %c%c%c%c%c%c%c%c | 0x%02X",
      BYTE_TO_BINARY(key),
      key
    );

    Serial.println(output);
    #endif
  }

}