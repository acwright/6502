// 6502 CPU
// Adapted from: https://github.com/OneLoneCoder/olcNES/blob/master/Part%232%20-%20CPU/olc6502.cpp

export interface CPUInstruction {
  name: string
  cycles: number
  opcode: () => number
  addrMode: () => number
}

export class CPU {

  private static C: number = 0b00000001
  private static Z: number = 0b00000010
  private static I: number = 0b00000100
  private static D: number = 0b00001000
  private static B: number = 0b00010000
  private static U: number = 0b00100000
  private static V: number = 0b01000000
  private static N: number = 0b10000000

  private fetched: number   = 0x00    // Working input value to the ALU
  private temp: number      = 0x0000  // A convenience var used everywhere
  private addrAbs: number   = 0x0000  // All used memory addresses end up here
  private addrRel: number   = 0x0000  // Represents abs address following a branch
  private opcode: number    = 0x00    // The instruction byte
  private cyclesRem: number = 0       // Counts how many cycles the current instruction has remaining

  cycles: number            = 0       // Counts the total number of cycles executed

  a: number   = 0x00
  x: number   = 0x00
  y: number   = 0x00
  pc: number  = 0x0000
  sp: number  = 0xFD
  st: number  = 0x00 | CPU.U

  read: (address: number) => number
  write: (address: number, data: number) => void

  constructor(
    read: (address: number) => number,
    write: (address: number, data: number) => void
  ) {
    this.read = read
    this.write = write

    this.reset()
  }

  reset(): void {
    // Read the PC location from the Reset vector
    const resetVector = 0xFFFC
    const lo: number = this.read(resetVector + 0)
    const hi: number = this.read(resetVector + 1)
    this.pc = (hi << 8) | lo

    // Clear the registers
    this.a = 0x00
    this.x = 0x00
    this.y = 0x00
    this.sp = 0xFD
    this.st = 0x00 | CPU.U

    // Clear our helper variables
    this.addrRel = 0x0000
    this.addrAbs = 0x0000
    this.fetched = 0x00

    // Reset takes 7 clock cycles
    this.cyclesRem = 7
    this.cycles += 7
  }

  irq(): void {
    // Are interrupts enabled?
    if (this.getFlag(CPU.I) == 0) {
      // Push the program counter onto the stack
      this.write(0x0100 + this.sp, (this.pc >> 8) & 0x00FF)
      this.decSP()
      this.write(0x0100 + this.sp, this.pc & 0x00FF)
      this.decSP()

      // Push the status register onto the stack
      this.setFlag(CPU.B, false)
      this.setFlag(CPU.I, true)
      this.write(0x0100 + this.sp, this.st)
      this.decSP()

      // Read new PC location from IRQ vector
      const irqVector = 0xFFFE
      const lo: number = this.read(irqVector + 0)
      const hi: number = this.read(irqVector + 1)
      this.pc = (hi << 8) | lo

      // IRQ takes 7 clock cycles
      this.cyclesRem = 7
      this.cycles += 7
    }
  }

  nmi(): void {
      // Push the program counter onto the stack
      this.write(0x0100 + this.sp, (this.pc >> 8) & 0x00FF)
      this.decSP()
      this.write(0x0100 + this.sp, this.pc & 0x00FF)
      this.decSP()

      // Push the status register onto the stack
      this.setFlag(CPU.B, false)
      this.setFlag(CPU.I, true)
      this.write(0x0100 + this.sp, this.st)
      this.decSP()

      // Read new PC location from NMI vector
      const nmiVector = 0xFFFA
      const lo: number = this.read(nmiVector + 0)
      const hi: number = this.read(nmiVector + 1)
      this.pc = (hi << 8) | lo

      // NMI takes 8 clock cycles
      this.cyclesRem = 7
      this.cycles += 7
  }

  // Perform one clock cycle
  tick(): void {
    if (this.cyclesRem == 0) {
      this.opcode = this.read(this.pc)
      this.incPC()

      const instruction = this.instructionTable[this.opcode]

      this.cyclesRem  = instruction.cycles
      this.cycles     += instruction.cycles

      const addCycleAddrMode  = instruction.addrMode()
      const addCycleOpcode    = instruction.opcode()
      
      // addrMode() and opcode() return 1 or 0 if additional clock cycles are required
      // TODO: Why is this & instead of +?
      this.cyclesRem += addCycleAddrMode & addCycleOpcode
      this.cycles    += addCycleAddrMode & addCycleOpcode
    }

    this.cyclesRem--
  }

