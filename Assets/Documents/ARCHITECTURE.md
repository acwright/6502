6502 System Architecture
========================

This document provides a detailed technical deep-dive into the architecture of the 6502 computer systems. Covers bus protocols, memory mapping, address decoding, timing, and hardware/software interaction.

**Last Updated:** March 7, 2026

---

## Table of Contents

- [Overview](#overview)
- [CPU Architecture](#cpu-architecture)
- [Memory Architecture](#memory-architecture)
- [Bus Architecture](#bus-architecture)
- [Address Decoding](#address-decoding)
- [Clock and Reset](#clock-and-reset)
- [Interrupt Handling](#interrupt-handling)
- [I/O Architecture](#io-architecture)
- [Timing and Bus Cycles](#timing-and-bus-cycles)
- [System Variants](#system-variants)

---

## Overview

All 6502 systems in this project share common architectural principles:

- **8-bit data bus** (D0-D7)
- **16-bit address bus** (A0-A15, addressing 64KB)
- **Memory-mapped I/O** (no separate I/O address space)
- **Single-phase clock** (PHI2) for bus synchronization
- **Little-endian byte ordering** (LSB at lower address)
- **Stack located at $0100-$01FF** (page 1)

---

## CPU Architecture

### W65C02S CMOS 6502

**Key Features:**
- CMOS version of original 6502 (lower power, higher speed)
- Up to 14 MHz operation (typically run at 1-8 MHz in this project)
- Additional instructions vs. NMOS 6502 (BRA, PHX, PHY, STZ, TRB, TSB, etc.)
- Fixes NMOS bugs (e.g., indirect JMP across page boundary)
- Static design (can be clocked arbitrarily slowly or stopped)

**Registers:**
- **A**: Accumulator (8-bit, primary data register)
- **X**: Index register X (8-bit, for indexed addressing)
- **Y**: Index register Y (8-bit, for indexed addressing)
- **SP**: Stack pointer (8-bit, points to page 1: $0100-$01FF)
- **PC**: Program counter (16-bit, current instruction address)
- **P**: Processor status (8-bit flags: NVxxDIZC)
  - **N**: Negative (bit 7 of result)
  - **V**: Overflow (signed arithmetic overflow)
  - **x**: Unused (bits 5 and 4)
  - **D**: Decimal mode (BCD arithmetic)
  - **I**: Interrupt disable
  - **Z**: Zero (result was zero)
  - **C**: Carry (unsigned overflow or borrow)

**Addressing Modes:**
1. **Implied**: Instruction operates on register (e.g., `TAX`)
2. **Accumulator**: Operates on accumulator (e.g., `ROL A`)
3. **Immediate**: Operand is literal value (e.g., `LDA #$42`)
4. **Zero Page**: Address in $00-$FF (e.g., `LDA $10`)
5. **Zero Page,X**: Zero page address + X (e.g., `LDA $10,X`)
6. **Zero Page,Y**: Zero page address + Y (e.g., `LDX $10,Y`)
7. **Absolute**: 16-bit address (e.g., `LDA $1234`)
8. **Absolute,X**: Absolute + X with carry (e.g., `LDA $1234,X`)
9. **Absolute,Y**: Absolute + Y with carry (e.g., `LDA $1234,Y`)
10. **Indirect**: Address points to address (e.g., `JMP ($1234)`)
11. **Indexed Indirect**: (Addr + X) points to address (e.g., `LDA ($10,X)`)
12. **Indirect Indexed**: (Addr) + Y points to address (e.g., `LDA ($10),Y`)
13. **Relative**: Signed offset for branches (e.g., `BNE label`)

### 65816 CPU (Optional Advanced Variant)

**Available in some board variants for 16-bit operation:**
- Backward compatible with 65C02 (emulation mode)
- Native mode: 16-bit accumulator and index registers
- 24-bit addressing (16MB address space)
- Requires additional support circuitry

---

## Memory Architecture

### Standard Memory Map

```
$0000 ┌─────────────────────────────────────┐
      │  Zero Page (256 bytes)              │  Fast access, special instructions
      │  $0000-$00FF                        │  System and user variables
$0100 ├─────────────────────────────────────┤
      │  Stack (256 bytes)                  │  Hardware stack (grows downward)
      │  $0100-$01FF                        │  SP points here ($01FF at reset)
$0200 ├─────────────────────────────────────┤
      │  Input Buffer (256 bytes)           │  Keyboard/command input buffer
      │  $0200-$02FF                        │
$0300 ├─────────────────────────────────────┤
      │  KERNAL Variables (256 bytes)       │  System variables
      │  $0300-$03FF                        │
$0400 ├─────────────────────────────────────┤
      │  User Variables (1KB)               │  User-accessible variables
      │  $0400-$07FF                        │
$0800 ├─────────────────────────────────────┤
      │                                     │
      │  Program RAM (~30KB)                │  User programs and data
      │  $0800-$7FFF                        │
      │                                     │
$8000 ├─────────────────────────────────────┤
      │  I/O Space (8KB, 8 slots)           │  Memory-mapped peripherals
      │  $8000-$9FFF                        │  Each slot = 1KB (1024 bytes)
      │                                     │
      │  IO 1 ($8000-$83FF): RAM Card       │
      │  IO 2 ($8400-$87FF): RAM Card       │
      │  IO 3 ($8800-$8BFF): RTC Card       │
      │  IO 4 ($8C00-$8FFF): Storage Card   │
      │  IO 5 ($9000-$93FF): Serial Card    │
      │  IO 6 ($9400-$97FF): GPIO Card      │
      │  IO 7 ($9800-$9BFF): Sound Card     │
      │  IO 8 ($9C00-$9FFF): Video Card     │
$A000 ├─────────────────────────────────────┤
      │  LO System ROM (8KB)                │  KERNAL routines (built-in)
      │  $A000-$BFFF                        │  System calls, I/O routines
$C000 ├─────────────────────────────────────┤
      │  HI System ROM or Cart ROM (16KB)   │  BASIC/Monitor or Cartridge
      │  $C000-$FFFF                        │  Can be replaced by ROM Cart
$FFFA ├─────────────────────────────────────┤
      │  Interrupt Vectors (6 bytes)        │
      │  $FFFA-$FFFB: NMI vector            │
      │  $FFFC-$FFFD: RESET vector          │
      │  $FFFE-$FFFF: IRQ/BRK vector        │
$FFFF └─────────────────────────────────────┘
```

### Memory Device Specifications

**RAM (AS6C62256 or equivalent):**
- 32KB static RAM (SRAM)
- Access time: 55-70ns typical
- 5V operation
- Mapped to $0000-$7FFF (lower 32KB)
- Battery backup capable (for data retention)

**ROM (Built-in System ROM):**
- Total: 24KB built-in ROM
- **LO ROM ($A000-$BFFF, 8KB):** KERNAL routines
  - System calls and I/O routines
  - Always present (not replaceable)
- **HI ROM ($C000-$FFFF, 16KB):** BASIC/Monitor
  - BASIC interpreter or Monitor program
  - Can be replaced by ROM Cart (cartridge slots)

**ROM Cart (Optional Cartridge):**
- 16KB maximum
- Replaces HI ROM ($C000-$FFFF)
- Allows game cartridges, custom software
- Switchable between built-in ROM and cart

**Banked Memory (Advanced Configurations):**
- Some systems support bank switching for >64KB
- Uses additional control registers
- Allows switching ROM or RAM banks in/out of address space
- See system-specific documentation

---

## Bus Architecture

### Address Bus (A0-A15)

**16-bit unidirectional bus:**
- Driven by CPU during normal operation
- Addresses 64KB memory space ($0000-$FFFF)
- Valid during entire PHI2 high period
- Can be tri-stated for DMA (not implemented in basic systems)

**Timing:**
- Valid by PHI2 rising edge
- Stable throughout PHI2 high
- May change during PHI2 low (not guaranteed stable)

### Data Bus (D0-D7)

**8-bit bidirectional bus:**
- **Read cycle**: Driven by memory/peripheral → read by CPU
- **Write cycle**: Driven by CPU → written to memory/peripheral
- Tri-state capable (high-impedance when not driven)

**Timing:**
- **Read**: Data must be valid before PHI2 falling edge
- **Write**: Data valid from early PHI2 high until early PHI2 low

### Control Signals

**RW (Read/Write):**
- Direction: CPU output
- **High (1)**: Read cycle (CPU reads from bus)
- **Low (0)**: Write cycle (CPU writes to bus)
- Valid entire PHI2 high period
- Used by peripherals to determine direction

**PHI2 (Phase 2 Clock):**
- System clock output from CPU or clock generator
- All bus activity synchronized to PHI2
- **Rising edge**: Address becomes valid
- **High period**: Data transfer occurs
- **Falling edge**: Data latched (reads) or written (writes)

**RESET:**
- Active-low system reset
- When low: CPU halts, enters reset state
- **Reset sequence**:
  1. Pull RESET low (min. 2 cycles at power-on, typically 100ms+)
  2. Release RESET (goes high)
  3. CPU reads reset vector from $FFFC-$FFFD
  4. PC loaded with vector value
  5. Execution begins at reset address

**IRQ (Interrupt Request):**
- Active-low maskable interrupt
- When low (and I flag clear): CPU completes current instruction, then:
  1. Push PC high byte to stack
  2. Push PC low byte to stack
  3. Push P (status register) to stack
  4. Set I flag (disable further IRQs)
  5. Load PC from IRQ vector ($FFFE-$FFFF)
  6. Jump to interrupt handler
- Handler ends with `RTI` (restores P and PC)

**NMI (Non-Maskable Interrupt):**
- Active-low, edge-triggered, non-maskable
- Cannot be disabled by I flag
- Used for critical events (power failure, hardware errors)
- **Trigger**: High-to-low transition (edge sensitive)
- Similar sequence to IRQ but uses NMI vector ($FFFA-$FFFB)

**RDY (Ready):**
- Active-low input to CPU
- When low: Stretches clock (CPU waits)
- Used for:
  - Slow memory access (wait states)
  - DMA operations
  - Single-stepping (debugging)

**SYNC (Sync Output):**
- High during opcode fetch (beginning of instruction)
- Used for debugging (breakpoint detection)
- Not used in standard operation

**BE (Bus Enable - 65C02S only):**
- Active-high input
- When low: CPU tri-states address and data buses
- Allows DMA controller or other master to access bus

---

## Address Decoding

### Basic Decoding Strategy

**Objective:** Enable correct device based on address bus value.

**Two-level decoding:**
1. **Coarse decode**: Upper address bits (A15-A13 typically) select region (RAM, ROM, I/O)
2. **Fine decode**: Lower address bits (A12-A0) select specific register within device

### RAM Decode ($0000-$7FFF)

**Logic:**
```
RAM_CS = NOT A15
```

- When A15 = 0 (addresses $0000-$7FFF): RAM selected
- Simplest possible decode (single inverter)

### ROM Decode ($8000-$FFFF)

**Logic:**
```
ROM_CE = A15
```

- When A15 = 1 (addresses $8000-$FFFF): ROM selected
- Direct connection (A15 → ROM chip enable)

### I/O Decode ($8000-$9FFF)

**8 I/O slots of 1KB each ($8000-$9FFF):**

**Coarse decode** (is address in $8000-$9FFF I/O region?):
```
IO_REGION = A15 AND (NOT A14) AND (NOT A13)
```
This is true when A15=1, A14=0, A13=0, giving addresses $8000-$9FFF.

**Using 74LS138 3-to-8 decoder for memory regions:**
- Inputs: A15, A14, A13
- Enable inputs: PHI2 (ensures only during valid cycle)
- Outputs: 8 regions of 8KB each
  - Output 0: $0000-$1FFF (RAM)
  - Output 1: $2000-$3FFF (RAM)
  - Output 2: $4000-$5FFF (RAM)
  - Output 3: $6000-$7FFF (RAM)
  - Output 4: $8000-$9FFF (I/O space) ← Enable I/O decode
  - Output 5: $A000-$BFFF (LO ROM - KERNAL)
  - Output 6: $C000-$DFFF (HI ROM upper half)
  - Output 7: $E000-$FFFF (HI ROM upper half)

**Fine decode** (which I/O slot in $8000-$9FFF?):
- Use A12, A11, A10 to select 1 of 8 I/O slots (1KB each)
- Example: 74LS138 with inputs A12, A11, A10
  - Output 0: $8000-$83FF (IO 1 - RAM Card)
  - Output 1: $8400-$87FF (IO 2 - RAM Card)
  - Output 2: $8800-$8BFF (IO 3 - RTC Card)
  - Output 3: $8C00-$8FFF (IO 4 - Storage Card)
  - Output 4: $9000-$93FF (IO 5 - Serial Card - ACIA)
  - Output 5: $9400-$97FF (IO 6 - GPIO Card - VIA)
  - Output 6: $9800-$9BFF (IO 7 - Sound Card)
  - Output 7: $9C00-$9FFF (IO 8 - Video Card)

### Backplane Address Decoding

**Card-based systems** use address decode logic on each card:
- All cards receive full address bus
- Each card decodes its own assigned address range
- Only one card responds to any given address (no conflicts!)

**I/O Card Implementation:**
Each I/O card uses a 74HC138 3-to-8 decoder:
- Inputs: A15, A14, A13, A12, A11, A10
- Jumper-selectable configuration determines which I/O slot ($8000-$9FFF range)
- Enable inputs qualified with PHI2 for valid bus cycle timing

**Example (Serial Card at $9000-$93FF):**
```
Serial_CS = A15 AND (NOT A14) AND (NOT A13) AND A12 AND (NOT A11) AND (NOT A10) AND PHI2
```

Implemented with 74HC138 decoder and standard 74-series logic gates. Jumpers allow reconfiguration to different I/O slots.

---

## Clock and Reset

### Clock Generation

**Crystal Oscillator:**
- Typical frequencies: 1 MHz, 1.8432 MHz, 2 MHz, 4 MHz, 8 MHz
- 1.8432 MHz popular for serial compatibility (divides evenly to standard baud rates)

**Clock Distribution:**
- PHI2 clock distributed to:
  - CPU PHI2 input
  - All peripherals (for synchronization)
  - Address decoding logic (qualify chip selects)

**Clock Circuit:**
```
Crystal → Oscillator → Divider (optional) → PHI2
                     └─→ UART clock (for serial)
```

**Typical implementations:**
1. **Discrete oscillator can:** Pre-packaged oscillator module (4 pins: VCC, GND, OUT, EN)
2. **Crystal + gates:** Pierce oscillator using 74HC04 inverters
3. **Microcontroller-generated**: For DEV system (Teensy generates clock for emulated CPU)

### Reset Circuit

**Power-On Reset (POR):**
```
VCC ──┬─── R ────┬──── RESET (to CPU and peripherals)
      │          │
      │         CAP
      │          │
     GND        GND
```

**Operation:**
1. Power applied: Capacitor discharged, RESET pulled low
2. Capacitor charges through resistor
3. When voltage reaches threshold (~3.5V), RESET goes high
4. Typical values: R=10K, C=10µF → ~100ms delay

**Manual Reset:**
- Push-button switch in parallel with capacitor
- Pressing button: Discharges cap, pulls RESET low
- Releasing button: Reset sequence repeats

**Reset Supervisor IC (better approach):**
- DS1233 or similar
- Monitors VCC
- Holds RESET low until VCC stable and valid
- Debounces manual reset button
- More reliable than RC circuit

---

## Interrupt Handling

### IRQ (Maskable Interrupt)

**Sources:**
- VIA timer interrupts
- Serial port (character received/transmitted)
- Keyboard (key pressed)
- Custom peripherals

**Handling:**
1. Peripheral asserts IRQ (pulls low)
2. CPU detects IRQ at end of instruction (if I flag clear)
3. CPU pushes PC and P to stack
4. CPU sets I flag (prevents nested IRQs)
5. CPU loads PC from $FFFE-$FFFF
6. ISR (Interrupt Service Routine) executes
7. ISR determines source (polls peripherals or uses vector table)
8. ISR handles interrupt
9. ISR ends with `RTI` (restore registers, clear I flag, return)

**Multiple IRQ sources:**

**Polling approach:**
```assembly
IRQ_Handler:
    PHA         ; Save A
    LDA VIA_IFR ; Check VIA interrupt flag register
    BNE Handle_VIA
    LDA ACIA_STATUS ; Check ACIA
    BNE Handle_ACIA
    ; ... check other sources
    PLA
    RTI

Handle_VIA:
    ; Service VIA interrupt
    ; Clear interrupt flag
    PLA
    RTI
```

**Priority encoding approach:**
- Hardware priority encoder (74LS148)
- Generates vector offset based on highest priority interrupt
- Jump table in ROM

### NMI (Non-Maskable Interrupt)

**Sources:**
- Power failure detection (brown-out detector)
- Hardware watchdog timer
- Panic button
- Reserved for critical, non-recoverable events

**Handling:**
- Similar to IRQ but uses $FFFA-$FFFB vector
- Cannot be disabled (always active)
- Edge-triggered (avoid repeated triggers)

**Example NMI handler:**
```assembly
NMI_Handler:
    ; Save critical data to battery-backed RAM
    ; Disable peripherals
    ; Halt or wait for power restoration
    RTI  ; Or infinite loop
```

---

## I/O Architecture

### Memory-Mapped I/O

**Philosophy:** Peripherals appear as memory locations.
- No special I/O instructions (use LDA, STA, etc.)
- Peripheral registers mapped to address space
- Accessing address reads/writes peripheral register

### VIA (6522 Versatile Interface Adapter)

**Typical address: Depends on card (GPIO Card typically at IO 6: $9400-$97FF)**
**Register offset: $9400-$940F (16 registers within GPIO card space)**

**Registers (example for GPIO Card at $9400):**
- **$9400**: ORB (Output Register B) / IRB (Input Register B)
- **$9401**: ORA (Output Register A) / IRA (Input Register A)
- **$9402**: DDRB (Data Direction Register B)
- **$9403**: DDRA (Data Direction Register A)
- **$9404**: T1C-L (Timer 1 Counter Low)
- **$9405**: T1C-H (Timer 1 Counter High)
- **$9406**: T1L-L (Timer 1 Latch Low)
- **$9407**: T1L-H (Timer 1 Latch High)
- **$9408**: T2C-L (Timer 2 Counter Low)
- **$9409**: T2C-H (Timer 2 Counter High)
- **$940A**: SR (Shift Register)
- **$940B**: ACR (Auxiliary Control Register)
- **$940C**: PCR (Peripheral Control Register)
- **$940D**: IFR (Interrupt Flag Register)
- **$940E**: IER (Interrupt Enable Register)
- **$940F**: ORA/IRA (no handshake)

**Usage example (set PA0 high):**
```assembly
LDA #$01     ; Bit 0 = 1
STA $9403    ; Set DDRA bit 0 to output
STA $9401    ; Set ORA bit 0 to high
```

### ACIA (6551 Asynchronous Communications Interface Adapter)

**Typical address: Serial Card at IO 5: $9000-$93FF**
**Register offset: $9000-$9003 (4 registers within serial card space)**

**Registers:**
- **$9000**: Data register (read = receive, write = transmit)
- **$9001**: Status register (read-only)
- **$9002**: Command register (write-only)
- **$9003**: Control register (write-only)

**Initialization:**
```assembly
LDA #$1F     ; 8-N-1, 19200 baud (example)
STA $9003    ; Write to control register
LDA #$0B     ; Set command: no parity, enable RX/TX
STA $9002
```

**Transmit character:**
```assembly
Wait_TX:
    LDA $9001     ; Read status
    AND #$10      ; Check TX buffer empty flag
    BEQ Wait_TX   ; Wait until ready
    LDA #'A'      ; Character to send
    STA $9000     ; Write to data register
```

### Video Card Registers

**Video Card at IO 8: $9C00-$9FFF (1KB)**
**Typical register map (varies by specific video card):**
- **$9C00**: Command/control register
- **$9C01**: Cursor position X
- **$9C02**: Cursor position Y
- **$9C03**: Character/color data
- **$9C10-$9FFF**: Video RAM (text mode or register space)

---

## Timing and Bus Cycles

### Read Cycle Timing

```
PHI2   ──┐  ┌──┐  ┌──
         └──┘  └──┘
         
A0-15  ────<ADDR>────────
         \_________/
         
RW     ────────────────── (HIGH)

D0-7   ────────<DATA>────
              \_____/
              (Memory or peripheral drives bus)
         
         tADS    tDSU
         |───|   |─|
         
tADS: Address setup time (address valid before PHI2 rise)
tDSU: Data setup time (data valid before PHI2 fall)
tAH:  Address hold time (address stable after PHI2 fall)
```

**Cycle steps:**
1. **PHI2 low**: CPU prepares address
2. **PHI2 rising edge**: Address valid, RW set high
3. **PHI2 high**: Memory/peripheral drives data onto bus
4. **PHI2 falling edge**: CPU latches data
5. **PHI2 low**: Prepare for next cycle

**Critical timing:**
- Memory access time must be < PHI2 high period
- Typical: 500ns @ 1 MHz, 250ns @ 2 MHz, 125ns @ 4 MHz
- SRAM typically 55-70ns → easily meets timing up to 10+ MHz
- ROM typically 150ns → may need wait states >4 MHz

### Write Cycle Timing

```
PHI2   ──┐  ┌──┐  ┌──
         └──┘  └──┘
         
A0-15  ────<ADDR>────────
         \_________/
         
RW     ──────┐  ┌─────── (LOW during write)
              └──┘
         
D0-7   ────<DATA>────────
           \________/
           (CPU drives bus)
         
         tADS tDSW tDHW
         |───||──||─|
         
tDSW: Data setup time for write (data valid before WE)
tDHW: Data hold time for write (data stable after WE)
```

**Cycle steps:**
1. **PHI2 low**: CPU prepares address and data
2. **PHI2 rising edge**: Address and data valid, RW set low
3. **PHI2 high**: Memory/peripheral latches data
4. **PHI2 falling edge**: Write complete
5. **PHI2 low**: CPU releases bus

### Wait States

**When needed:**
- Slow peripheral or memory
- Complex address decoding (propagates delay)

**Implementation:**
- Pull RDY low before PHI2 falling edge
- CPU extends PHI2 high period (stretches clock)
- Peripheral drives data (or latches write)
- Peripheral releases RDY (pull high)
- CPU continues

**Rarely needed in modern systems** (fast SRAM/ROM).

---

## System Variants

### The COB (Computer On Backplane)

**Bus architecture:**
- Parallel backplane with card edge connectors
- All signals bussed to slots (A0-A15, D0-D7, control)
- Each card decodes own address range

**Advantages:**
- Modular expansion
- Easy to add/remove peripherals
- Educational (shows bus clearly)

**Challenges:**
- Bus loading (capacitance limits speed)
- Address conflicts (must carefully assign addresses)

### The DEV (Development System)

**Emulation architecture:**
- Teensy 4.1 emulates entire 6502 system
- vrEmu6502 library (cycle-accurate emulator)
- GPIO pins emulate bus signals (for peripherals)

**Advantages:**
- No need for vintage chips
- Easy firmware updates (flash new ROM via USB)
- Built-in debugging (serial monitor, breakpoints)

**Architecture differences:**
- Software memory (not physical RAM/ROM chips)
- Emulated bus (not physical signal lines)
- Same instruction set and behavior as real 6502

### The KIM (Single-Board Computer)

**Integrated architecture:**
- All components on one PCB
- ATmega1284p handles keyboard/LCD
- W65C02S CPU, RAM, ROM on same board

**Advantages:**
- Simpler assembly (no backplane/cards)
- Lower cost
- Compact footprint

### The VCS (Compact Variant)

**Simplified architecture:**
- Minimal component count
- On-board peripherals only
- No expansion bus

---

**For more technical details, see:**
- [W65C02S Datasheet](https://www.westerndesigncenter.com/wdc/documentation/w65c02s.pdf)
- [6522 VIA Datasheet](http://archive.6502.org/datasheets/mos_6522_preliminary_nov_1977.pdf)
- [Main README](../../README.md) - Project overview

---

**Last Updated:** March 7, 2026
