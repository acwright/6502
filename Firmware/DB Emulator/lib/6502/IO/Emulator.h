#ifndef _EMULATOR_H
#define _EMULATOR_H

#include <Arduino.h>
#include <TimeLib.h>
#include <SD.h>
#include "IO.h"
#include "constants.h"
#include "pins.h"

#ifdef DEVBOARD_0
#include <Wire.h>
#endif

#ifdef DEVBOARD_1
#include <SPI.h>
#endif

#ifdef DEVBOARD_1_1
#include <Wire.h>
#include <SPI.h>
#endif

/* ------------------------------------------------------------------ */
/*                                REGISTERS                           */
/* ------------------------------------------------------------------ */
/* | # |            WRITE             |             READ            | */
/* ------------------------------------------------------------------ */
/* | 0 |                     SERIAL DATA REGISTER                   | */
/* ------------------------------------------------------------------ */
/* | 1 |   SERIAL CONTROL REGISTER    |   SERIAL STATUS REGISTER    | */
/* ------------------------------------------------------------------ */
/* | 2 |           KB CMD              |           KB DATA          | */
/* ------------------------------------------------------------------ */
/* | 3 |             N/A               |           MOUSE X          | */
/* ------------------------------------------------------------------ */
/* | 4 |             N/A               |           MOUSE Y          | */
/* ------------------------------------------------------------------ */
/* | 5 |             N/A               |           MOUSE W          | */
/* ------------------------------------------------------------------ */
/* | 6 |             N/A               |          MOUSE BTNS        | */
/* ------------------------------------------------------------------ */
/* | 7 |             N/A               |           JOY BTNS         | */
/* ------------------------------------------------------------------ */
/* | 8 |             N/A               |        RTC SEC (0-59)      | */
/* ------------------------------------------------------------------ */
/* | 9 |             N/A               |        RTC MIN (0-59)      | */
/* ------------------------------------------------------------------ */
/* | A |             N/A               |         RTC HR (0-23)      | */
/* ------------------------------------------------------------------ */
/* | B |             N/A               |        RTC DAY (0-31)      | */
/* ------------------------------------------------------------------ */
/* | C |            N/A                |        RTC MON (1-12)      | */
/* ------------------------------------------------------------------ */
/* | D |            N/A                |         RTC YR (0-99)      | */
/* ------------------------------------------------------------------ */
/* | E |                          PRAM DATA                         | */
/* ------------------------------------------------------------------ */
/* | F |                         PRAM ADDRESS                       | */
/* ------------------------------------------------------------------ */
/*                                                                    */
/* SERIAL CONTROL REGISTER                                            */
/* | 7   | 6  | 5  | 4  | 3  | 2  | 1  | 0  |                         */
/* | IE  | CE | TO | T0 | M3 | M2 | M1 | M0 |                         */
/* | 0   | 0  | 0  | 0  | 0  |  0 |  0 |  0 |  <- Default Values      */
/*                                                                    */
/* IE    - Interrupt Enable (1 = ENABLED, 0 = DISABLED)               */
/* CE    - Chip Enable Pin (1 = LOW, 0 = HIGH) (Dev Board 1.1 ONLY)   */
/* T0-1  - Target Select                                              */
/* M0-3  - Mode Select                                                */
/*                                                                    */
/* TARGET:                                                            */
/* | T1 | T0 |     TARGET     |                                       */
/* | 0  | 0  |     SERIAL     |  USB Serial                           */
/* | 0  | 1  |      SPI       |  Dev Board 1.0 and 1.1 ONLY           */
/* | 1  | 0  |      I2C       |  Dev Board 0.0 and 1.1 ONLY           */
/* | 1  | 1  |     TX/RX      |  Hardware Serial                      */
/*                                                                    */
/* SERIAL STATUS REGISTER                                             */
/* | 7  | 6 | 5 | 4 | 3 | 2 | 1 | 0 |                                 */
/* | DA | X | X | X | X | X | X | X |                                 */
/* | 0  | 0 | 0 | 0 | 0 | 0 | 0 | 0 |  <- Default Values              */
/*                                                                    */
/* DA    - Data Available / Interrupt (1 = AVAILABLE/INT, 0 = NONE)   */
/*         Note: Reading from serial data register clears DA/INT      */
/*                                                                    */
/*                                                                    */
/* KB CMD REGISTER                                                    */
/* | 7  | 6 | 5 | 4 | 3 | 2 | 1 | 0 |                                 */
/* | IE | X | X | X | X | X | X | X |                                 */
/* | 0  | 0 | 0 | 0 | 0 | 0 | 0 | 0 |  <- Default Values              */
/*                                                                    */
/* IE   - Keyboard Interrupt Enable (1 = ENABLED, 0 = DISABLED)       */
/*                                                                    */
/* KB DATA REGISTER                                                   */
/* | 7  | 6 | 5 | 4 | 3  | 2  | 1  | 0  |                             */
/* | KA |         ASCII DATA            |                             */
/* | 0  | 0 | 0 | 0 | 0  | 0  | 0  | 0  |  <- Default Values          */
/*                                                                    */
/* KA   - Key Available (1 = AVAILABLE, 0 = NONE)                     */
/* DO-6 - ASCII Data                                                  */
/*                                                                    */
/* JOY BTNS REGISTERS                                                 */
/* | 7     | 6     | 5    | 4    | 3    | 2    | 1 | 0 |              */
/* | Y     | X     | B    | A    | R    | L    | D | U |              */
/* | 0     | 0     | 0    | 0    | 0    | 0    | 0 | 0 |  <- Default  */
/*                                                                    */
#define EMU_SER_CTRL_M0     0b00000001
#define EMU_SER_CTRL_M1     0b00000010
#define EMU_SER_CTRL_M2     0b00000100
#define EMU_SER_CTRL_M3     0b00001000
#define EMU_SER_CTRL_T0     0b00010000
#define EMU_SER_CTRL_T1     0b00100000
#define EMU_SER_CTRL_CE     0b01000000
#define EMU_SER_CTRL_IE     0b10000000

