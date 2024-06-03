#ifndef _UTILITIES_H
#define _UTILITIES_H

#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')

#define FREQ_TO_PERIOD(freq)      (unsigned long) ((1.0 / freq) * 1000000) // In microseconds
#define PERIOD_TO_FREQ(period)    (double) (1.0 / (period / 1000000)) // In Hz

#endif