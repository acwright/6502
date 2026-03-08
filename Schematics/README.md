6502 Schematics
===============

This folder contains **PDF exports** of schematic diagrams for all PCBs in the 6502 project. The original KiCad schematic source files (.kicad_sch) are located in the [Hardware](../Hardware) folder alongside the PCB layouts.

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

This folder provides **PDF exports** of all schematics for easy viewing without requiring KiCad. The PDFs show:
- Component placement and values
- Net connections (wires)
- Power distribution
- Signal routing
- Component reference designators

**Use cases:**
- Quick reference without opening KiCad
- Understanding circuit operation
- Troubleshooting hardware issues
- Printing for bench reference
- Learning digital design principles

**To modify designs:** Access the original KiCad schematic files in the [Hardware](../Hardware) folder.

---

## Folder Structure

```
Schematics/
├── <BoardName>/<Rev X.X>/
│   └── <BoardName>.pdf           # PDF schematic export
└── ...
```

**For KiCad source files (.kicad_sch), see:**
```
Hardware/
├── <BoardName>/
│   └── Rev X.X/
│       ├── <BoardName>.kicad_sch      # Schematic source file
│       ├── <BoardName>.kicad_pcb      # PCB layout
│       └── <BoardName>.kicad_pro      # KiCad project
└── ...
```

### Multi-Sheet Schematics

Complex designs use hierarchical schematics with multiple sheets. In the PDF exports:
- Multi-page PDFs contain all sheets
- Each page represents one schematic sheet
- Navigation index usually on first page

---

## Viewing Schematics

### PDF Viewing (This Folder)

1. **Navigate** to `Schematics/<BoardName>/`
2. **Open** `<BoardName>.pdf` in any PDF viewer
   - Adobe Acrobat Reader
   - Preview (macOS)
   - Browser (Chrome, Firefox, Edge)
   - PDF viewers on Linux (Evince, Okular)

**Features:**
- Searchable text (component references, net names)
- Printable for bench reference
- Zoom and pan for detail
- Multi-page navigation for complex designs
- No KiCad installation required

**Limitations:**
- Not editable (view-only)
- Cannot export netlists or BOMs

### Editing Schematics (KiCad Required)

To modify schematics:
1. **Install KiCad 7.0+** from [kicad.org](https://www.kicad.org/)
2. **Navigate** to `Hardware/<BoardName>/Rev X.X/`
3. **Open** `<BoardName>.kicad_sch` in KiCad Schematic Editor
4. Make edits and regenerate PDFs if needed

---

## Reading Schematics

### Navigation in PDF

**Single-Page PDFs:**
- Zoom: Ctrl/Cmd + Plus/Minus or zoom controls
- Pan: Click and drag or scroll bars
- Fit to page: Zoom controls or keyboard shortcuts (varies by viewer)
- Search: Ctrl/Cmd + F to find component references or net names

**Multi-Page PDFs:**
- Page navigation: Sidebar thumbnails or page controls
- Jump to page: Direct page number entry
- Bookmarks: Some PDFs include bookmarks for major sections

### Component Information

**Component Symbol:**
- Shows graphical representation (IC outline, resistor symbol, etc.)
- Pin numbers and names
- Reference designator (U1, R5, C12)
- Value (10K, 100nF, 74LS245)

**To see additional details** (footprint, datasheet, part numbers):
- Refer to BOM files in Production/ folder
- Open source schematic in KiCad (Hardware/ folder)

### Net Names

Wires (nets) connect components:
- **Labels**: Named connections (e.g., A0, D7, PHI2)
- **Global labels**: Connections across sheets (bus signals)
- **Power symbols**: +5V, GND (automatically connected)

**Tracing connections in PDF:**
- Follow wire lines visually
- Use search (Ctrl/Cmd + F) to find all instances of a net name
- Check multiple pages for connections in multi-sheet designs

---

## Schematic Conventions

### Net Naming

**Address Bus:**
- A0, A1, A2, ..., A15 (16-bit address)

**Data Bus:**
- D0, D1, D2, ..., D7 (8-bit data)

**Bus Control Signals:**
- PHI2: Clock
- RWB: Read/Write (1=Read, 0=Write)
- RESB: Reset (active low)
- IRQB: Interrupt Request (active low)
- NMIB: Non-Maskable Interrupt (active low)
- RDY: Ready (for clock stretching)
- SYNC: Sync 
- BE: Bus Enable
- EXP0-3: Extra Pins 0-3 (Reserved for future use)

**Power:**
- +5V or VCC: Positive supply
- GND: Ground

**Chip Enables:**
- CS: Chip Enable/Chip Select (active high)
- CSB: Chip Enable/Select (active low, indicated by B suffix or overline)

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
- 100 or 100R or 100Ω = 100 ohms
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

## Regenerating PDF Exports

The PDFs in this folder are exported from the KiCad source files in the Hardware/ folder. If you modify a schematic, regenerate the PDF:

### PDF Export from KiCad

1. Open schematic in KiCad (from Hardware/<BoardName>/ folder)
2. File → Plot
3. Select "PDF" as output format
4. Configure:
   - Include border + title block: Yes
   - Single page per sheet: Yes (for multi-sheet)
   - Output directory: Set to Schematics/<BoardName>/
5. Click "Plot All Pages"
6. PDF saved to Schematics/ folder

### Additional Exports (From Hardware/ Folder)

**Netlist Export:**
1. File → Export → Netlist
2. Select format (usually "Classic KiCad")
3. Use for simulation tools (SPICE, LTSpice) or custom analysis

**BOM Export:**
1. File → Export → BOM
2. Select BOM format plugin
3. Use for component ordering and cost estimation

**Note:** Production-ready BOM files are available in the Production/ folder.

---

**For more information, see:**
- [Main README](../README.md) - Project overview
- [Hardware README](../Hardware/README.md) - PCB layouts and pin assignments
- [KiCad Documentation](https://docs.kicad.org/) - KiCad user guide

---