  // Execute one instruction
  step(): number {
    // Finish current instruction
    if (this.cyclesRem > 0) {
      do {
        this.tick()
      } while (this.cyclesRem > 0)
    }

    const startCycles = this.cycles

    do {
      this.tick()
    } while (this.cyclesRem > 0)

    return this.cycles - startCycles
  }

  // Fetch the data
  private fetch(): number {
    if (!(this.instructionTable[this.opcode].addrMode == this.IMP)) {
      this.fetched = this.read(this.addrAbs)
    }
    return this.fetched
  }

  // Get the value of flag in the status register
  private getFlag(flag: number): number {
    return (this.st & flag) > 0 ? 1 : 0
  }

  // Set or clear a flag in the status register
  private setFlag(flag: number, value: boolean): void {
    if (value) {
      this.st |= flag
    } else {
      this.st &= ~flag
    }
  }

  private incPC() {
    this.pc++
    if (this.pc > 0xFFFF) {
      this.pc = 0x0000
    }
  }

  private decPC() {
    if (this.pc > 0x0000) {
      this.pc--
    } else {
      this.pc = 0xFFFF
    }
  }

  private incSP() {
    this.sp++
    if (this.sp > 0xFF) {
      this.sp = 0x00
    }
  }

  private decSP() {
    if (this.sp > 0x00) {
      this.sp--
    } else {
      this.sp = 0xFF
    }
  }

  // Addressing Modes

  private IMP(): number {
    this.fetched = this.a
    return 0 
  }

  private IMM(): number {
    this.addrAbs = this.pc++;
    return 0
  }

  private ZP0(): number {
    this.addrAbs = this.read(this.pc)
    this.incPC()
    this.addrAbs &= 0x00FF
    return 0
  }

  private ZPX(): number {
    this.addrAbs = this.read(this.pc) + this.x
    this.incPC()
    this.addrAbs &= 0x00FF
    return 0
  }

  private ZPY(): number {
    this.addrAbs = this.read(this.pc) + this.y
    this.incPC()
    this.addrAbs &= 0x00FF
    return 0
  }

  private REL(): number {
    this.addrRel = this.read(this.pc)
    this.incPC()
    if (this.addrRel & 0x80) {
      this.addrRel |= 0xFF00
    }
    return 0
  }

  private ABS(): number {
    const lo: number = this.read(this.pc)
    this.incPC()
    const hi: number = this.read(this.pc)
    this.incPC()

    this.addrAbs = (hi << 8) | lo

    return 0
  }

  private ABX(): number {
    const lo: number = this.read(this.pc)
    this.incPC()
    const hi: number = this.read(this.pc)
    this.incPC()

    this.addrAbs = (hi << 8) | lo
    this.addrAbs += this.x

    if ((this.addrAbs & 0xFF00) != (hi << 8)) {
      return 1
    } else {
      return 0
    }
  }

  private ABY(): number {
    const lo: number = this.read(this.pc)
    this.incPC()
    const hi: number = this.read(this.pc)
    this.incPC()

    this.addrAbs = (hi << 8) | lo
    this.addrAbs += this.y

    if ((this.addrAbs & 0xFF00) != (hi << 8)) {
      return 1
    } else {
      return 0
    }
  }

  private IND(): number {
    const ptrLo: number = this.read(this.pc)
    this.incPC()
    const ptrHi: number = this.read(this.pc)
    this.incPC()

    const ptr = (ptrHi << 8) | ptrLo

    // Simulate page boundary hardware bug
    if (ptrLo == 0x00FF) {
      this.addrAbs = (this.read(ptr & 0xFF00) << 8) | this.read(ptr)
    } else {
      this.addrAbs = (this.read(ptr + 1) << 8) | this.read(ptr)
    }

    return 0
  }

  private IZX(): number {
    const t = this.read(this.pc)
    this.incPC()

    const lo = this.read((t + this.x) & 0x00FF)
    const hi = this.read((t + this.x + 1) & 0x00FF)

    this.addrAbs = (hi << 8) | lo

    // TODO: Why is this different than IZY?

    return 0
  }

