#ifndef _EMULATOR_H
#define _EMULATOR_H

#include <Arduino.h>
#include "IO.h"
#include "constants.h"
#include "pins.h"

#ifdef DEVBOARD_0
/*                                REGISTERS                           */
/* ------------------------------------------------------------------ */
/* | # |            WRITE             |             READ            | */
/* ------------------------------------------------------------------ */
/* | 0 |    PRINT DATA (SERIAL)       |              N/A            | */
/* ------------------------------------------------------------------ */
/* | 1 |                       SCRATCH REGISTER                     | */
/* ------------------------------------------------------------------ */
/* | 2 |                      GPIO DATA REGISTER                    | */
/* ------------------------------------------------------------------ */
/* | 3 |                 GPIO DATA DIRECTION REGISTER               | */
/* ------------------------------------------------------------------ */
/*                                                                    */
/* GPIO DATA DIRECTION REGISTER                                       */
/* | 7 | 6 | 5 | 4 | 3  | 2  | 1  | 0  |                              */
/* | X | X | X | X | D3 | D2 | D1 | D0 |                              */
/* | 0 | 0 | 0 | 0 | 0  | 0  | 0  | 0  |  <- Default Values           */
/*                                                                    */
/* DO-7 - Data Direction (1 = OUTPUT, 0 = INPUT)                      */
/*                                                                    */

#define EMU_GPIO0 0b00000001
#define EMU_GPIO1 0b00000010
#define EMU_GPIO2 0b00000100
#define EMU_GPIO3 0b00001000
#endif

#ifdef DEVBOARD_1
#include <SPI.h>
/*                                REGISTERS                           */
/* ------------------------------------------------------------------ */
/* | # |            WRITE             |             READ            | */
/* ------------------------------------------------------------------ */
/* | 0 |    PRINT DATA (SERIAL)       |              N/A            | */
/* ------------------------------------------------------------------ */
/* | 1 |                     SPI TRANSFER REGISTER                  | */
/* ------------------------------------------------------------------ */
/* | 2 |    SPI CONTROL REGISTER      |      SPI STATUS REGISTER    | */
/* ------------------------------------------------------------------ */
/* | 3 |                       SPI CLOCK REGISTER                   | */  
/* ------------------------------------------------------------------ */
/*                                                                    */
/* SPI CONTROL REGISTER                                               */
/* | 7   | 6 | 5  | 4  | 3  | 2   | 1   | 0   |                       */
/* | IE  | X | DO | M1 | M0 | CS2 | CS1 | CS0 |                       */
/* | 0   | 0 | 1  | 0  | 0  |  0  |  0  |  0  |  <- Default Values    */
/*                                                                    */
/* IE    - Interrupt Enable (1 = ENABLED, 0 = DISABLED)               */
/* DO    - Data Order (1 = MSBFIRST, 0 = LSBFIRST)                    */
/* M0-1  - SPI MODE (0-3)                                             */
/* CS0-2 - Chip Selected for next transfer                            */
/*                                                                    */
/* SPI STATUS REGISTER                                                */
/* | 7   | 6   | 5  | 4  | 3  | 2   | 1   | 0   |                     */
/* | INT | BSY | DO | M1 | M0 | CS2 | CS1 | CS0 |                     */
/* | 0   | 0   | 1  | 0  | 0  |  0  |  0  |  0  |  <- Default Values  */
/*                                                                    */
/* INT   - Interrupt Flag (1 = INT OCCURRED, 0 = NONE)                */
/* BSY   - Busy Flag (1 = TRANSFERRING, 0 = DONE)                     */
/* DO    - Data Order (1 = LSBFIRST, 0 = MSBFIRST)                    */
/* M0-1  - SPI MODE (0-3)                                             */
/* CS0-2 - Chip Selected for next transfer                            */
/*                                                                    */
/* SPI CLOCK REGISTER                                                 */
/* | 7   | 6  | 5  | 4  | 3 | 2 | 1 | 0 |                             */
/* | 128 | 64 | 32 | 16 | 8 | 4 | 2 | 1 |                             */
/* | 0   | 0  | 0  | 0  | 0 | 1 | 0 | 0 |  <- Default Values          */
/* Speed in MHz (Default 4MHz)                                        */
/*                                                                    */
/* Ref: https://www.pjrc.com/teensy/td_libs_SPI.html                  */
/*                                                                    */
#define EMU_SPI_CTRL_CS0    0b00000001
#define EMU_SPI_CTRL_CS1    0b00000010
#define EMU_SPI_CTRL_CS2    0b00000100
#define EMU_SPI_CTRL_M0     0b00001000
#define EMU_SPI_CTRL_M1     0b00010000
#define EMU_SPI_CTRL_DO     0b00100000
#define EMU_SPI_CTRL_X      0b01000000
#define EMU_SPI_CTRL_IE     0b10000000

#define EMU_SPI_STATUS_CS0  0b00000001
#define EMU_SPI_STATUS_CS1  0b00000010
#define EMU_SPI_STATUS_CS2  0b00000100
#define EMU_SPI_STATUS_M0   0b00001000
#define EMU_SPI_STATUS_M1   0b00010000
#define EMU_SPI_STATUS_DO   0b00100000
#define EMU_SPI_STATUS_BSY  0b01000000
#define EMU_SPI_STATUS_INT  0b10000000
#endif

class Emulator: public IO {
  private:
    uint8_t data;

    #ifdef DEVBOARD_0
    uint8_t scratch;
    uint8_t gpioData;
    uint8_t gpioDataDir;
    #endif

    #ifdef DEVBOARD_1
    uint8_t spiTransfer;
    uint8_t spiControl;
    uint8_t spiStatus;
    uint8_t spiClock;

    bool spiTransferPending = false;
    #endif

  public:
    Emulator();
    ~Emulator() {}

    uint8_t id() { return IO_EMULATOR; }
    String  description() override { return "Emulator"; }
    bool    passthrough() override { return false; }
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick() override;
    void    reset() override;
};

#endif