6502 Hardware
=============

This folder contains KiCad hardware designs for all PCBs in the 6502 project. Each design includes complete schematics, PCB layouts, and component libraries.

---

## Table of Contents

- [Overview](#overview)
- [Folder Structure](#folder-structure)
- [Hardware Categories](#hardware-categories)
- [Design Standards](#design-standards)
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
│   ├── Rev X.X/
│       ├── <BoardName>.kicad_pro    # KiCad project file
│       ├── <BoardName>.kicad_sch    # Schematic file
│       ├── <BoardName>.kicad_pcb    # PCB layout file
│       ├── fp-lib-table              # Footprint library table
│       └── sym-lib-table             # Symbol library table
└── ...
```

Each board folder contains one or more revision subfolders. Always use the latest revision unless specifically maintaining compatibility with older versions.

---

## Design Standards

All hardware designs follow these standards for consistency and manufacturability:

### PCB Specifications

**Layers:**
- Simple boards: 2-layer

**Dimensions:**
- Card width: 100mm
- Card height: 60mm
- Board width: 100mm
- Board height: 100mm
- Helper width: 100mm
- Helper height: 50mm
- PCB thickness: 1.6mm standard

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
- Through-hole: 1/8W resistors, radial capacitors (when space allows)

**Connectors:**
- Through-hole: For all connectors (mechanical stress consideration)
- IDC headers: 2.54mm pitch for ribbon cables
- Card edge: Gold-plated fingers for backplane cards

### Schematic Conventions

**Net Naming:**
- Address bus: A0-A15
- Data bus: D0-D7
- Control signals: PHI2, RWB, RESB, IRQB, NMIB, etc.
- Power: +5V or VCC, GND
- Chip enables: CS, CSB (active low)

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

## Assembly Notes

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

### Rev 0.0 (Prototyping - 2024)

**Boards:**
- Dev Board

**Known Issues (Rev 1.0):**
- Dev Board 0.0 uses TXS0108 level shifters which were found to be unpredictable on the 6502 bus with some cards

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
- None

### Rev 1.1 (Planned - Future)

**Improvements:**
- TODO

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
