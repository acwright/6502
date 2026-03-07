6502 Templates
==============

This folder contains KiCad project templates for creating new boards, cards, or helpers in the 6502 project. Templates provide pre-configured project settings, library links, and starting schematics/PCBs that follow project conventions.

**Last Updated:** March 7, 2026

---

## Overview

Templates accelerate new board development by providing:
- Pre-configured project settings
- Linked symbol and footprint libraries (6502 Parts, etc.)
- Standard design rules for JLCPCB manufacturing
- Worksheet (title block) with project information
- Starting schematic/PCB layouts (for cards: standard card edge connector)

---

## Available Templates

**Card Template:**
- For backplane-compatible peripheral cards
- Includes standard card edge connector
- Preconfigured dimensions (100mm × 60mm typical)
- Bus connection labels (A0-A15, D0-D7, control signals)
- Address decoding subcircuit placeholder

**Helper Template:**
- For helper boards connecting to GPIO Card or other peripherals
- Standard IDC headers
- Power connections

**Board Template:**
- Blank starting point for standalone boards
- Standard libraries linked
- JLCPCB design rules preloaded

---

## Using Templates

### Creating New Project from Template

1. **In KiCad:**
   - File → New Project from Template
   - Browse to `Templates/<TemplateName>/`
   - Select template
   - Choose destination folder for new project
   - Click OK

2. **KiCad copies template** to new location

3. **Rename project:**
   - Files are copied with template name
   - Rename `.kicad_pro`, `.kicad_sch`, `.kicad_pcb` to your board name

4. **Update project metadata:**
   - Open schematic
   - Edit title block (File → Page Settings)
   - Update title, date, revision, author

5. **Begin design:**
   - Add components to schematic
   - Assign footprints
   - Update PCB from schematic
   - Route PCB

### Manual Setup (Without Template)

If templates not installed:

1. Create new KiCad project
2. Add libraries:
   - Symbol libraries: `Libraries/6502 Parts.kicad_sym`
   - Footprint libraries: `Libraries/6502 Parts.pretty/`, etc.
3. Import design rules:
   - PCB Editor → File → Board Setup → Design Rules
   - Import `.kicad_dru` file from `Production/<SimilarBoard>/`
4. Set up title block:
   - File → Page Settings
   - Enter project information

---

## Customizing Templates

To create your own templates:

1. Create a complete KiCad project (schematic + PCB)
2. Set up all libraries, design rules, and settings
3. Create generic/placeholder components (to be customized by users)
4. Save project to `Templates/<YourTemplateName>/`
5. Add `meta/info.html` file (template description for KiCad)
6. (Optional) Add preview image `meta/icon.png`

**meta/info.html example:**
```html
<!DOCTYPE HTML>
<html>
<head>
<meta charset="UTF-8">
</head>
<body>
<h1>6502 Card Template</h1>
<p>
Template for creating backplane-compatible cards. Includes standard
card edge connector, address decoding subcircuit, and project libraries.
</p>
</body>
</html>
```

---

**For more information, see:**
- [Main README](../README.md) - Project overview
- [Hardware README](../Hardware/README.md) - Hardware design conventions
- [Libraries README](../Libraries/README.md) - Component libraries

---

**Last Updated:** March 7, 2026
