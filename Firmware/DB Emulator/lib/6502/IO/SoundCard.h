#ifndef _SOUND_CARD_H
#define _SOUND_CARD_H

#include "IO.h"
#include "AVStream.h"

/*                       MOS 6581/8580 SID REGISTERS                  */
/* ------------------------------------------------------------------ */
/* | ADDR |              WRITE              |          READ          | */
/* ------------------------------------------------------------------ */
/* | $00  | Voice 1 Frequency Lo            |                        | */
/* | $01  | Voice 1 Frequency Hi            |                        | */
/* | $02  | Voice 1 Pulse Width Lo          |                        | */
/* | $03  | Voice 1 Pulse Width Hi          |                        | */
/* | $04  | Voice 1 Control Register        |                        | */
/* | $05  | Voice 1 Attack / Decay          |                        | */
/* | $06  | Voice 1 Sustain / Release       |                        | */
/* | $07  | Voice 2 Frequency Lo            |                        | */
/* | $08  | Voice 2 Frequency Hi            |                        | */
/* | $09  | Voice 2 Pulse Width Lo          |                        | */
/* | $0A  | Voice 2 Pulse Width Hi          |                        | */
/* | $0B  | Voice 2 Control Register        |                        | */
/* | $0C  | Voice 2 Attack / Decay          |                        | */
/* | $0D  | Voice 2 Sustain / Release       |                        | */
/* | $0E  | Voice 3 Frequency Lo            |                        | */
/* | $0F  | Voice 3 Frequency Hi            |                        | */
/* | $10  | Voice 3 Pulse Width Lo          |                        | */
/* | $11  | Voice 3 Pulse Width Hi          |                        | */
/* | $12  | Voice 3 Control Register        |                        | */
/* | $13  | Voice 3 Attack / Decay          |                        | */
/* | $14  | Voice 3 Sustain / Release       |                        | */
/* | $15  | Filter Cutoff Lo (bits 0-2)     |                        | */
/* | $16  | Filter Cutoff Hi                |                        | */
/* | $17  | Filter Resonance / Routing      |                        | */
/* | $18  | Filter Mode / Volume            |                        | */
/* | $19  | Paddle X                        | Paddle X               | */
/* | $1A  | Paddle Y                        | Paddle Y               | */
/* | $1B  | Voice 3 Oscillator              | Voice 3 Oscillator     | */
/* | $1C  | Voice 3 Envelope               | Voice 3 Envelope       | */
/* ------------------------------------------------------------------ */

class SoundCard: public IO {
  private:
    uint8_t registers[32];
    uint32_t v3Accumulator;
    uint32_t v3NoiseShift;

  public:
    SoundCard();
    ~SoundCard() {};

    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick(uint32_t cpuFrequency) override;
    void    reset() override;
};

#endif
