6502 Hardware
=============

This folder contains KiCad hardware designs for all PCBs in the 6502 project. Each design includes complete schematics, PCB layouts, and component libraries.

**Last Updated:** March 7, 2026

---

## Table of Contents

- [Overview](#overview)
- [Folder Structure](#folder-structure)
- [Hardware Categories](#hardware-categories)
- [Design Standards](#design-standards)
- [Boards Detailed Reference](#boards-detailed-reference)
- [Cards Detailed Reference](#cards-detailed-reference)
- [Helpers Detailed Reference](#helpers-detailed-reference)
- [Carts Detailed Reference](#carts-detailed-reference)
- [Assembly Notes](#assembly-notes)
- [Revision History](#revision-history)

---

## Overview

The 6502 project hardware is organized into four main categories:

1. **Boards**: Core system boards (mainboards, backplanes, development boards)
2. **Cards**: Peripheral expansion cards that plug into backplane slots
3. **Helpers**: Interface boards that provide specific functionality or connectors
4. **Carts**: Cartridge boards for ROM images

All designs are created using **KiCad 7.0+** and optimized for manufacturing by **JLCPCB**. Designs use a mix of surface-mount (SMD) and through-hole (DIP/TH) components for optimal balance of manufacturability and hobbyist assembly.

---

## Folder Structure

```
Hardware/
├── <BoardName>/
│   ├── Rev 1.0/
│   │   ├── <BoardName>.kicad_pro    # KiCad project file
│   │   ├── <BoardName>.kicad_sch    # Schematic file
│   │   ├── <BoardName>.kicad_pcb    # PCB layout file
│   │   ├── fp-lib-table              # Footprint library table
│   │   └── sym-lib-table             # Symbol library table
│   └── Rev 1.1/ (if updated)
└── ...
```

Each board folder contains one or more revision subfolders. Always use the latest revision unless specifically maintaining compatibility with older versions.

---

## Hardware Categories

### Boards (13 total)

| Name | Function | System | Status |
|------|----------|--------|--------|
| Backplane | 3-slot passive backplane | COB | ✓ Tested |
| Backplane Pro | 3-slot active backplane with clock/reset/power | COB | ✓ Tested |
| Backplane Helper | +2 expansion slots | COB, KIM | ✓ Tested |
| Dev Board | Teensy 4.1 65C02 emulator | DEV | ✓ Tested |
| Dev Output Board | 2.4" LCD VT-AC terminal | DEV | ✓ Tested |
| Input Board | PS/2 keyboard/mouse + SNES controllers | VCS | ✓ Tested |
| LCD Board | 320×240 LCD via 6522 VIA | COB | ⚠️ UNTESTED |
| Main Board | 65C02 + 32KB RAM + 32KB ROM | KIM, VCS | ✓ Tested |
| Output Board | VGA (Pico9918) + audio (ARMSID) | VCS | ⚠️ UNTESTED |

### Cards (17 total)

| Name | Function | Interface | Status |
|------|----------|-----------|--------|
| Blinkenlights Card | LED bus visualizer | Visual | ✓ Tested |
| CPU Card | 65C02 CPU host | CPU | ✓ Tested |
| CPU Card Pro | 65816 CPU host | CPU | ⚠️ UNTESTED |
| GPIO Card | 6522 VIA I/O | IO 6 ($9400-$97FF) | ✓ Tested |
| LCD Card | 16×2 LCD via VIA | IO 6 ($9400-$97FF) | ✓ Tested |
| Memory Card | 32KB RAM + 32KB ROM | $0000-$FFFF | ✓ Tested |
| Prototype Card | Blank prototyping area | Custom | ✓ Tested |
| RAM Card | 512KB banked RAM | IO 1/2 ($8000-$87FF) | ✓ Tested |
| RTC Card | Real-time clock | IO 3 ($8800-$8BFF) | ✓ Tested |
| Serial Card | 6551 ACIA RS-232 | IO 5 ($9000-$93FF) | ✓ Tested |
| Serial Card Pro | Enhanced 6551 serial | IO 5 ($9000-$93FF) | ✓ Tested |
| Sound Card | ARMSID audio | IO 7 ($9800-$9BFF) | ✓ Tested |
| Storage Card | CompactFlash IDE | IO 4 ($8C00-$8FFF) | ✓ Tested |
| Storage Card Pro | SD + Flash + SPI | IO 4 ($8C00-$8FFF) | ⚠️ UNTESTED |
| VGA Card | VGA via Pico9918 | IO 8 ($9C00-$9FFF) | ✓ Tested |
| VGA Card Pro | Programmable VGA | IO 8 ($9C00-$9FFF) | ⚠️ UNTESTED |
| Video Card | TMS9918A composite | IO 8 ($9C00-$9FFF) | ✓ Tested |
| Video Card Pro | AVR-based composite | IO 8 ($9C00-$9FFF) | ⚠️ UNTESTED |

### Helpers (14 total)

| Name | Function | Connection | Status |
|------|----------|------------|--------|
| Backplane Helper | +2 card slots | Backplane | ✓ Tested |
| Breadboard Helper | Bus to breadboard | Ribbon cable | ✓ Tested |
| Clock Helper | Manual clock control | Clock input | ✓ Tested |
| DB25 Helper | GPIO to DB25 | GPIO Card | ✓ Tested |
| GPIO Breadboard Helper | GPIO to breadboard | GPIO Card | ✓ Tested |
| GPIO Helper | 8 buttons + 8 LEDs | GPIO Card | ✓ Tested |
| Joystick Helper | Atari joystick port | GPIO Card | ✓ Tested |
| Keyboard Encoder Helper | Keyboard + PS/2 to ASCII | GPIO Card | ✓ Tested |
| Keyboard Helper | 64-key matrix + joysticks | GPIO Card | ✓ Tested |
| Keypad Helper | Hex keypad 4×4 + 2×4 | GPIO Card | ✓ Tested |
| Keypad LCD Helper | LCD for keypad | GPIO Card | ✓ Tested |
| Mega Helper | Arduino Mega interface | Bus | ✓ Tested |
| PS2 Helper | PS/2 to matrix | GPIO Card | ✓ Tested |
| VERA Helper | VERA module adapter | Bus | Experimental |

### Carts (1 total)

| Name | Function | Capacity | Status |
|------|----------|----------|--------|
| ROM Cart | Swappable ROM cartridge | 16KB | ✓ Tested |

---

## Design Standards

All hardware designs follow these standards for consistency and manufacturability:

### PCB Specifications

**Layers:**
- Simple boards: 2-layer
- Complex boards with power planes: 4-layer
- Edge connectors and high-speed signals: typically 4-layer

**Dimensions:**
- Card width: 100mm (standard card edge connector width)
- Card height: Varies (60-100mm typical)
- Board thickness: 1.6mm standard

**Trace Width:**
- Signal traces: 0.254mm (10 mil) minimum, 0.381mm (15 mil) typical
- Power traces: 0.508mm (20 mil) minimum, wider for high current
- Clearance: 0.254mm (10 mil) minimum

**Vias:**
- Standard: 0.6mm drill, 1.0mm pad
- Power: 0.8mm drill, 1.2mm pad
- Minimum: 0.3mm drill, 0.6mm pad

**JLCPCB Compatibility:**
- All designs pass JLCPCB design rules (see Production folder)
- Component selection optimized for JLCPCB assembly catalog
- Alternative footprints provided where JLCPCB parts unavailable

### Component Selection

**IC Packages:**
- Through-hole (DIP): For CPUs, RAM, ROM, socketed ICs
- SMD (SOIC, TSSOP, QFN): For modern ICs, microcontrollers, buffers

**Passive Components:**
- SMD: 0805 or 1206 packages (hand-solderable)
- Through-hole: 1/4W resistors, radial capacitors (when space allows)

**Connectors:**
- Through-hole: For all connectors (mechanical stress consideration)
- IDC headers: 2.54mm pitch for ribbon cables
- Card edge: Gold-plated fingers for backplane cards

### Schematic Conventions

**Net Naming:**
- Address bus: A0-A15
- Data bus: D0-D7
- Control signals: PHI2, RW, RESET, IRQ, NMI, etc.
- Power: +5V or VCC, GND
- Chip enables: CE, CS, CSB (active low)

**Component References:**
- U: Integrated circuits
- R: Resistors
- C: Capacitors
- D: Diodes
- Q: Transistors
- J: Connectors/headers
- SW: Switches
- LED: LEDs
- Y: Crystals/oscillators

**Component Values:**
- Resistors: Ohms (100R, 1K, 10K)
- Capacitors: Farads (100nF, 10μF, 100μF)
- Inductors: Henries (10μH, 100μH)

---

## Boards Detailed Reference

### Backplane

**PCB:** 4-layer, 160mm × 100mm  
**Power Distribution:** 5V input, distributed to 5 card slots  
**Bus Buffering:** 74LS245 bidirectional buffers  
**Card Slots:** 5× card edge connectors (2.54mm pitch)  

**Key Signals per Slot:**
- Address bus: A0-A15 (buffered)
- Data bus: D0-D7 (buffered bidirectional)
- Control: PHI2, RW, RESET, IRQ, NMI
- Power: +5V, GND

**Jumpers:** None  
**Power Consumption:** ~100mA + cards  
**Assembly:** SMD assembly by JLCPCB, through-hole connectors manual

---

### Backplane Pro

**PCB:** 4-layer, 160mm × 100mm  
**Power Distribution:** VRM (voltage regulator module) provides 5V to all slots  
**Clock Generation:** Onboard oscillator circuit (1-8 MHz user-selectable via crystal)  
**Reset Circuit:** Power-on reset + manual reset button  
**Bus Buffering:** 74LS245 bidirectional buffers + 74LS244 unidirectional buffers  
**Card Slots:** 5× card edge connectors  

**Oscillator Options:**
- 1 MHz crystal
- 2 MHz crystal
- 4 MHz crystal
- 8 MHz crystal (fast 65C02 only)

**Jumpers:**
- JP1: Clock source select (onboard vs. external)
- JP2: Reset source (auto vs. manual only)

**Power Input:** 5-12V DC (regulated to 5V onboard)  
**Power Consumption:** ~200mA + cards  
**Assembly:** Full SMD assembly available from JLCPCB

---

### Dev Board

**PCB:** 4-layer, 100mm × 80mm  
**Microcontroller:** Teensy 4.1 (ARM Cortex-M7 @ 600 MHz)  
**Emulation:** vrEmu6502 library (cycle-accurate 65C02)  
**Memory:** Up to 32KB base RAM + 32KB banked RAM (default), expandable to 512KB banked RAM with PSRAM  
**Storage:** MicroSD card slot  
**Networking:** Ethernet PHY (10/100 Mbps)  
**USB:** Host mode (keyboard, joystick input)  

**Connectors:**
- J1: Bus output (26-pin IDC) to Dev Output Board or peripherals
- J2: Ethernet RJ45
- J3: USB host (keyboard/joystick)
- J4: MicroSD card slot
- J5: Power input (USB or barrel jack)
- J6: Serial UART (for debugging)

**Buttons:**
- SW1: Run/Stop (toggle emulation)
- SW2: Step (single instruction or cycle)
- SW3: Reset (trigger CPU reset)
- SW4: Frequency (cycle through clock speeds)

**LEDs:**
- D1: Power
- D2: CPU running
- D3: SD card activity
- D4: Network activity

**Firmware:** [DB Emulator](../Firmware/DB%20Emulator/)  
**Power Consumption:** ~500mA typical, 1A maximum

---

### Dev Output Board

**PCB:** 2-layer, 60mm × 80mm  
**Display:** ILI9341 2.4" TFT LCD (320×240)  
**Protocol:** VT-AC ASCII terminal emulation  
**Connection:** SPI to Dev Board  

**Connectors:**
- J1: SPI + control (10-pin IDC) from Dev Board
- J2: Power (5V)

**Firmware:** [DOB Terminal](../Firmware/DOB%20Terminal/)  
**Power Consumption:** ~150mA

---

### Main Board

**PCB:** 4-layer, 100mm × 160mm  
**CPU:** W65C02S (DIP-40 socket)  
**RAM:** 32KB SRAM (62256 or compatible, DIP-28 socket)  
**ROM:** 32KB EEPROM (28C256 or compatible, DIP-28 socket)  
**Clock:** Crystal oscillator circuit (1-8 MHz selectable)  
**Reset:** RC reset circuit with manual reset button  

**Memory Map:**
- $0000-$7FFF: RAM (32KB)
- $8000-$FFFF: ROM (32KB)

**Connectors:**
- J1: Bus expansion (40-pin IDC)
- J2: Power input (barrel jack or screw terminal)
- J3: Serial output (optional header)

**Jumpers:**
- JP1: Clock source (onboard crystal vs. external)
- JP2: ROM programming enable/disable

**LEDs:**
- D1: Power
- D2: Activity (PHI2 clock indicator)

**Power Consumption:** ~200mA @ 5V

---

### Input Board

**PCB:** 2-layer, 100mm × 80mm  
**Controllers:**
- ATTiny85 #1: PS/2 keyboard controller
- ATTiny85 #2: PS/2 mouse controller

**Connectors:**
- J1: PS/2 keyboard (6-pin Mini-DIN)
- J2: PS/2 mouse (6-pin Mini-DIN)
- J3, J4: SNES controller ports (7-pin)
- J5: Bus interface (20-pin IDC)

**Firmware:**
- [IB Keyboard Controller](../Firmware/IB%20Keyboard%20Controller/)
- [IB Mouse Controller](../Firmware/IB%20Mouse%20Controller/)

**I/O Registers** (memory-mapped, assuming IO 6 at $9400):
- $9408: Keyboard data
- $9409: Keyboard status
- $940A: Mouse X coordinate
- $940B: Mouse Y coordinate
- $940C: Mouse buttons
- $940D: SNES controller 1
- $940E: SNES controller 2

**Power Consumption:** ~100mA

---

## Cards Detailed Reference

### GPIO Card

**PCB:** 2-layer, 100mm × 60mm  
**Chip:** 6522 VIA (Versatile Interface Adapter)  
**I/O:** 20 GPIO pins (Port A: 8 pins, Port B: 8 pins, CA1/CA2/CB1/CB2: 4 pins)  
**Timers:** 2× 16-bit timers  
**Shift Register:** 8-bit bidirectional  

**Default Address Range:** IO 6 ($9400-$97FF), configurable via jumpers

**Register Map:**
- $9400: ORB (Output Register B)
- $9401: ORA (Output Register A)  
- $9402: DDRB (Data Direction Register B)
- $9403: DDRA (Data Direction Register A)
- $9404: T1CL (Timer 1 Counter Low)
- $9405: T1CH (Timer 1 Counter High)
- $9406: T1LL (Timer 1 Latch Low)
- $9407: T1LH (Timer 1 Latch High)
- $9408: T2CL (Timer 2 Counter Low)
- $9409: T2CH (Timer 2 Counter High)
- $940A: SR (Shift Register)
- $940B: ACR (Auxiliary Control Register)
- $940C: PCR (Peripheral Control Register)
- $940D: IFR (Interrupt Flag Register)
- $940E: IER (Interrupt Enable Register)
- $C00F: ORA_NH (Output Register A, no handshake)

**Jumpers:**
- JP1-JP4: Address select A4-A7 (sets base address)
- JP5: IRQ enable/disable

**Connectors:**
- J1: Port A (10-pin header)
- J2: Port B (10-pin header)
- J3: Control signals (CA1, CA2, CB1, CB2)
- J4: Bus connection (card edge connector)

**Power Consumption:** ~50mA

**Compatible Helpers:**
- Keyboard Encoder Helper
- PS2 Helper
- Keyboard Helper
- Keypad Helper
- GPIO Helper
- Joystick Helper
- DB25 Helper
- GPIO Breadboard Helper

---

### Memory Card

**PCB:** 2-layer, 100mm × 60mm  
**RAM:** 32KB SRAM (62256, DIP-28 socket)  
**ROM:** 32KB EEPROM (28C256, DIP-28 socket)  
**Address Decoder:** 74LS00 + 74LS138 logic  

**Memory Map:**
- $0000-$7FFF: RAM (32KB, controlled by A15=0)
- $8000-$FFFF: ROM (32KB, controlled by A15=1)

**Address Decode Logic:**
- RAM enabled when A15=0
- ROM enabled when A15=1
- Simple and reliable, no jumpers needed

**Sockets:** IC sockets recommended for RAM and ROM (easy replacement/programming)

**Power Consumption:** ~80mA

---

### RAM Card

**PCB:** 4-layer, 100mm × 60mm  
**Capacity:** 512KB SRAM organized as two 256KB banks  
**Banking Logic:** Single latch controls both bank windows  
**Bank Windows:** IO 1 ($8000-$83FF) and IO 2 ($8400-$87FF)  

**Bank Select Latch** ($83FF or $87FF):
- Write bank number (0-255) to select which 1KB window appears at both IO slots
- Same bank number applies to both windows, accessing different 256KB regions
- IO 1 window: Shows bank[n] from first 256KB (bytes n×1024 to n×1024+1023)
- IO 2 window: Shows bank[n] from second 256KB (bytes 262144+n×1024 to 262144+n×1024+1023)
- Example: Writing 5 to latch:
  - IO 1 ($8000-$83FF) = bytes 5120-6143 (bank 5 of first 256KB)
  - IO 2 ($8400-$87FF) = bytes 267264-268287 (bank 5 of second 256KB)

**Jumpers:**
- JP1-JP4: Address select for bank register

**Use Cases:**
- Large program storage (load different code segments as needed)
- Ramdisk
- Frame buffers
- Data storage beyond 64KB addressing limit

**Power Consumption:** ~150mA

---

### Serial Card

**PCB:** 2-layer, 100mm × 60mm  
**Chip:** 6551 ACIA (Asynchronous Communications Interface Adapter)  
**Level Shifter:** MAX232 (TTL to RS-232)  
**Connector:** DB9 female (DTE configuration)  

**Default Address Range:** IO 5 ($9000-$93FF)

**Register Map:**
- $9000: Data register (read RX, write TX)
- $9001: Status register (read only)
- $9002: Command register (write only)
- $9003: Control register (write only)

**Supported Baud Rates:** 50, 75, 110, 135, 150, 300, 600, 1200, 1800, 2400, 3600, 4800, 7200, 9600, 19200 bps

**Jumpers:**
- JP1-JP4: Address select
- JP5: IRQ enable/disable
- JP6: Clock source (onboard vs. external)

**Onboard Crystal:** 1.8432 MHz (standard for accurate baud rate generation)

**Power Consumption:** ~100mA

---

### VGA Card

**PCB:** 2-layer, 100mm × 60mm  
**Graphics Chip:** Raspberry Pi Pico running [Pico9918](https://github.com/visrealm/pico9918) firmware  
**Video Output:** VGA (640×480 @ 60Hz)  
**Graphics Modes:** TMS9918A-compatible modes  
**Colors:** 16-color palette  
**Sprites:** 32 sprites support  

**Default Address Range:** IO 8 ($9C00-$9FFF)

**Register Map** (TMS9918A-compatible):
- $9C00: Data port (VRAM read/write)
- $9C01: Control/status port

**Connectors:**
- J1: VGA output (15-pin HD15)
- J2: Bus connection (card edge)

**Programming:** Flash Pico9918 firmware to Pi Pico before installation

**Power Consumption:** ~150mA

---

### Video Card

**PCB:** 2-layer, 100mm × 60mm  
**Graphics Chip:** TMS9918A VDP (Video Display Processor)  
**VRAM:** 16KB (separate from main system RAM)  
**Video Output:** Composite video (RCA jack)  
**Resolution:** 256×192 pixels (various graphics modes)  
**Colors:** 16-color palette  
**Sprites:** 32 sprites (8×8 or 8×16)  
**Crystal:** 10.738635 MHz (NTSC) or 10.7386 MHz (PAL)  

**Default Address Range:** IO 8 ($9C00-$9FFF)

**Register Map:**
- $9C00: Data port (VRAM read/write)
- $9C01: Control/status port

**Jumpers:**
- JP1-JP4: Address select
- JP5: NTSC vs. PAL crystal selection

**Power Consumption:** ~200mA

---

## Helpers Detailed Reference

### Keyboard Encoder Helper

**PCB:** 2-layer, 80mm × 60mm  
**Microcontroller:** ATmega1284p  
**Inputs:** PS/2 keyboard OR 8×8 keyboard matrix (Keyboard Helper)  
**Output:** ASCII data via 6522 VIA ports  

**Connections:**
- J1: PS/2 keyboard (6-pin Mini-DIN)
- J2: Keyboard matrix input (16-pin IDC from Keyboard Helper)
- J3: VIA Port A connection (10-pin IDC to GPIO Card)
- J4: VIA Port B connection (10-pin IDC to GPIO Card)
- J5: Control signals (CA1, CA2, CB1, CB2)

**Firmware:** [KEH Controller](../Firmware/KEH%20Controller/)

**Features:**
- Simultaneous PS/2 and matrix input support
- ASCII conversion (0x00-0xFF including extended)
- Full modifier support (Shift, Ctrl, Alt, Fn)
- Function keys F1-F12
- Circular buffer (16 bytes)

**Operation:**
- PS/2 mode: ASCII output on VIA Port A, data ready strobe on CA1
- Matrix mode: Scan rows on Port A, read columns on Port B, ASCII output on Port B

**Power Consumption:** ~50mA

---

### PS2 Helper

**PCB:** 2-layer, 60mm × 40mm  
**Microcontroller:** ATmega328p  
**Input:** PS/2 keyboard  
**Output:** MT8808 8×8 crosspoint switch control (emulates keyboard matrix)  

**Connections:**
- J1: PS/2 keyboard (6-pin Mini-DIN)
- J2: MT8808 control signals (AX0-AX2, AY0-AY2, DATA, STROBE, RESET)
- J3: VIA connection (to GPIO Card)

**Firmware:** [PS2 Keyboard Controller](../Firmware/PS2%20Keyboard%20Controller/)

**Operation:**
- Receives PS/2 scancodes
- Converts to matrix coordinates
- Controls MT8808 to close appropriate row/column intersection
- VIA scans matrix normally (transparent to software)

**Power Consumption:** ~40mA

---

### Clock Helper

**PCB:** 2-layer, 60mm × 60mm  
**Oscillator:** Crystal oscillator (1, 2, 4, or 8 MHz typical)  
**Modes:** Free-run, single-cycle step, single-instruction step  

**Connectors:**
- J1: Clock output (to CPU or Backplane Pro)
- J2: CPU SYNC input (for instruction stepping)

**Controls:**
- SW1: Run/Stop toggle
- SW2: Single step (momentary)

**Operation:**
- **Free-run mode:** Normal operation, clock output matches crystal frequency
- **Single-cycle mode:** Each press of SW2 advances clock one cycle
- **Single-instruction mode:** Monitors SYNC signal, steps until next instruction fetch

**Power Consumption:** <10mA

---

## Carts Detailed Reference

### ROM Cart

**PCB:** 2-layer, 80mm × 60mm  
**ROM:** EEPROM (28C256 or compatible, DIP-28 socket)  
**Capacity:** Up to 32KB (16KB typical for cartridge slot)  
**Address Range:** $C000-$FFFF (16KB, when inserted)  

**Connector:** Card edge (compatible with Main Board cart slot)

**Programming:**
- Remove from cart slot
- Program EEPROM using standard programmer (TL866, MiniPro)
- Reinsert into cart slot

**Use Cases:**
- VCS game cartridges
- Swappable BIOS/monitor ROMs
- Development (quick ROM changes without desoldering)

**Jumpers:**
- JP1: Address map select (if supporting multiple sizes)

**Power Consumption:** <10mA

---

## Assembly Notes

### SMD vs. Through-Hole

**Fully SMD boards** (JLCPCB assembly recommended):
- Backplane Pro
- Dev Board
- Dev Output Board
- Most cards

**Mixed SMD + Through-hole:**
- Main Board (SMD + sockets for RAM/ROM/CPU)
- Memory Card (SMD + sockets)
- Serial Card (SMD + DB9 connector)
- Most helpers

**Mostly Through-hole:**
- Prototype Card
- Breadboard Helper
- Some helper boards

### Component Orientation

**ICs in DIP sockets:**
- Pin 1 indicator (notch or dot) must align with socket and PCB marking
- CPU, RAM, ROM: Check datasheets for pin 1 location

**Electrolytic capacitors:**
- Polarity marked on PCB (+ and -)
- Negative stripe on capacitor body aligns with - on PCB

**Diodes:**
- Cathode band aligns with band on PCB silkscreen

**LEDs:**
- Longer lead (anode) to + pad
- Flat side (cathode) to - pad or square pad

### Soldering Tips

**Through-hole:**
- Insert component from top, bend leads on bottom
- Apply soldering iron to pad + lead simultaneously
- Feed solder to junction (not to iron tip)
- Remove iron then solder wire
- Ideal joint: shiny, concave fillet

**SMD (if hand-soldering):**
- Use fine-tip soldering iron (conical tip)
- Apply flux to pads
- Tack one corner pin first
- Solder remaining pins
- Check for bridges with magnification
- Use solder wick to remove bridges

**IC sockets:**
- Tack opposite corner pins first
- Solder all remaining pins
- Check alignment before soldering all pins

### DRC (Design Rules Check) Verification

Before ordering PCBs, run DRC in KiCad:
1. Open PCB layout in KiCad
2. Select Tools → Design Rules Checker
3. Load JLCPCB design rules (if available in Production folder)
4. Run DRC
5. Fix all errors (warnings acceptable if understood)

Common errors:
- Trace too thin (minimum 0.127mm outer layer)
- Clearance too small (minimum 0.127mm)
- Via drill too small (minimum 0.3mm)
- Silkscreen over pads (warnings, JLCPCB will clip automatically)

---

## Revision History

### Rev 1.0 (Initial Release - 2024)

**Boards:**
- Initial designs for all boards
- Backplane, Backplane Pro, Main Board, Dev Board

**Cards:**
- CPU Card, Memory Card, Video Card, VGA Card, Serial Card
- GPIO Card, Sound Card, Storage Card
- Blinkenlights Card, LCD Card, Prototype Card
- RAM Card, RTC Card

**Helpers:**
- Keyboard Encoder Helper, PS2 Helper
- Keypad Helper, Keyboard Helper
- Clock Helper, GPIO Helper, Breadboard Helper
- Joystick Helper, DB25 Helper
- Backplane Helper, Mega Helper

**Carts:**
- ROM Cart

**Known Issues (Rev 1.0):**
- Some SMD footprints may not match JLCPCB assembly catalog (requires substitution)
- Backplane Pro power regulator sizing marginal under full load (use external 5V supply if issues)

### Rev 1.1 (Planned - Future)

**Improvements:**
- Update SMD footprints to match current JLCPCB catalog
- Add TVS diodes for ESD protection on external connectors
- Optimize power distribution on Backplane Pro (larger regulator)
- Add test points on critical signals
- Improve silkscreen labeling (connector pin numbers, jumper settings)

---

## Working with Hardware Designs

### Opening in KiCad

1. Install [KiCad 7.0 or later](https://www.kicad.org/)
2. Clone this repository or download specific board folder
3. Navigate to `Hardware/<BoardName>/Rev X.X/`
4. Open `<BoardName>.kicad_pro` in KiCad

### Viewing Schematics

1. With project open, click "Schematic Editor" icon
2. Navigate multi-sheet schematics with sheet hierarchy (left panel)
3. Use Find tool (Ctrl+F / Cmd+F) to locate specific nets or components

### Viewing PCB Layout

1. With project open, click "PCB Editor" icon
2. Use 3D Viewer (View → 3D Viewer) to see board in 3D
3. Toggle layers (right panel) to examine inner layers, silkscreen, etc.

### Exporting Production Files

See [Production README](../Production/README.md) for detailed export instructions.

Quick export (from PCB Editor):
- Gerbers: File → Fabrication Outputs → Gerbers
- Drill files: File → Fabrication Outputs → Drill Files
- BOM: File → Fabrication Outputs → BOM
- Pick and Place: File → Fabrication Outputs → Footprint Position (.pos) File

---

**For more information, see:**
- [Main README](../README.md) - Project overview and system descriptions
- [Production README](../Production/README.md) - Manufacturing and ordering
- [Firmware README](../Firmware/README.md) - Firmware for microcontroller-based boards
- [Schematics README](../Schematics/README.md) - Schematic viewing and conventions

---

**Last Updated:** March 7, 2026
