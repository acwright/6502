#include "CPU.h"

CPU::CPU(vrEmu6502MemRead readFn, vrEmu6502MemWrite writeFn) {
  this->cpu = vrEmu6502New(CPU_W65C02, readFn, writeFn);
  this->irq = vrEmu6502Int(this->cpu);
  this->nmi = vrEmu6502Nmi(this->cpu);
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

uint8_t CPU::step() {
  return vrEmu6502InstCycle(this->cpu);
}

void CPU::irqTrigger() {
  *irq = IntRequested;
}

void CPU::irqClear() {
  *irq = IntCleared;
}

void CPU::nmiTrigger() {
  *nmi = IntRequested;
}

uint16_t CPU::programCounter() { return vrEmu6502GetPC(this->cpu); }
uint8_t CPU::accumulator() { return vrEmu6502GetAcc(this->cpu); }
uint8_t CPU::x() { return vrEmu6502GetX(this->cpu); }
uint8_t CPU::y() { return vrEmu6502GetY(this->cpu); }
uint8_t CPU::status() { return vrEmu6502GetStatus(this->cpu); }
uint8_t CPU::stackPointer() { return vrEmu6502GetStackPointer(this->cpu); }
uint8_t CPU::opcodeCycle() { return vrEmu6502GetOpcodeCycle(this->cpu); }