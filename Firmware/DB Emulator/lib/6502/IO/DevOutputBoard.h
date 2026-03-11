#ifndef _DEV_OUTPUT_BOARD_H
#define _DEV_OUTPUT_BOARD_H

#include "IO.h"

/*                                REGISTERS                           */
/* ------------------------------------------------------------------ */
/* | # |            WRITE             |             READ            | */
/* ------------------------------------------------------------------ */
/* | 0 |    TRANSMIT DATA REGISTER    |    STATUS REGISTER (bit 7)  | */
/* ------------------------------------------------------------------ */
/*                                                                    */
/* STATUS REGISTER                                                    */
/* | 7   | 6 | 5 | 4 | 3 | 2 | 1 | 0 |                                */
/* | BSY | X | X | X | X | X | X | X |                                */
/* | 0   | 0 | 0 | 0 | 0 | 0 | 0 | 0 |  <- Default Value              */
/*                                                                    */

#define DOB_STATUS_BUSY               0b10000000

class DevOutputBoard: public IO {
  public:
    DevOutputBoard();
    ~DevOutputBoard() {};
    
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick(uint32_t cpuFrequency) override;
    void    reset() override;
};

#endif
