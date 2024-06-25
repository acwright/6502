#include "Mouse.h"

namespace Mouse {

  MouseController controller(usb);

  byte mouseX     = 0;
  byte mouseY     = 0;
  byte mouseW     = 0;
  byte mouseBtns  = 0;

  void begin() { /* Do nothing... */ }

  void update() {
    if (controller.available()) {
      byte _mouseX = controller.getMouseX();
      byte _mouseY = controller.getMouseY();
      byte _mouseWheel = controller.getWheel();

      mouseX    = _mouseX < 0 ? (abs(_mouseX) | 0b10000000) : _mouseX;                  // If negative, set bit 7 (Left = -X)
      mouseY    = _mouseY < 0 ? (abs(_mouseY) | 0b10000000) : _mouseY;                  // If negative, set bit 7 (Up = -Y)
      mouseW    = _mouseWheel < 0 ? (abs(_mouseWheel) | 0b10000000) : _mouseWheel;      // If negative, set bit 7 (Up = -W)
      mouseBtns = controller.getButtons();
    
      controller.mouseDataClear();
    }

    #ifdef MOUSE_DEBUG
      // Serial.print("Mouse: Buttons = ");
      // Serial.print(mouseBtns);
      // Serial.print(",  X = ");
      // Serial.print(mouseX);
      // Serial.print(",  Y = ");
      // Serial.print(mouseY);
      // Serial.print(",  Wheel = ");
      // Serial.print(mouseW);
      // Serial.println();
    #endif
  }

  byte read(byte reg) {
    switch(reg) {
    case MOUSE_X:
      return mouseX;
    case MOUSE_Y:
      return mouseY;
    case MOUSE_W:
      return mouseW;
    case MOUSE_BTN:
      return mouseBtns;
    default:
      return 0;
    }
  }

  void write(byte reg, byte data) { /* Do nothing... Mouse registers are read-only. */ }

}