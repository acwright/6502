#ifndef _RTC_H
#define _RTC_H

#include <Arduino.h>

class RTC {

public:

  RTC();

  void begin();
  void update();
  byte read(word address);
  void write(word address, byte data);
  
};

#endif