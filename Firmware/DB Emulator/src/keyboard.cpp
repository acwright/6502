#include "keyboard.h"
#include "macros.h"
#include "devboard.h"

namespace Keyboard {

  KeyboardController controller(usb);

  byte keyboardData       = 0;
  byte keyboardIntStatus  = 0;

  void begin() {
    controller.attachPress(onKeyPress);
  }

  void update() { /* Do nothing... */ }

  byte read(byte reg) {
    switch(reg) {
    case KBD_DATA: 
      keyboardIntStatus &= 0b01111111;  // Clear INT bit
      return keyboardData;
    case KBD_ISR:
      digitalWriteFast(IRQB, LOW);
      pinMode(IRQB, INPUT);
      return keyboardIntStatus;
    default:
      return 0;
    }
  }

  void write(byte reg, byte data) { /* Do nothing... Keyboard registers are read-only. */ }

  void onKeyPress(int key) {
    if (key >= 0) {
      keyboardData = key;
      keyboardIntStatus |= 0b10000000;  // Set INT bit
      pinMode(IRQB, OUTPUT);
      digitalWriteFast(IRQB, HIGH);
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