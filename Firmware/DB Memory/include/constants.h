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

#define IO_BANKS        (word[8]) { 0x8000, 0x8400, 0x8800, 0x8C00, 0x9000, 0x9400, 0x9800, 0x9C00 }
#define IO_BANK_SIZE    0x3FF

#define PAGE_SIZE       1024
#define RAM_PAGES       32
#define IO_PAGES        8
#define ROM_PAGES       32
#define ROM_MAX         256

#endif