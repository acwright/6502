6502 Libraries
==============

This folder contains KiCad component libraries (symbols, footprints, and 3D models) used throughout the 6502 project. These libraries provide custom and project-specific components for schematic capture and PCB layout.

**Last Updated:** March 7, 2026

---

## Table of Contents

- [Overview](#overview)
- [Library Contents](#library-contents)
- [Using Libraries](#using-libraries)
- [Adding Components](#adding-components)
- [3D Models](#3d-models)

---

## Overview

KiCad uses three types of libraries:

1. **Symbol Libraries** (.kicad_sym): Schematic symbols (graphical representations for schematics)
2. **Footprint Libraries** (.pretty folders): PCB footprints (component land patterns for PCB layout)
3. **3D Model Libraries** (STEP files): 3D models for PCB visualization

---

## Library Contents

### Symbol Libraries

**6502 Parts.kicad_sym:**
- Custom symbols for 6502-specific components
- 65C02 CPU variants
- 6522 VIA (Versatile Interface Adapter)
- 6551 ACIA (serial communication)
- Memory chips (62256 SRAM, 28C256 EEPROM)
- Support ICs (74-series logic, buffers, decoders)
- Connectors (card edge, IDC headers)
- Project-specific components

### Footprint Libraries

**6502 Parts.pretty/:**
- PCB footprints for 6502 project components
- DIP packages (CPU, RAM, ROM, ICs)
- SMD packages (0805, 1206 resistors/caps, SOICs, TSSOPs)
- Connectors (IDC headers, card edges, barrel jacks, DB9)
- Custom footprints (unique to this project)

**6502 Logos.pretty/:**
- Project logo footprints for PCB silkscreen
- Various sizes for different board sizes
- Suitable for silkscreen layer branding

**A.C. Wright Logo.pretty/:**
- Designer logo footprints
- Used for attribution on PCB silkscreen

### 3D Models

**Models/:**
- STEP files for components
- Used in KiCad 3D viewer
- Accurate component dimensions
- See [Models README](./Models/README.md) for details

---

## Using Libraries

### Adding to KiCad Project

**Method 1: Project-Specific (Recommended)**

1. Copy `Libraries/` folder to your project directory, OR
2. Create symbolic link:
   ```bash
   ln -s /path/to/6502/Libraries /path/to/your_project/Libraries
   ```

3. In KiCad project:
   - **Schematic Editor** → Preferences → Manage Symbol Libraries
   - Click "+" (Add existing library)
   - Browse to `Libraries/6502 Parts.kicad_sym`
   - Choose "Relative to project" path
   
4. Similarly for footprints:
   - **PCB Editor** → Preferences → Manage Footprint Libraries
   - Add `Libraries/6502 Parts.pretty` folder

**Method 2: Global (Available to All Projects)**

1. **Symbol Libraries:**
   - Preferences → Manage Symbol Libraries
   - Switch to "Global Libraries" tab
   - Add `6502 Parts.kicad_sym` with absolute path

2. **Footprint Libraries:**
   - Preferences → Manage Footprint Libraries
   - Switch to "Global Libraries" tab
   - Add `6502 Parts.pretty` folder with absolute path

### Using Components in Schematic

1. In Schematic Editor, press `A` (Add Symbol)
2. Search for component name (e.g., "W65C02S", "6522")
3. Components from 6502 Parts library will appear
4. Select and place on schematic

### Assigning Footprints

1. With component selected, press `E` (Edit Properties)
2. Click footprint field
3. Browse footprint libraries
4. Select appropriate footprint from "6502 Parts" library
5. Click OK

---

## Adding Components

### Adding New Symbols

1. Open Schematic Editor
2. Tools → Symbol Editor
3. File → Open Library → select `6502 Parts.kicad_sym`
4. File → New Symbol
5. Draw symbol:
   - Add pins (Place → Pin)
   - Add graphical elements (rectangles, arcs, text)
   - Set pin numbers and names
6. Save symbol
7. Close Symbol Editor
8. New symbol now available in schematics

**Symbol Guidelines:**
- Use standard pin orientations (inputs left, outputs right)
- Label all pins clearly
- Group related pins logically
- Add datasheet link in symbol properties
- Include descriptive value/reference fields

### Adding New Footprints

1. Open PCB Editor
2. Tools → Footprint Editor
3. File → New Footprint
4. Draw footprint:
   - Add pads (Place → Pad)
   - Set pad numbering to match symbol pins
   - Add silkscreen outline (F.SilkScreen layer)
   - Add component value text (REF** and VALUE)
   - Add courtyard (F.CrtYd layer) with 0.25mm clearance
5. Set 3D model (if available):
   - Footprint Properties → 3D Settings
   - Add STEP file from Models/ folder
   - Adjust scale/rotation/offset
6. Save to `6502 Parts.pretty/` folder
7. New footprint available for assignment

**Footprint Guidelines:**
- Follow IPC-7351 standards for pad dimensions
- Add reference designator (REF**) on F.SilkScreen
- Add component value (VALUE) on F.Fab layer
- Include courtyard (0.25mm clearance around component)
- Add pin 1 indicator for polarized/oriented components
- Verify against component datasheet dimensions

### Linking 3D Models

When creating footprints with 3D models:

1. Place STEP file in `Libraries/Models/`
2. In Footprint Editor:
   - Footprint Properties → 3D Settings tab
   - Add 3D Model
   - Browse to STEP file
   - Adjust:
     - **Scale**: Usually 1.0 for STEP files
     - **Rotation**: Align with PCB orientation
     - **Offset**: Position accurately on pads
3. Verify in 3D Viewer (View → 3D Viewer)

---

## 3D Models

See [Models/README.md](./Models/README.md) for:
- Available 3D model library
- Model sources (manufacturer, GrabCAD, component library)
- How to add new models
- Model file formats and conversions

---

**For more information, see:**
- [Main README](../README.md) - Project overview
- [Hardware README](../Hardware/README.md) - PCB designs using these libraries
- [KiCad Libraries Documentation](https://docs.kicad.org/7.0/en/kicad/kicad.html#libraries) - Official KiCad library guide

---

**Last Updated:** March 7, 2026