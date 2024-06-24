#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <Arduino.h>

void setAddrDirIn();
void setDataDirIn();
void setDataDirOut();
word readAddress();
byte readData();
void writeData(byte d);

#endif