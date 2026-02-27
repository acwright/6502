#include "GPIOJoystickAttachment.h"

GPIOJoystickAttachment::GPIOJoystickAttachment(bool attachToPortA, uint8_t priority)
  : GPIOAttachment(priority, false, false, false, false),  // No control lines
    buttonState(0x00), attachedToPortA(attachToPortA) {
  reset();
}

void GPIOJoystickAttachment::reset() {
  GPIOAttachment::reset();
  buttonState = 0x00;
}

uint8_t GPIOJoystickAttachment::readPortA(uint8_t ddrA, uint8_t orA) {
  if (attachedToPortA) {
    // Return inverted button state (active-low)
    return ~buttonState;
  }
  return 0xFF;
}

uint8_t GPIOJoystickAttachment::readPortB(uint8_t ddrB, uint8_t orB) {
  if (!attachedToPortA) {
    // Return inverted button state (active-low)
    return ~buttonState;
  }
  return 0xFF;
}

void GPIOJoystickAttachment::updateJoystick(uint8_t buttons) {
  buttonState = buttons;
}

uint8_t GPIOJoystickAttachment::getButtonState() const {
  return buttonState;
}

bool GPIOJoystickAttachment::isButtonPressed(uint8_t button) const {
  return (buttonState & button) != 0;
}
