#include "Cart.h"

Cart::Cart() {
  uint16_t size = CART_END - CART_START + 1;

  this->data = new uint8_t[size];

  for (uint16_t i = 0x0000; i < size; i++) {
    this->data[i] = 0xEA; // Load with NOPs
  }
}

Cart::~Cart() {
  delete[] this->data;
}

uint8_t Cart::read(uint16_t index) {
  return this->data[index];
}

void Cart::write(uint16_t index, uint8_t value) { /* Do nothing */ }

void Cart::load(uint16_t index, uint8_t value) {
  this->data[index] = value;
}