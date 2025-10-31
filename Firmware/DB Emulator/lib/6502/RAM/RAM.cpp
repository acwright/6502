#include "RAM.h"

RAM::RAM() {
  uint16_t size = RAM_END - RAM_START + 1;

  this->data = new uint8_t[size];

  for (uint16_t i = 0x0000; i < size; i++) {
    this->data[i] = 0x00; 
  }
}

RAM::~RAM() {
  delete[] this->data;
}

uint8_t RAM::read(uint16_t index) {
  return this->data[index];
}

void RAM::write(uint16_t index, uint8_t value) {
  this->data[index] = value;
}