#ifndef _CONSTANTS_H
#define _CONSTANTS_H

/*      MEMORY MAP      */
/* -------------------- */
/* |                  | */
/* |        ROM       | */
/* |    $A000-$FFFF   | */
/* |                  | */
/* -------------------- */
/* |    IO 7 ($9C00)  | */
/* -------------------- */
/* |    IO 6 ($9800)  | */
/* -------------------- */
/* |    IO 5 ($9400)  | */
/* -------------------- */
/* |    IO 4 ($9000)  | */
/* -------------------- */
/* |    IO 3 ($8C00)  | */
/* -------------------- */
/* |    IO 2 ($8800)  | */
/* -------------------- */
/* |    IO 1 ($8400)  | */
/* -------------------- */
/* |    IO 0 ($8000)  | */
/* -------------------- */
/* |                  | */
/* |        RAM       | */
/* |    $0000-$7FFF   | */
/* |                  | */
/* -------------------- */

#define RAM_START       0x0000
#define RAM_END         0x7FFF
#define RAM_CODE        0x0800
#define IO_START        0x8000
#define IO_END          0x9FFF
#define ROM_START       0x8000
#define ROM_END         0xFFFF
#define ROM_CODE        0xA000
#define CART_START      0x8000
#define CART_END        0xFFFF
#define CART_CODE       0xC000

#define IO_EMPTY        0
#define IO_SERIAL_CARD  1
#define IO_STORAGE_CARD 2
#define IO_RAM_CARD     3
#define IO_EMULATOR     4
#define IO_SLOTS        8
#define IO_SLOT_SIZE    0x400
#define IO_IRQ_FLAG     0b00000001
#define IO_NMI_FLAG     0b00000010

#define INPUT_CTX_ROM   0
#define INPUT_CTX_CART  1
#define INPUT_CTX_PROG  2
#define INPUT_CTX_IO    3

#define PAGE_SIZE       256

#define FREQ_SIZE       22
#define FREQ_LABELS     (String[FREQ_SIZE])  { "1 Hz", "2 Hz", "4 Hz", "8 Hz", "16 Hz", "32 Hz", "64 Hz", "122 Hz", "244 Hz", "488 Hz", "976 Hz", "1.9 kHz", "3.9 kHz", "7.8 kHz", "15.6 kHz", "31.2 kHz", "62.5 kHz", "125 kHz", "250 kHz", "500 kHz", "1 MHz", "2 MHz" }
#define FREQ_PERIODS    (double[FREQ_SIZE])  { 1000000, 500000, 250000, 125000, 62500, 31250, 15625, 7812, 3906, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1, 0.5 } // Full cycle period in microseconds

#define DEBOUNCE        5     // 5 milliseconds
#define FILE_PER_PAGE   8
#define FILE_MAX        256

#endif