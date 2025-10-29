#include "Cart.h"

Cart::Cart(uint16_t size) {
  this->size = size;
  this->data = new uint8_t[size];

  for (uint16_t a = 0x0000; a < size; a++) {
    this->data[a] = 0xEA; // Load with NOPs
  }
}

Cart::~Cart() {
  delete[] this->data;
}

uint8_t Cart::read(uint16_t index) {
  return this->data[index];
}

void Cart::write(uint16_t index, uint8_t value) { /* Do nothing */ }