  private IZY(): number {
    const t = this.read(this.pc)
    this.incPC()

    const lo = this.read((t ) & 0x00FF)
    const hi = this.read((t + 1) & 0x00FF)

    this.addrAbs = (hi << 8) | lo
    this.addrAbs += this.y

    if ((this.addrAbs & 0xFF00) != (hi << 8)) {
      return 1
    } else {
      return 0
    }
  }

  // Opcodes
  // https://github.com/OneLoneCoder/olcNES/blob/master/Part%232%20-%20CPU/olc6502.cpp

  private ADC(): number { return 0 }
  
  private AND(): number { return 0 }
  private ASL(): number { return 0 }
  private BCC(): number { return 0 }
  private BCS(): number { return 0 }
  private BEQ(): number { return 0 }
  private BIT(): number { return 0 }
  private BMI(): number { return 0 }
  private BNE(): number { return 0 }
  private BPL(): number { return 0 }
  private BRK(): number { return 0 }
  private BVC(): number { return 0 }
  private BVS(): number { return 0 }
  private CLC(): number { return 0 }
  private CLD(): number { return 0 }
  private CLI(): number { return 0 }
  private CLV(): number { return 0 }
  private CMP(): number { return 0 }
  private CPX(): number { return 0 }
  private CPY(): number { return 0 }
  private DEC(): number { return 0 }
  private DEX(): number { return 0 }
  private DEY(): number { return 0 }
  private EOR(): number { return 0 }
  private INC(): number { return 0 }
  private INX(): number { return 0 }
  private INY(): number { return 0 }
  private JMP(): number { return 0 }
  private JSR(): number { return 0 }
  private LDA(): number { return 0 }
  private LDX(): number { return 0 }
  private LDY(): number { return 0 }
  private LSR(): number { return 0 }

  private NOP(): number {
    switch (this.opcode) {
      case 0x1C:
      case 0x3C:
      case 0x5C:
      case 0x7C:
      case 0xDC:
      case 0xFC:
        return 1
      }
    return 0
  }

  private ORA(): number {
    this.fetch()
    this.a |= this.fetched
    this.setFlag(CPU.Z, this.a == 0x00)
    this.setFlag(CPU.N, (this.a & 0x80) != 0)
    return 1
  }

  private PHA(): number {
    this.write(0x0100 + this.sp, this.a)
    this.decSP()
    return 0
  }

  private PHP(): number {
    this.write(0x0100 + this.sp, this.st | CPU.B | CPU.U)
    this.setFlag(CPU.B, false)
    this.setFlag(CPU.U, false)
    this.decSP()
    return 0
  }

  private PLA(): number {
    this.incSP()
    this.a = this.read(0x0100 + this.sp)
    this.setFlag(CPU.Z, this.a == 0x00)
    this.setFlag(CPU.N, (this.a & 0x80) != 0)
    return 0
  }

  private PLP(): number {
    this.incSP()
    this.st = this.read(0x0100 + this.sp)
    this.setFlag(CPU.U, true)
    return 0
  }

  private ROL(): number {
    this.fetch()
    this.temp = (this.fetched << 1) | this.getFlag(CPU.C)
    this.setFlag(CPU.C, (this.temp & 0xFF00) != 0)
    this.setFlag(CPU.Z, (this.temp & 0x00FF) == 0x00)
    this.setFlag(CPU.N, (this.temp & 0x0080) != 0)
    if (this.instructionTable[this.opcode].addrMode == this.IMP) {
      this.a = this.temp & 0x00FF
    } else {
      this.write(this.addrAbs, this.temp & 0x00FF)
    }
    return 0
  }

  private ROR(): number {
    this.fetch()
    this.temp = (this.getFlag(CPU.C) << 7) | (this.fetched >> 1)
    this.setFlag(CPU.C, (this.fetched & 0x01) != 0)
    this.setFlag(CPU.Z, (this.temp & 0x00FF) == 0x00)
    this.setFlag(CPU.N, (this.temp & 0x0080) != 0)
    if (this.instructionTable[this.opcode].addrMode == this.IMP) {
      this.a = this.temp & 0x00FF
    } else {
      this.write(this.addrAbs, this.temp & 0x00FF)
    }
    return 0
  }

  private RTI(): number {
    this.incSP()
    this.st = this.read(0x0100 + this.sp)
    this.st &= ~CPU.B
    this.st &= ~CPU.U
    this.incSP()
    this.pc = this.read(0x0100 + this.sp)
    this.incSP()
    this.pc |= this.read(0x0100 + this.sp) << 8
    return 0
  }

