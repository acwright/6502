#include "ROM.h"

ROM::ROM(uint16_t size) {
  this->size = size;
  this->data = new uint8_t[size];

  for (uint16_t a = 0x0000; a < size; a++) {
    this->data[a] = 0xEA; // Load with NOPs
  }
}

ROM::~ROM() {
  delete[] this->data;
}

uint8_t ROM::read(uint16_t index) {
  return this->data[index];
}

void ROM::write(uint16_t index, uint8_t value) { /* Do nothing */ }