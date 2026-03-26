#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#include <stdint.h>

#define VERSION         "2.0"

// AV protocol
#define AV_SYNC             0xAA
#define AV_TYPE_SOUND       0x01
#define AV_TYPE_VIDEO_DATA  0x02
#define AV_TYPE_VIDEO_REG   0x03
#define AV_TYPE_VIDEO_ADDR  0x04
#define AV_TYPE_RESET       0xFE

// Display geometry
#define ACTIVE_W  256
#define ACTIVE_H  192
#define BORDER_X   32
#define BORDER_Y   24
#define CANVAS_W  320
#define CANVAS_H  240

// TMS9918A 16-color RGB565 palette
static const uint16_t TMS_PALETTE[16] = {
  0x0000,  //  0  Transparent / Black
  0x0000,  //  1  Black
  0x2648,  //  2  Medium Green
  0x5EEF,  //  3  Light Green
  0x52BD,  //  4  Dark Blue
  0x7BBF,  //  5  Light Blue
  0xD289,  //  6  Dark Red
  0x475E,  //  7  Cyan
  0xFAAA,  //  8  Medium Red
  0xFBCF,  //  9  Light Red
  0xD60A,  // 10  Dark Yellow
  0xE670,  // 11  Light Yellow
  0x2587,  // 12  Dark Green
  0xCAD7,  // 13  Magenta
  0xCE79,  // 14  Gray
  0xFFFF,  // 15  White
};

#endif