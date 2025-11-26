#include "RAMCard.h"

RAMCard::RAMCard(uint8_t *data) {
  this->data = data;
	this->bank = 0x00;

  for (size_t i = 0; i < RC_BLOCK_SIZE * RC_BLOCK_COUNT; i++) {
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