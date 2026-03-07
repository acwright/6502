6502 CAD Designs
================

This folder contains mechanical CAD designs for enclosures, mounting solutions, and 3D models for the 6502 project. Designs are available in multiple formats for 3D printing, CNC machining, or laser cutting.

**Last Updated:** March 7, 2026

---

## Table of Contents

- [Overview](#overview)
- [Folder Structure](#folder-structure)
- [Available Designs](#available-designs)
- [File Formats](#file-formats)
- [3D Printing](#3d-printing)
- [Using CAD Files](#using-cad-files)
- [Contributing CAD Designs](#contributing-cad-designs)

---

## Overview

CAD designs complement the PCBs by providing:
- **Enclosures**: Protective cases for complete systems
- **Mounting Solutions**: Standoffs, brackets, panel mounts
- **Mechanical Integration**: Keyboard housings, card guides, backplane frames
- **Visualization**: 3D models for understanding system assembly

---

## Folder Structure

```
CAD/
├── Bases/              # Complete system assemblies (COB, DEV, KIM, VCS)
│   ├── The COB/
│   │   ├── The COB.stl
│   │   ├── The COB.step
│   │   └── The COB.png (preview image)
│   ├── The DEV/
│   ├── The KIM/
│   └── The VCS/
├── Card/               # 3D models of individual cards
├── Enclosures/         # Custom enclosure designs
└── Tops/               # Top panels and covers
```

---

## Available Designs

### System Bases

**The COB Base:**
- 3D model showing complete COB assembly
- Includes backplane, cards, helpers
- For visualization and enclosure design reference

**The DEV Base:**
- Dev Board + Dev Output Board assembly
- Compact footprint design

**The KIM Base:**
- Main Board + keypad + LCD arrangement
- Retro styling inspired by original KIM-1

**The VCS Base:**
- Main Board + Input Board + Output Board layout
- Console-style form factor

### Enclosures

Enclosure designs (when available) include:
- Top and bottom shells
- Cutouts for connectors (power, video, serial, USB, etc.)
- Ventilation slots
- Mounting holes for standoffs
- Front panel cutouts (keypad, LCD, LEDs)

### Top Panels

- Acrylic or aluminum top panels with:
  - Card slot labels
  - Status LED windows
  - Button cutouts
  - Connector access

---

## File Formats

### STL (Stereolithography)
- **Purpose**: 3D printing
- **Software**: Any slicer (Cura, PrusaSlicer, Simplify3D)
- **Advantages**: Universal format, directly printable
- **Limitations**: Not easily editable

### STEP (Standard for the Exchange of Product Data)
- **Purpose**: CAD interchange, editing, CNC machining
- **Software**: Fusion 360, FreeCAD, SolidWorks, OnShape, etc.
- **Advantages**: Parametric, editable, high precision
- **Limitations**: Larger files

### DXF (Drawing Exchange Format) - If Available
- **Purpose**: 2D laser cutting, CNC routing
- **Software**: LaserCut, AutoCAD, Inkscape, etc.
- **Advantages**: Precise 2D outlines
- **Use**: Panel mounting holes, acrylic panels, metal enclosures

---

## 3D Printing

### Recommended Settings

**Enclosures:**
- **Material**: PLA (easiest), PETG (more durable), ABS (heat-resistant)
- **Layer Height**: 0.2mm (balance of speed and quality)
- **Infill**: 15-25% (adequate strength for enclosures)
- **Supports**: Usually required for overhangs >45°
- **Perimeters**: 3-4 walls (for strength)
- **Print Speed**: 50-60mm/s (PLA), 40-50mm/s (PETG)

**Mounting Brackets:**
- **Material**: PETG or ABS (stronger than PLA)
- **Layer Height**: 0.15-0.2mm
- **Infill**: 30-50% (more strength needed)
- **Perimeters**: 4-5 walls

**Standoffs:**
- **Material**: PETG (strong, slightly flexible)
- **Infill**: 50-100% (solid for threaded inserts)
- **Print Orientation**: Vertical (for threaded inserts)

### Post-Processing

**Remove Supports:**
- Use side cutters or pliers
- Sand support contact areas if needed

**Threaded Inserts:**
- Many designs include holes for M2.5 or M3 threaded inserts
- Heat insert with soldering iron (set to 200-230°C)
- Press gently into plastic until flush

**Sanding/Finishing:**
- Sand with 200→400→600 grit for smooth finish
- Use filler primer for painted enclosures

---

## Using CAD Files

### Viewing 3D Models

**Online Viewers:**
- [3D Viewer Online](https://3dviewer.net/)
- Upload STL or STEP files to view in browser

**Desktop Software:**
- [FreeCAD](https://www.freecad.org/) (free, open-source)
- [Fusion 360](https://www.autodesk.com/products/fusion-360/personal) (free for hobbyists)
- [Blender](https://www.blender.org/) (free, primarily for rendering)

### Editing STEP Files

1. Open STEP file in Fusion 360 or FreeCAD
2. Modify dimensions, add features, adjust cutouts
3. Export as:
   - STL for 3D printing
   - DXF for 2D cutting (if extracting profile)
   - STEP for sharing editable version

### Customizing Enclosures

**Common Modifications:**
- Adjust enclosure dimensions for custom boards
- Add/remove connector cutouts
- Modify ventilation pattern
- Add text/logos to top panel
- Change mounting hole spacing

**Workflow:**
1. Import STEP file to CAD software
2. Make edits (add holes, change size, etc.)
3. Export STL
4. Slice and print

---

## Contributing CAD Designs

We welcome community contributions of enclosures and mechanical designs!

### Design Guidelines

**Enclosures:**
- Measure actual PCB dimensions (not just schematic)
- Allow 2-3mm clearance around PCBs
- Include ventilation (esp. for boards with regulators or microcontrollers)
- Design for printability (minimize supports)
- Include mounting holes for M2.5 or M3 standoffs
- Consider connector access

**File Requirements:**
- Provide both STL (for printing) and STEP (for editing)
- Include preview image (PNG or JPG)
- Write brief README describing design

### Submitting Designs

1. Create design in CAD software
2. Test print prototype
3. Export STL + STEP files
4. Add folder to `CAD/<CategoryName>/<YourDesign>/`
5. Include README.md with:
   - Description
   - Print settings used
   - Assembly instructions
   - Photos of finished print
6. Submit pull request on GitHub

---

**For more information, see:**
- [Main README](../README.md) - Project overview
- [Hardware README](../Hardware/README.md) - PCB dimensions and specifications

---

**Last Updated:** March 7, 2026