#define EMU_SER_STATUS_DA   0b10000000

#define EMU_KEY_INT         0b10000000
#define EMU_KEY_AVAILABLE   0b10000000

#define EMU_PRAM_FILE_NAME  "PRAM.bin"
#define EMU_PRAM_SIZE       0x100 // 256 bytes

#define EMU_TARGET_SERIAL   0
#define EMU_TARGET_SPI      1
#define EMU_TARGET_I2C      2
#define EMU_TARGET_TXRX     3

class Emulator: public IO {
  private:
    uint8_t serialData;
    uint8_t serialControl;
    uint8_t serialStatus;
    uint8_t keyboardCmd;
    uint8_t keyboardData;
    uint8_t mouseXData;
    uint8_t mouseYData;
    uint8_t mouseWData;
    uint8_t mouseBtnsData;
    uint8_t joystickData;

    uint8_t *pramData;
    uint8_t pramAddress;

    uint8_t target;

    void configureSerial(uint8_t value);
    void transmitSerial(uint8_t value);

    void modeToSPIChipSelect(uint8_t mode);
    uint32_t modeToSPISpeed(uint8_t mode);
    uint32_t modeToI2CSpeed(uint8_t mode);
    uint32_t modeToBaudRate(uint8_t mode);

  public:
    Emulator();
    ~Emulator();

    uint8_t id() { return IO_EMULATOR; }
    String  description() override { return "Emulator"; }
    bool    passthrough() override { return false; }
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick() override;
    void    reset() override;

    void updateKeyboard(uint8_t ascii);
    void updateMouse(int mouseX, int mouseY, int mouseWheel, uint8_t mouseButtons);
    void updateJoystick(uint32_t buttons);
};

#endif