#ifndef _RTC_CARD_H
#define _RTC_CARD_H

#include "IO.h"
#include "constants.h"

class RTCCard: public IO {
  private:

  public:
    RTCCard();
    ~RTCCard() {}

    uint8_t id() { return IO_RTC_CARD; }
    String  description() override { return "RTC Card"; }
    bool    passthrough() override { return false; }
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick() { return 0x00; }
    void    reset() override;
};

#endif