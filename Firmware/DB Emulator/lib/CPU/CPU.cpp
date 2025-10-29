#include "CPU.h"

CPU::CPU(vrEmu6502MemRead readFn, vrEmu6502MemWrite writeFn) {
  this->cpu = vrEmu6502New(CPU_W65C02, readFn, writeFn);
}

CPU::~CPU() {
  vrEmu6502Destroy(this->cpu);
}

void CPU::reset() {
  vrEmu6502Reset(this->cpu);
}

void CPU::tick() {
  vrEmu6502Tick(this->cpu);
}

void CPU::step() {
  vrEmu6502InstCycle(this->cpu);
}