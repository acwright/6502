#include "ROM.h"

ROM::ROM() {
  uint16_t size = ROM_END - ROM_START + 1;
  
  this->data = new uint8_t[size];

  for (uint16_t i = 0x0000; i < size; i++) {
    this->data[i] = 0xEA; // Load with NOPs
  }
}

ROM::~ROM() {
  delete[] this->data;
}

uint8_t ROM::read(uint16_t index) {
  return this->data[index];
}

void ROM::write(uint16_t index, uint8_t value) { /* Do nothing */ }

void ROM::load(uint16_t index, uint8_t value) {
  this->data[index] = value;
}