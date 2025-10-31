#include "RAMCard.h"

RAMCard::RAMCard() {
  uint16_t size = RC_BLOCK_SIZE * RC_BLOCK_COUNT;

  this->data = new uint8_t[size]; // 0x40000 (256K) if EXTMEM | 0x4000 (16K) if not
	this->bank = 0x00;

  for (int i = 0; i < size; i++) {
    this->data[i] = 0x00;
  }
}

RAMCard::~RAMCard() {
  delete[] this->data;
}

uint8_t RAMCard::read(uint16_t address) {
	#ifdef MEM_EXTMEM
	uint32_t index = (this->bank << 0xA) | address;
	#else
	uint32_t index = ((this->bank & 0x0F) << 0xA) | address;
	#endif

	return this->data[index];
}

void RAMCard::write(uint16_t address, uint8_t value) {
	if (address == 0x03FF) {
		this->bank = value;
	}

	#ifdef MEM_EXTMEM
	uint32_t index = (this->bank << 0xA) | address;
	#else
	uint32_t index = ((this->bank & 0x0F) << 0xA) | address;
	#endif

	this->data[index] = value;
}