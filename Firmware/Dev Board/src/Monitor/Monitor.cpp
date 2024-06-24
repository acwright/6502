#include "Monitor.h"

Monitor::Monitor() {}

void Monitor::begin()         { /* Do nothing... */ }
void Monitor::update()        { /* Do nothing... */ }
byte Monitor::read(byte reg)  { /* Do nothing... Monitor is write only */ }

void Monitor::write(byte reg, byte data) {
  switch(reg) {
  case MON_DATA:
    Serial.write(data);
    break;
  default:
    break;
  }
}
  