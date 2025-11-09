#include "GPIO.h"

GPIO::GPIO(uint8_t attachment) {
  this->attachment = attachment;
}

uint8_t GPIO::id() {
  switch(this->attachment) {
    case GPIO_ATTACHMENT_GPIO_HELPER:
      return IO_GPIO_CARD_GH;
    case GPIO_ATTACHMENT_INPUT_BOARD:
      return IO_INPUT_BOARD;
    case GPIO_ATTACHMENT_KEYBOARD_HELPER:
      return IO_GPIO_CARD_KB;
    case GPIO_ATTACHMENT_KEYPAD_HELPER:
      return IO_GPIO_CARD_KH;
    case GPIO_ATTACHMENT_LCD_CARD:
      return IO_LCD_CARD;
    default:
      return 0xFF; // Shouldn't happen
  }
}

String GPIO::description() {
  switch(this->attachment) {
    case GPIO_ATTACHMENT_GPIO_HELPER:
      return "GPIO Card (GPIO Helper)";
    case GPIO_ATTACHMENT_INPUT_BOARD:
      return "Input Board";
    case GPIO_ATTACHMENT_KEYBOARD_HELPER:
      return "GPIO Card (Keyboard / PS2 Helper)";
    case GPIO_ATTACHMENT_KEYPAD_HELPER:
      return "GPIO Card (Keypad Helper)";
    case GPIO_ATTACHMENT_LCD_CARD:
      return "LCD Card";
    default:
      return "GPIO"; // Shouldn't happen
  }
}

uint8_t GPIO::read(uint16_t address) { return 0x00; }
void GPIO::write(uint16_t address, uint8_t value) {}
void GPIO::reset() {}

void GPIO::updateJoystick(uint32_t buttons) {
  // XBOX Button Values
  // LFUNC (View): 0x8 1000
  // RFUNC (Menu): 0x4 0100
  // A: 0x10 0001 0000
  // B: 0x20 0010 0000
  // X: 0x40 0100 0000
  // Y: 0x80 1000 0000
  // UP: 0x100 0001 0000 0000
  // DOWN: 0x200 0010 0000 0000
  // LEFT: 0x400 0100 0000 0000
  // RIGHT: 0x800 1000 0000 0000
  // LBTN: 0x1000 0001 0000 0000 0000
  // RBTN: 0x2000 0010 0000 0000 0000
  // LABTN: 0x4000 0100 0000 0000 0000
  // RABTN: 0x8000 1000 0000 0000 0000

  // XBOX Analog Values
  // Left Analog X: 0:-32768(left) +32768(right) 
  // Left Analog Y: 1:-32768(down) +32768(up)
  // Right Analog X: 2:-32768(left) +32768(right) 
  // LTRIG: 3:0-1023
  // RTRIG: 4:0-1023
  // Right Analog Y: 5:-32768(down) +32768(up)

  // uint8_t joystickBtnL = buttons & 0xFF;
  // uint8_t joystickBtnH = (buttons >> 8) & 0xFF;
  // uint8_t joystickBtn  = (joystickBtnL & 0xF0) | (joystickBtnH & 0x0F);
  // uint8_t joystickLAnXL = joystick.getAxis(0) & 0xFF;
  // uint8_t joystickLAnXH = (joystick.getAxis(0) >> 8) & 0xFF;
  // uint8_t joystickLAnYL = joystick.getAxis(1) & 0xFF;
  // uint8_t joystickLAnYH = (joystick.getAxis(1) >> 8) & 0xFF;
  // uint8_t joystickRAnXL = joystick.getAxis(2) & 0xFF;
  // uint8_t joystickRAnXH = (joystick.getAxis(2) >> 8) & 0xFF;
  // uint8_t joystickRAnYL = joystick.getAxis(5) & 0xFF;
  // uint8_t joystickRAnYH = (joystick.getAxis(5) >> 8) & 0xFF;
  // uint8_t joystickLTrL = joystick.getAxis(3) & 0xFF;
  // uint8_t joystickLTrH = (joystick.getAxis(3) >> 8) & 0xFF;
  // uint8_t joystickRTrL = joystick.getAxis(4) & 0xFF;
  // uint8_t joystickRTrH = (joystick.getAxis(4) >> 8) & 0xFF;
}