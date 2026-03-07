6502 Schematics
===============

This folder contains schematic diagrams for all PCBs in the 6502 project. Schematics are created using **KiCad 7.0+** and provide complete electrical design documentation for every board, card, and helper.

**Last Updated:** March 7, 2026

---

## Table of Contents

- [Overview](#overview)
- [Folder Structure](#folder-structure)
- [Opening Schematics](#opening-schematics)
- [Reading Schematics](#reading-schematics)
- [Schematic Conventions](#schematic-conventions)
- [Exporting Schematics](#exporting-schematics)

---

## Overview

Schematics show electrical connections between components:
- Component placement and values
- Net connections (wires)
- Power distribution
- Signal routing
- Component reference designators

**Use cases:**
- Understanding circuit operation
- Troubleshooting hardware issues
- Modifying designs
- Learning digital design principles
- Creating custom peripherals

---

## Folder Structure

```
Schematics/
├── <BoardName>/
│   ├── <BoardName>.kicad_sch      # Schematic file
│   ├── <Sheet>.kicad_sch          # Additional sheets (if multi-sheet)
│   ├── <BoardName>.pdf (optional) # PDF export for easy viewing
│   └── sym-lib-table              # Symbol library table
└── ...
```

### Multi-Sheet Schematics

Complex designs use hierarchical schematics:
- **Root sheet**: Top-level schematic showing major blocks
- **Sub-sheets**: Detailed schematics for each block (CPU, memory, I/O, power, etc.)

**Example (Main Board):**
```
Main Board/
├── Main Board.kicad_sch         # Root sheet
├── CPU.kicad_sch                # CPU subcircuit
├── Memory.kicad_sch             # RAM/ROM subcircuit
├── Clock_Reset.kicad_sch        # Clock and reset circuitry
└── Power.kicad_sch              # Power supply
```

---

## Opening Schematics

### Using KiCad

1. **Install KiCad 7.0+** from [kicad.org](https://www.kicad.org/)
2. **Navigate** to `Schematics/<BoardName>/`
3. **Open** `<BoardName>.kicad_sch` in KiCad Schematic Editor
   - Double-click file (if KiCad associated), OR
   - Launch KiCad → File → Open → select .kicad_sch file

### Viewing Without KiCad

If PDF exports are available:
- Open `<BoardName>.pdf` in any PDF viewer
- Searchable, printable, but not editable

---

## Reading Schematics

### Navigation

**Single-Sheet Schematics:**
- Pan: Middle mouse button drag or arrow keys
- Zoom: Mouse wheel or +/- keys
- Fit to screen: Home key

**Multi-Sheet Schematics:**
- Hierarchy navigator: Left sidebar shows sheet structure
- Click sheet name to navigate
- Double-click hierarchical sheet symbol to enter sub-sheet
- Up arrow icon: Return to parent sheet

### Component Information

**Component Symbol:**
- Shows graphical representation (IC outline, resistor symbol, etc.)
- Pin numbers and names
- Reference designator (U1, R5, C12)
- Value (10K, 100nF, 74LS245)

**Right-click component** → Properties to see:
- Footprint assignment
- Datasheet link
- Additional fields (manufacturer, part number)

### Net Names

Wires (nets) connect components:
- **Labels**: Named connections (e.g., A0, D7, PHI2)
- **Global labels**: Connections across sheets (bus signals)
- **Power symbols**: +5V, GND (automatically connected)

**Tracing connections:**
- Click net → highlights entire net
- Right-click → "Highlight Net" shows all connected pins

---

## Schematic Conventions

### Net Naming

**Address Bus:**
- A0, A1, A2, ..., A15 (16-bit address)

**Data Bus:**
- D0, D1, D2, ..., D7 (8-bit data)

**Control Signals:**
- PHI2: System clock
- RW: Read/Write (1=Read, 0=Write)
- RESET: System reset (active low)
- IRQ: Interrupt Request (active low)
- NMI: Non-Maskable Interrupt (active low)
- RDY: Ready (for clock stretching)

**Power:**
- +5V or VCC: Positive supply
- GND: Ground

**Chip Enables:**
- CE, CS: Chip Enable/Chip Select (active high)
- CEB, CSB: Chip Enable/Select (active low, indicated by B suffix or overline)

### Component References

| Prefix | Component Type | Examples |
|--------|----------------|----------|
| U | Integrated Circuit | U1 (CPU), U2 (RAM), U3 (VIA) |
| R | Resistor | R1 (10K), R2 (4.7K) |
| C | Capacitor | C1 (100nF), C2 (10μF) |
| D | Diode | D1 (1N4148), D2 (LED) |
| Q | Transistor | Q1 (2N3904) |
| J | Connector | J1 (USB), J2 (power jack) |
| SW | Switch | SW1 (reset button) |
| Y | Crystal/Oscillator | Y1 (1.8432 MHz) |
| L | Inductor | L1 (10μH) |
| F | Fuse | F1 (1A) |
| TP | Test Point | TP1 (GND), TP2 (5V) |

### Value Notation

**Resistors:**
- 100R or 100Ω = 100 ohms
- 1K or 1KΩ = 1,000 ohms (1 kilo-ohm)
- 10K = 10,000 ohms
- 1M = 1,000,000 ohms (1 mega-ohm)

**Capacitors:**
- 100pF = 100 picofarads
- 1nF = 1,000 pF (1 nanofarad)
- 100nF = 0.1μF (100 nanofarads or 0.1 microfarads)
- 10μF = 10 microfarads
- 1000μF = 1 millifarad

**Inductors:**
- 10μH = 10 microhenries
- 100μH = 100 microhenries

---

## Exporting Schematics

### PDF Export

1. Open schematic in KiCad
2. File → Print (or File → Plot)
3. Select "PDF" as output format
4. Configure:
   - Include border + title block: Yes
   - Single page per sheet: Yes (for multi-sheet)
5. Click "Plot All Pages"
6. PDF saved to schematic folder

**Use cases:**
- Viewing without KiCad
- Printing for reference
- Sharing with collaborators

### Netlist Export

1. File → Export → Netlist
2. Select format (usually "Classic KiCad")
3. Click "Export Netlist"

**Use cases:**
- Importing to simulation tools (SPICE, LTSpice)
- PCB layout import
- Custom analysis scripts

### BOM (Bill of Materials) Export

1. File → Export → BOM
2. Select BOM format plugin
3. Configure fields to export
4. Click "Generate"

**Use cases:**
- Component ordering
- Cost estimation
- Inventory management

---

**For more information, see:**
- [Main README](../README.md) - Project overview
- [Hardware README](../Hardware/README.md) - PCB layouts and pin assignments
- [KiCad Documentation](https://docs.kicad.org/) - KiCad user guide

---

**Last Updated:** March 7, 2026
