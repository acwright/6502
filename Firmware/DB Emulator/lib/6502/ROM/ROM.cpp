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

uint8_t ROM::read(uint16_t address) {
  return this->data[address];
}

void ROM::write(uint16_t address, uint8_t value) { /* Do nothing */ }

void ROM::load(uint16_t address, uint8_t value) {
  this->data[address] = value;
}