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
#define CART_START      0xC000
#define CART_END        0xFFFF

#define PAGE_SIZE       1024
#define RAM_PAGES       32
#define IO_PAGES        8
#define ROM_PAGES       32
#define ROM_MAX         256
#define CART_MAX        256

#define FILE_CTX_ROM    0
#define FILE_CTX_CART   1
#endif