  private RTS(): number {
    this.incSP()
    this.pc = this.read(0x0100 + this.sp)
    this.incSP()
    this.pc |= this.read(0x0100 + this.sp) << 8
    this.incPC()

    return 0
  }

  private SBC(): number {
    this.fetch()

    const value = this.fetched ^ 0x00FF
    
    this.temp = this.a + value + this.getFlag(CPU.C)
    this.setFlag(CPU.C, (this.temp & 0xFF00) != 0)
    this.setFlag(CPU.Z, (this.temp & 0x00FF) == 0)
    this.setFlag(CPU.V, ((this.temp ^ this.a) & (this.temp ^ value) & 0x0080) != 0)
    this.setFlag(CPU.N, (this.temp & 0x0080) != 0)

    this.a = this.temp & 0x00FF

    return 1
  }

  private SEC(): number {
    this.setFlag(CPU.C, true)
    return 0
  }

  private SED(): number {
    this.setFlag(CPU.D, true)
    return 0
  }

  private SEI(): number {
    this.setFlag(CPU.I, true)
    return 0
  }

  private STA(): number {
    this.write(this.addrAbs, this.a)
	  return 0
  }

  private STX(): number {
    this.write(this.addrAbs, this.x)
	  return 0
  }

  private STY(): number {
    this.write(this.addrAbs, this.y)
	  return 0
  }

  private TAX(): number {
    this.x = this.a
    this.setFlag(CPU.Z, this.x == 0x00)
    this.setFlag(CPU.N, (this.x & 0x80) != 0)
    return 0
  }

  private TAY(): number {
    this.y = this.a
    this.setFlag(CPU.Z, this.y == 0x00)
    this.setFlag(CPU.N, (this.y & 0x80) != 0)
    return 0
  }

  private TSX(): number {
    this.x = this.sp
    this.setFlag(CPU.Z, this.x == 0x00)
    this.setFlag(CPU.N, (this.x & 0x80) != 0)
    return 0
  }

  private TXA(): number {
    this.a = this.x
    this.setFlag(CPU.Z, this.a == 0x00)
    this.setFlag(CPU.N, (this.a & 0x80) != 0)
    return 0
  }

  private TXS(): number {
    this.sp = this.x
    return 0
  }

  private TYA(): number {
    this.a = this.y
    this.setFlag(CPU.Z, this.a == 0x00)
    this.setFlag(CPU.N, (this.a & 0x80) != 0)
    return 0
  }

  private XXX(): number { return 0 }

