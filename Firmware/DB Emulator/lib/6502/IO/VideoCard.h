#ifndef _VIDEO_CARD_H
#define _VIDEO_CARD_H

#include "IO.h"
#include "constants.h"

class VideoCard: public IO {
  private:

  public:
    VideoCard();
    ~VideoCard() {}

    uint8_t id() { return IO_VIDEO_CARD; }
    String  description() override { return "Video Card"; }
    bool    passthrough() override { return false; }
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick() { return 0x00; }
    void    reset() override;
};

#endif