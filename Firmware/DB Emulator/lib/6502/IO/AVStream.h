#ifndef _AV_STREAM_H
#define _AV_STREAM_H

#include <Arduino.h>
#include "../pins.h"

// Sync byte — first byte of every 4-byte AV packet
#define AV_SYNC             0xAA

// Packet type constants
#define AV_TYPE_SOUND       0x01  // SID register write
#define AV_TYPE_VIDEO_DATA  0x02  // Video data port write (VRAM)
#define AV_TYPE_VIDEO_REG   0x03  // Video register write
#define AV_TYPE_VIDEO_ADDR  0x04  // Video address set [addr_hi, addr_lo]
#define AV_TYPE_RESET       0xFE  // Reset all AV state

// Send a 4-byte AV packet over SerialUSB2 and HWSERIAL
inline void avSend(uint8_t type, uint8_t a, uint8_t b) {
  const uint8_t packet[4] = { AV_SYNC, type, a, b };
  SerialUSB2.write(packet, 4);
  HWSERIAL.write(packet, 4);
}

#endif