  instructionTable: CPUInstruction[] = [
    { name: 'BRK', cycles: 7, opcode: this.BRK, addrMode: this.IMM },
    { name: 'ORA', cycles: 6, opcode: this.ORA, addrMode: this.IZX },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 8, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 3, opcode: this.NOP, addrMode: this.IMP },
    { name: 'ORA', cycles: 3, opcode: this.ORA, addrMode: this.ZP0 },
    { name: 'ASL', cycles: 5, opcode: this.ASL, addrMode: this.ZP0 },
    { name: '???', cycles: 5, opcode: this.XXX, addrMode: this.IMP },
    { name: 'PHP', cycles: 3, opcode: this.PHP, addrMode: this.IMP },
    { name: 'ORA', cycles: 2, opcode: this.ORA, addrMode: this.IMM },
    { name: 'ASL', cycles: 2, opcode: this.ASL, addrMode: this.IMP },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.NOP, addrMode: this.IMP },
    { name: 'ORA', cycles: 4, opcode: this.ORA, addrMode: this.ABS },
    { name: 'ASL', cycles: 6, opcode: this.ASL, addrMode: this.ABS },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },

    { name: 'BPL', cycles: 2, opcode: this.BPL, addrMode: this.REL },
    { name: 'ORA', cycles: 5, opcode: this.ORA, addrMode: this.IZY },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 8, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.NOP, addrMode: this.IMP },
    { name: 'ORA', cycles: 4, opcode: this.ORA, addrMode: this.ZPX },
    { name: 'ASL', cycles: 6, opcode: this.ASL, addrMode: this.ZPX },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },
    { name: 'CLC', cycles: 2, opcode: this.CLC, addrMode: this.IMP },
    { name: 'ORA', cycles: 4, opcode: this.ORA, addrMode: this.ABY },
    { name: '???', cycles: 2, opcode: this.NOP, addrMode: this.IMP },
    { name: '???', cycles: 7, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.NOP, addrMode: this.IMP },
    { name: 'ORA', cycles: 4, opcode: this.ORA, addrMode: this.ABX },
    { name: 'ASL', cycles: 7, opcode: this.ASL, addrMode: this.ABX },
    { name: '???', cycles: 7, opcode: this.XXX, addrMode: this.IMP },

    { name: 'JSR', cycles: 6, opcode: this.JSR, addrMode: this.ABS },
    { name: 'AND', cycles: 6, opcode: this.AND, addrMode: this.IZX },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 8, opcode: this.XXX, addrMode: this.IMP },
    { name: 'BIT', cycles: 3, opcode: this.BIT, addrMode: this.ZP0 },
    { name: 'AND', cycles: 3, opcode: this.AND, addrMode: this.ZP0 },
    { name: 'ROL', cycles: 5, opcode: this.ROL, addrMode: this.ZP0 },
    { name: '???', cycles: 5, opcode: this.XXX, addrMode: this.IMP },
    { name: 'PLP', cycles: 4, opcode: this.PLP, addrMode: this.IMP },
    { name: 'AND', cycles: 2, opcode: this.AND, addrMode: this.IMM },
    { name: 'ROL', cycles: 2, opcode: this.ROL, addrMode: this.IMP },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: 'BIT', cycles: 4, opcode: this.BIT, addrMode: this.ABS },
    { name: 'AND', cycles: 4, opcode: this.AND, addrMode: this.ABS },
    { name: 'ROL', cycles: 6, opcode: this.ROL, addrMode: this.ABS },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },

    { name: 'BMI', cycles: 2, opcode: this.BMI, addrMode: this.REL },
    { name: 'AND', cycles: 5, opcode: this.AND, addrMode: this.IZY },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 8, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.NOP, addrMode: this.IMP },
    { name: 'AND', cycles: 4, opcode: this.AND, addrMode: this.ZPX },
    { name: 'ROL', cycles: 6, opcode: this.ROL, addrMode: this.ZPX },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },
    { name: 'SEC', cycles: 2, opcode: this.SEC, addrMode: this.IMP },
    { name: 'AND', cycles: 4, opcode: this.AND, addrMode: this.ABY },
    { name: '???', cycles: 2, opcode: this.NOP, addrMode: this.IMP },
    { name: '???', cycles: 7, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.NOP, addrMode: this.IMP },
    { name: 'AND', cycles: 4, opcode: this.AND, addrMode: this.ABX },
    { name: 'ROL', cycles: 7, opcode: this.ROL, addrMode: this.ABX },
    { name: '???', cycles: 7, opcode: this.XXX, addrMode: this.IMP },

    { name: 'RTI', cycles: 6, opcode: this.RTI, addrMode: this.IMP },
    { name: 'EOR', cycles: 6, opcode: this.EOR, addrMode: this.IZX },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 8, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 3, opcode: this.NOP, addrMode: this.IMP },
    { name: 'EOR', cycles: 3, opcode: this.EOR, addrMode: this.ZP0 },
    { name: 'LSR', cycles: 5, opcode: this.LSR, addrMode: this.ZP0 },
    { name: '???', cycles: 5, opcode: this.XXX, addrMode: this.IMP },
    { name: 'PHA', cycles: 3, opcode: this.PHA, addrMode: this.IMP },
    { name: 'EOR', cycles: 2, opcode: this.EOR, addrMode: this.IMM },
    { name: 'LSR', cycles: 2, opcode: this.LSR, addrMode: this.IMP },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: 'JMP', cycles: 3, opcode: this.JMP, addrMode: this.ABS },
    { name: 'EOR', cycles: 4, opcode: this.EOR, addrMode: this.ABS },
    { name: 'LSR', cycles: 6, opcode: this.LSR, addrMode: this.ABS },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },

    { name: 'BVC', cycles: 2, opcode: this.BVC, addrMode: this.REL },
    { name: 'EOR', cycles: 5, opcode: this.EOR, addrMode: this.IZY },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 8, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.NOP, addrMode: this.IMP },
    { name: 'EOR', cycles: 4, opcode: this.EOR, addrMode: this.ZPX },
    { name: 'LSR', cycles: 6, opcode: this.LSR, addrMode: this.ZPX },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },
    { name: 'CLI', cycles: 2, opcode: this.CLI, addrMode: this.IMP },
    { name: 'EOR', cycles: 4, opcode: this.EOR, addrMode: this.ABY },
    { name: '???', cycles: 2, opcode: this.NOP, addrMode: this.IMP },
    { name: '???', cycles: 7, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.NOP, addrMode: this.IMP },
    { name: 'EOR', cycles: 4, opcode: this.EOR, addrMode: this.ABX },
    { name: 'LSR', cycles: 7, opcode: this.LSR, addrMode: this.ABX },
    { name: '???', cycles: 7, opcode: this.XXX, addrMode: this.IMP },

    { name: 'RTS', cycles: 6, opcode: this.RTS, addrMode: this.IMP },
    { name: 'ADC', cycles: 6, opcode: this.ADC, addrMode: this.IZX },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 8, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 3, opcode: this.NOP, addrMode: this.IMP },
    { name: 'ADC', cycles: 3, opcode: this.ADC, addrMode: this.ZP0 },
    { name: 'ROR', cycles: 5, opcode: this.ROR, addrMode: this.ZP0 },
    { name: '???', cycles: 5, opcode: this.XXX, addrMode: this.IMP },
    { name: 'PLA', cycles: 4, opcode: this.PLA, addrMode: this.IMP },
    { name: 'ADC', cycles: 2, opcode: this.ADC, addrMode: this.IMM },
    { name: 'ROR', cycles: 2, opcode: this.ROR, addrMode: this.IMP },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: 'JMP', cycles: 5, opcode: this.JMP, addrMode: this.IND },
    { name: 'ADC', cycles: 4, opcode: this.ADC, addrMode: this.ABS },
    { name: 'ROR', cycles: 6, opcode: this.ROR, addrMode: this.ABS },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },

    { name: 'BVS', cycles: 2, opcode: this.BVS, addrMode: this.REL },
    { name: 'ADC', cycles: 5, opcode: this.ADC, addrMode: this.IZY },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 8, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.NOP, addrMode: this.IMP },
    { name: 'ADC', cycles: 4, opcode: this.ADC, addrMode: this.ZPX },
    { name: 'ROR', cycles: 6, opcode: this.ROR, addrMode: this.ZPX },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },
    { name: 'SEI', cycles: 2, opcode: this.SEI, addrMode: this.IMP },
    { name: 'ADC', cycles: 4, opcode: this.ADC, addrMode: this.ABY },
    { name: '???', cycles: 2, opcode: this.NOP, addrMode: this.IMP },
    { name: '???', cycles: 7, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.NOP, addrMode: this.IMP },
    { name: 'ADC', cycles: 4, opcode: this.ADC, addrMode: this.ABX },
    { name: 'ROR', cycles: 7, opcode: this.ROR, addrMode: this.ABX },
    { name: '???', cycles: 7, opcode: this.XXX, addrMode: this.IMP },

    { name: '???', cycles: 2, opcode: this.NOP, addrMode: this.IMP },
    { name: 'STA', cycles: 6, opcode: this.STA, addrMode: this.IZX },
    { name: '???', cycles: 2, opcode: this.NOP, addrMode: this.IMP },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },
    { name: 'STY', cycles: 3, opcode: this.STY, addrMode: this.ZP0 },
    { name: 'STA', cycles: 3, opcode: this.STA, addrMode: this.ZP0 },
    { name: 'STX', cycles: 3, opcode: this.STX, addrMode: this.ZP0 },
    { name: '???', cycles: 3, opcode: this.XXX, addrMode: this.IMP },
    { name: 'DEY', cycles: 2, opcode: this.DEY, addrMode: this.IMP },
    { name: '???', cycles: 2, opcode: this.NOP, addrMode: this.IMP },
    { name: 'TXA', cycles: 2, opcode: this.TXA, addrMode: this.IMP },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: 'STY', cycles: 4, opcode: this.STY, addrMode: this.ABS },
    { name: 'STA', cycles: 4, opcode: this.STA, addrMode: this.ABS },
    { name: 'STX', cycles: 4, opcode: this.STX, addrMode: this.ABS },
    { name: '???', cycles: 4, opcode: this.XXX, addrMode: this.IMP },

    { name: 'BCC', cycles: 2, opcode: this.BCC, addrMode: this.REL },
    { name: 'STA', cycles: 6, opcode: this.STA, addrMode: this.IZY },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },
    { name: 'STY', cycles: 4, opcode: this.STY, addrMode: this.ZPX },
    { name: 'STA', cycles: 4, opcode: this.STA, addrMode: this.ZPX },
    { name: 'STX', cycles: 4, opcode: this.STX, addrMode: this.ZPY },
    { name: '???', cycles: 4, opcode: this.XXX, addrMode: this.IMP },
    { name: 'TYA', cycles: 2, opcode: this.TYA, addrMode: this.IMP },
    { name: 'STA', cycles: 5, opcode: this.STA, addrMode: this.ABY },
    { name: 'TXS', cycles: 2, opcode: this.TXS, addrMode: this.IMP },
    { name: '???', cycles: 5, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 5, opcode: this.NOP, addrMode: this.IMP },
    { name: 'STA', cycles: 5, opcode: this.STA, addrMode: this.ABX },
    { name: '???', cycles: 5, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 5, opcode: this.XXX, addrMode: this.IMP },

    { name: 'LDY', cycles: 2, opcode: this.LDY, addrMode: this.IMM },
    { name: 'LDA', cycles: 6, opcode: this.LDA, addrMode: this.IZX },
    { name: 'LDX', cycles: 2, opcode: this.LDX, addrMode: this.IMM },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },
    { name: 'LDY', cycles: 3, opcode: this.LDY, addrMode: this.ZP0 },
    { name: 'LDA', cycles: 3, opcode: this.LDA, addrMode: this.ZP0 },
    { name: 'LDX', cycles: 3, opcode: this.LDX, addrMode: this.ZP0 },
    { name: '???', cycles: 3, opcode: this.XXX, addrMode: this.IMP },
    { name: 'TAY', cycles: 2, opcode: this.TAY, addrMode: this.IMP },
    { name: 'LDA', cycles: 2, opcode: this.LDA, addrMode: this.IMM },
    { name: 'TAX', cycles: 2, opcode: this.TAX, addrMode: this.IMP },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: 'LDY', cycles: 4, opcode: this.LDY, addrMode: this.ABS },
    { name: 'LDA', cycles: 4, opcode: this.LDA, addrMode: this.ABS },
    { name: 'LDX', cycles: 4, opcode: this.LDX, addrMode: this.ABS },
    { name: '???', cycles: 4, opcode: this.XXX, addrMode: this.IMP },

    { name: 'BCS', cycles: 2, opcode: this.BCS, addrMode: this.REL },
    { name: 'LDA', cycles: 5, opcode: this.LDA, addrMode: this.IZY },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 5, opcode: this.XXX, addrMode: this.IMP },
    { name: 'LDY', cycles: 4, opcode: this.LDY, addrMode: this.ZPX },
    { name: 'LDA', cycles: 4, opcode: this.LDA, addrMode: this.ZPX },
    { name: 'LDX', cycles: 4, opcode: this.LDX, addrMode: this.ZPY },
    { name: '???', cycles: 4, opcode: this.XXX, addrMode: this.IMP },
    { name: 'CLV', cycles: 2, opcode: this.CLV, addrMode: this.IMP },
    { name: 'LDA', cycles: 4, opcode: this.LDA, addrMode: this.ABY },
    { name: 'TSX', cycles: 2, opcode: this.TSX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.XXX, addrMode: this.IMP },
    { name: 'LDY', cycles: 4, opcode: this.LDY, addrMode: this.ABX },
    { name: 'LDA', cycles: 4, opcode: this.LDA, addrMode: this.ABX },
    { name: 'LDX', cycles: 4, opcode: this.LDX, addrMode: this.ABY },
    { name: '???', cycles: 4, opcode: this.XXX, addrMode: this.IMP },

    { name: 'CPY', cycles: 2, opcode: this.CPY, addrMode: this.IMM },
    { name: 'CMP', cycles: 6, opcode: this.CMP, addrMode: this.IZX },
    { name: '???', cycles: 2, opcode: this.NOP, addrMode: this.IMP },
    { name: '???', cycles: 8, opcode: this.XXX, addrMode: this.IMP },
    { name: 'CPY', cycles: 3, opcode: this.CPY, addrMode: this.ZP0 },
    { name: 'CMP', cycles: 3, opcode: this.CMP, addrMode: this.ZP0 },
    { name: 'DEC', cycles: 5, opcode: this.DEC, addrMode: this.ZP0 },
    { name: '???', cycles: 5, opcode: this.XXX, addrMode: this.IMP },
    { name: 'INY', cycles: 2, opcode: this.INY, addrMode: this.IMP },
    { name: 'CMP', cycles: 2, opcode: this.CMP, addrMode: this.IMM },
    { name: 'DEX', cycles: 2, opcode: this.DEX, addrMode: this.IMP },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: 'CPY', cycles: 4, opcode: this.CPY, addrMode: this.ABS },
    { name: 'CMP', cycles: 4, opcode: this.CMP, addrMode: this.ABS },
    { name: 'DEC', cycles: 6, opcode: this.DEC, addrMode: this.ABS },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },

    { name: 'BNE', cycles: 2, opcode: this.BNE, addrMode: this.REL },
    { name: 'CMP', cycles: 5, opcode: this.CMP, addrMode: this.IZY },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 8, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.NOP, addrMode: this.IMP },
    { name: 'CMP', cycles: 4, opcode: this.CMP, addrMode: this.ZPX },
    { name: 'DEC', cycles: 6, opcode: this.DEC, addrMode: this.ZPX },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },
    { name: 'CLD', cycles: 2, opcode: this.CLD, addrMode: this.IMP },
    { name: 'CMP', cycles: 4, opcode: this.CMP, addrMode: this.ABY },
    { name: 'NOP', cycles: 2, opcode: this.NOP, addrMode: this.IMP },
    { name: '???', cycles: 7, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.NOP, addrMode: this.IMP },
    { name: 'CMP', cycles: 4, opcode: this.CMP, addrMode: this.ABX },
    { name: 'DEC', cycles: 7, opcode: this.DEC, addrMode: this.ABX },
    { name: '???', cycles: 7, opcode: this.XXX, addrMode: this.IMP },

    { name: 'CPX', cycles: 2, opcode: this.CPX, addrMode: this.IMM },
    { name: 'SBC', cycles: 6, opcode: this.SBC, addrMode: this.IZX },
    { name: '???', cycles: 2, opcode: this.NOP, addrMode: this.IMP },
    { name: '???', cycles: 8, opcode: this.XXX, addrMode: this.IMP },
    { name: 'CPX', cycles: 3, opcode: this.CPX, addrMode: this.ZP0 },
    { name: 'SBC', cycles: 3, opcode: this.SBC, addrMode: this.ZP0 },
    { name: 'INC', cycles: 5, opcode: this.INC, addrMode: this.ZP0 },
    { name: '???', cycles: 5, opcode: this.XXX, addrMode: this.IMP },
    { name: 'INX', cycles: 2, opcode: this.INX, addrMode: this.IMP },
    { name: 'SBC', cycles: 2, opcode: this.SBC, addrMode: this.IMM },
    { name: 'NOP', cycles: 2, opcode: this.NOP, addrMode: this.IMP },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: 'CPX', cycles: 4, opcode: this.CPX, addrMode: this.ABS },
    { name: 'SBC', cycles: 4, opcode: this.SBC, addrMode: this.ABS },
    { name: 'INC', cycles: 6, opcode: this.INC, addrMode: this.ABS },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },

    { name: 'BEQ', cycles: 2, opcode: this.BEQ, addrMode: this.REL },
    { name: 'SBC', cycles: 5, opcode: this.SBC, addrMode: this.IZY },
    { name: '???', cycles: 2, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 8, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.NOP, addrMode: this.IMP },
    { name: 'SBC', cycles: 4, opcode: this.SBC, addrMode: this.ZPX },
    { name: 'INC', cycles: 6, opcode: this.INC, addrMode: this.ZPX },
    { name: '???', cycles: 6, opcode: this.XXX, addrMode: this.IMP },
    { name: 'SED', cycles: 2, opcode: this.SED, addrMode: this.IMP },
    { name: 'SBC', cycles: 4, opcode: this.SBC, addrMode: this.ABY },
    { name: 'NOP', cycles: 2, opcode: this.NOP, addrMode: this.IMP },
    { name: '???', cycles: 7, opcode: this.XXX, addrMode: this.IMP },
    { name: '???', cycles: 4, opcode: this.NOP, addrMode: this.IMP },
    { name: 'SBC', cycles: 4, opcode: this.SBC, addrMode: this.ABX },
    { name: 'INC', cycles: 7, opcode: this.INC, addrMode: this.ABX },
    { name: '???', cycles: 7, opcode: this.XXX, addrMode: this.IMP } 
  ]

}