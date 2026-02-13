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
  if (index <= (ROM_END - ROM_START)) {
    return this->data[index];
  } else {
    return 0x00;
  }
}

void ROM::write(uint16_t index, uint8_t value) {
  if (index <= (ROM_END - ROM_START)) {
    this->data[index] = value;
  }
}