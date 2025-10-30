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
#define IO_START        0x8000
#define IO_END          0x9FFF
#define ROM_START       0x8000
#define ROM_CODE        0xA000
#define ROM_END         0xFFFF
#define CART_START      0x8000
#define CART_CODE       0xC000
#define CART_END        0xFFFF

#define FREQS           (String[21])  { "1 Hz", "2 Hz", "4 Hz", "8 Hz", "16 Hz", "32 Hz", "64 Hz", "122 Hz", "244 Hz", "488 Hz", "976 Hz", "1.9 kHz", "3.9 kHz", "7.8 kHz", "15.6 kHz", "31.2 kHz", "62.5 kHz", "125 kHz", "250 kHz", "500 kHz", "1 MHz" }
#define FREQ_PERIODS    (double[21])  { 1000000, 500000, 250000, 125000, 62500, 31250, 15625, 7812, 3906, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1 } // Full cycle period in microseconds

#define DEBOUNCE        5     // 5 milliseconds

#define IO_VIDEO_CARD   0
#define IO_SOUND_CARD   1
#define IO_GPIO_CARD_KB 2
#define IO_GPIO_CARD_GH 3
#define IO_GPIO_CARD_KH 4
#define IO_INPUT_BOARD  5
#define IO_SERIAL_CARD  6
#define IO_STORAGE_CARD 7
#define IO_RTC_CARD     8
#define IO_EMULATOR     9
#define IO_LCD_CARD     10
#define IO_RAM_CARD     11
#define IO_NYI_1        12
#define IO_NYI_2        13
#define IO_NYI_3        14
#define IO_EMPTY        15
#define IO_BANKS        (uint16_t[8]) { 0x8000, 0x8400, 0x8800, 0x8C00, 0x9000, 0x9400, 0x9800, 0x9C00 }
#define IO_BANK_SIZE    0x3FF
#define IO_IRQ_FLAG     0b00000001
#define IO_NMI_FLAG     0b00000010

#define ROM_MAX         256
#define CART_MAX        256

#define INPUT_CTX_ROM    0
#define INPUT_CTX_CART   1
#define INPUT_CTX_IO     2

#endif