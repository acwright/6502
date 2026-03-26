#ifndef _VIDEO_CARD_H
#define _VIDEO_CARD_H

#include "IO.h"
#include "AVStream.h"

/*                    TMS9918A VIDEO DISPLAY PROCESSOR                */
/* ------------------------------------------------------------------ */
/* Two memory-mapped ports:                                           */
/*   $00 — Data port (VRAM read/write)                                */
/*   $01 — Control/Register port                                      */
/*                                                                    */
/* Control port two-byte write sequence:                              */
/*   First byte:  data / low address byte                             */
/*   Second byte: command + high address bits                         */
/*     bit 7 = 0: Set VRAM address for read/write                     */
/*     bit 7 = 1: Register write (bits 0-2 = register number)        */
/*                                                                    */
/* Registers 0-7:                                                     */
/*   R0: Mode control 1 (M3 in bit 1, External VDP in bit 0)         */
/*   R1: Mode control 2 (4/16K, blank, IE, M1, M2, sprite size/mag)  */
/*   R2: Name table base address                                      */
/*   R3: Color table base address                                     */
/*   R4: Pattern generator base address                               */
/*   R5: Sprite attribute table base address                          */
/*   R6: Sprite pattern generator base address                        */
/*   R7: Text/backdrop color                                          */
/*                                                                    */
/* Status register (read from control port):                          */
/*   bit 7: Frame interrupt flag (vblank)                             */
/*   bit 6: 5th sprite flag                                           */
/*   bit 5: Sprite coincidence flag                                   */
/*   bits 0-4: 5th sprite number                                     */
/* ------------------------------------------------------------------ */

class VideoCard: public IO {
  private:
    uint8_t  vram[16384];
    uint8_t  registers[8];
    uint8_t  status;
    uint16_t addr;
    uint8_t  latch;
    bool     firstByte;
    uint8_t  readBuf;

    // Vblank interrupt timing
    uint32_t cycleCount;
    uint32_t cyclesPerFrame;

  public:
    VideoCard();
    ~VideoCard() {};

    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick(uint32_t cpuFrequency) override;
    void    reset() override;
};

#endif
