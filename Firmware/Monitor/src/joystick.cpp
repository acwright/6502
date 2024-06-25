#include "Joystick.h"

namespace Joystick {

  JoystickController controller(usb);

  byte joystickBtnL     = 0;
  byte joystickBtnH     = 0;
  byte joystickLAnXL    = 0;
  byte joystickLAnXH    = 0;
  byte joystickLAnYL    = 0;
  byte joystickLAnYH    = 0;
  byte joystickRAnXL    = 0;
  byte joystickRAnXH    = 0;
  byte joystickRAnYL    = 0;
  byte joystickRAnYH    = 0;
  byte joystickLTrL     = 0;
  byte joystickLTrH     = 0;
  byte joystickRTrL     = 0;
  byte joystickRTrH     = 0;

  void begin() { /* Do nothing... */ }

  void update() { 
    if (controller.available()) {
      uint32_t buttons    = controller.getButtons();

      if (controller.joystickType() == JoystickController::XBOX360 || 
          controller.joystickType() == JoystickController::XBOXONE) 
      {
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

        joystickBtnL = buttons & 0xFF;
        joystickBtnH = (buttons >> 8) & 0xFF;
        joystickLAnXL = controller.getAxis(0) & 0xFF;
        joystickLAnXH = (controller.getAxis(0) >> 8) & 0xFF;
        joystickLAnYL = controller.getAxis(1) & 0xFF;
        joystickLAnYH = (controller.getAxis(1) >> 8) & 0xFF;
        joystickRAnXL = controller.getAxis(2) & 0xFF;
        joystickRAnXH = (controller.getAxis(2) >> 8) & 0xFF;
        joystickRAnYL = controller.getAxis(5) & 0xFF;
        joystickRAnYH = (controller.getAxis(5) >> 8) & 0xFF;
        joystickLTrL = controller.getAxis(3) & 0xFF;
        joystickLTrH = (controller.getAxis(3) >> 8) & 0xFF;
        joystickRTrL = controller.getAxis(4) & 0xFF;
        joystickRTrH = (controller.getAxis(4) >> 8) & 0xFF;
      }

      controller.joystickDataClear();
    
      #ifdef JOY_DEBUG
      Serial.printf("Joystick: Buttons = %x", buttons);

      uint64_t axis_mask  = controller.axisMask();
      
      for (uint8_t i = 0; axis_mask != 0; i++, axis_mask >>= 1) {
        if (axis_mask & 1) {
          Serial.printf(" %d:%d", i, controller.getAxis(i));
        }
      }
      Serial.println();
      #endif
    }
  }

  byte read(byte reg) { 
    switch(reg) {
    case JOY_BTNL:
      return joystickBtnL;
    case JOY_BTNH:
      return joystickBtnH;
    case JOY_LAXL:
      return joystickLAnXL;
    case JOY_LAXH:
      return joystickLAnXH;
    case JOY_LAYL:
      return joystickLAnYL;
    case JOY_LAYH:
      return joystickLAnYH;
    case JOY_RAXL:
      return joystickRAnXL;
    case JOY_RAXH:
      return joystickRAnXH;
    case JOY_RAYL:
      return joystickRAnYL;
    case JOY_RAYH:
      return joystickRAnYH;
    case JOY_LTRL:
      return joystickLTrL;
    case JOY_LTRH:
      return joystickLTrH;
    case JOY_RTRL:
      return joystickRTrL;
    case JOY_RTRH:
      return joystickRTrH;
    default:
      return 0;
    }
  }

  void write(byte reg, byte data) { /* Do nothing... Joystick registers are read-only. */ }

}