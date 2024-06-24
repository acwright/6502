#include "RTC.h"

RTC::RTC() {}

void RTC::begin() {
  setSyncProvider(syncTime);
}

void RTC::update() { /* Do nothing... */ }

byte RTC::read(byte reg) {
  time_t time = now();

  switch(reg) {
  case RTC_SEC:
    return byte(second(time));
  case RTC_MIN:
    return byte(minute(time));
  case RTC_HR:
    return byte(hour(time));
  case RTC_DAY:
    return byte(day(time));
  case RTC_MON:
    return byte(month(time));
  case RTC_YR:
    return byte(year(time) - 1970); // Offset from 1970 (0-99)
  default:
    return 0;
  }
}

void RTC::write(byte reg, byte data) { /* Do nothing... RTC registers are read-only. */ }

String RTC::formattedDateTime() {
  String time;

  time.append(month());
  time.append("/");
  time.append(day());
  time.append("/");
  time.append(year());
  time.append(" ");
  time.append(hour());
  time.append(":");
  time.append(minute() < 10 ? "0": "");
  time.append(minute());
  time.append(":");
  time.append(second() < 10 ? "0": "");
  time.append(second());

  return time;
}

time_t RTC::syncTime() {
  return Teensy3Clock.get();
}