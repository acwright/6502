#include "RAM.h"

RAM::RAM() {
  uint16_t size = RAM_END - RAM_START + 1;

  this->data = new uint8_t[size];

  for (uint16_t a = 0x0000; a < size; a++) {
    this->data[a] = 0x00; 
  }
}

RAM::~RAM() {
  delete[] this->data;
}

uint8_t RAM::read(uint16_t address) {
  return this->data[address];
}

void RAM::write(uint16_t address, uint8_t value) {
  this->data[address] = value;
}