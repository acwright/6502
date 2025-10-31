#ifndef _CPU_H
#define _CPU_H

#include "vrEmu6502/vrEmu6502.h"

class CPU {
  private:
    VrEmu6502 *cpu;

  public:
    CPU(vrEmu6502MemRead readFn, vrEmu6502MemWrite writeFn);
    ~CPU();

    void reset();
    void tick();
    uint8_t step();
    void irq();
    void nmi();
};

#endif