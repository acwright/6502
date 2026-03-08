6502 Production Files
=====================

This folder contains JLCPCB-ready production files for manufacturing all PCBs in the 6502 project. Each board/card/helper has complete manufacturing packages including Gerber files, drill files, BOMs (Bill of Materials), and pick-and-place files for assembly.

---

## Table of Contents

- [Overview](#overview)
- [Folder Structure](#folder-structure)
- [Ordering from JLCPCB](#ordering-from-jlcpcb)
- [Production Files Included](#production-files-included)
- [Cost Estimates](#cost-estimates)
- [Quality Control](#quality-control)
- [Through-Hole Assembly](#through-hole-assembly)
- [Troubleshooting Orders](#troubleshooting-orders)

---

## Overview

All production files are optimized for **JLCPCB** manufacturing and assembly services. Files follow JLCPCB's specifications and use components from their assembly catalog when possible.

**Production File Generation:**
- All production files are generated using [Fabrication-Toolkit](https://github.com/bennymeg/Fabrication-Toolkit)
- This KiCad plugin automates the generation of Gerbers, drill files, BOMs, and pick-and-place files optimized for JLCPCB

**What's Included:**
- Gerber files (PCB layers, soldermask, silkscreen)
- Excellon drill files (plated and non-plated holes)
- BOMs with JLCPCB part numbers (LCSC codes)
- Pick-and-place (CPL) files for SMD assembly
- Design rule files (.kicad_dru) verified for JLCPCB capabilities

---

## Folder Structure

```
Production/
├── <BoardName>/
|   └── Rev X.X/
│       ├── <BoardName>_Gerber.zip       # Gerber + drill files (upload to JLCPCB)
│       ├── <BoardName>_BOM.csv          # Bill of Materials
│       ├── <BoardName>_CPL.csv          # Component placement (pick-and-place)
│       ├── <BoardName>.kicad_dru        # Design rules file
│       └── README.md (optional)         # Board-specific notes
└── ...
```

---

## Ordering from JLCPCB

### Step-by-Step Guide

**1. Prepare Files**
   - Locate board folder in Production/
   - Identify Gerber ZIP, BOM CSV, and CPL CSV files

**2. Go to JLCPCB Website**
   - Navigate to [jlcpcb.com](https://jlcpcb.com/)
   - Create account or log in

**3. Upload Gerber File**
   - Click "Order now" or "Quote now"
   - Click "Add gerber file"
   - Upload `<BoardName>_Gerber.zip`
   - Wait for automatic parsing and preview

**4. Configure PCB Options**

   **Basic Settings:**
   - **Dimensions**: Auto-detected from Gerber (verify correct)
   - **Layers**: 2 or 4 (check Hardware README for specific board)
   - **Quantity**: 5 or 10 minimum (5 is usually cheapest for prototypes)
   - **Thickness**: 1.6mm (standard)
   
   **Advanced Settings:**
   - **PCB Color**: Your choice
   - **Surface Finish**: 
     - HASL (lead-free): Most economical
     - ENIG: Better for edge connectors and fine pitch
   - **Remove Order Number**: Optional
   - **Gold Fingers**: Yes (for card edge connectors on Cards), No (for most boards)
   
**5. Enable SMD Assembly** (Optional but Recommended)
   - Scroll down to "SMT Assembly" section
   - Toggle "SMT Assembly" to ON
   
   **Assembly Options:**
   - **PCBA Type**: Economic (standard, good for most boards)
   - **Assembly Side**: Top Side (unless BOM specifies bottom)
   - **PCBA Qty**: Same as PCB quantity or fewer (e.g., order 10 PCBs, assemble 5)
   
   **Upload Assembly Files:**
   - Click "Add BOM File" → upload `bom.csv`
   - Click "Add CPL File" → upload `positions.csv`
   - Click "Process BOM & CPL"
   
**6. Component Selection**
   - JLCPCB will parse BOM and match components
   - Review component list:
     - **Green checkmark**: Component in stock, will be assembled
     - **Orange warning**: Component unavailable, select substitute or omit
     - **Red X**: Invalid part number, requires correction
   
   **For unavailable components:**
   - Click component row to search alternates
   - Select equivalent component (check datasheet for compatibility)
   - OR check "Do not place" to assemble manually later
   
**7. Confirm Placement**
   - Click "Next"
   - Review component placement preview (3D view)
   - Verify:
     - Component orientation (especially ICs, polarized parts)
     - No components placed on pads or traces
     - Correct side (top vs. bottom)
   - Click "Confirm" if everything looks correct
   
**8. Review and Order**
   - Review order summary:
     - PCB cost
     - Assembly cost ($3-8 setup fee + component costs)
     - Shipping cost
   - Select shipping method
   - Add to cart
   - Proceed to checkout
   - Enter shipping address and payment
   - Submit order

**9. Wait for Production**
   - PCB manufacturing: 1-3 days (standard)
   - SMD assembly: 1-4 days (after PCB complete)
   - Shipping: varies by method
   - Total: typically 7-21 days door-to-door

**10. Track Order**
   - JLCPCB sends email updates
   - Track shipment via provided tracking number

---

## Production Files Included

### Gerber Files (.gbr)

Gerber format describes each PCB layer:
- **.GTO** / **.gto**: Top silkscreen (component labels)
- **.GBO** / **.gbo**: Bottom silkscreen
- **.GTS** / **.gts**: Top soldermask (green/black coating)
- **.GBS** / **.gbs**: Bottom soldermask
- **.GTL** / **.gtl**: Top copper layer
- **.GBL** / **.gbl**: Bottom copper layer
- **.G2** / **.g2**: Inner layer 2 (4-layer boards only)
- **.G3** / **.g3**: Inner layer 3 (4-layer boards only)
- **.GKO** / **.gko** / **.gm1**: Board outline (edge cuts)

### Drill Files (.drl, .xln)

- **PTH (Plated Through-Holes)**: Holes with copper plating (component leads, vias)
- **NPTH (Non-Plated Through-Holes)**: Mounting holes, tooling holes

### BOM (Bill of Materials) (.csv)

CSV file listing all components:
- **Designator**: Component reference (U1, R1, C1, etc.)
- **Quantity**: Number of this component
- **Value**: Component value or part number
- **Footprint**: PCB footprint name
- **LCSC Part #**: JLCPCB/LCSC component catalog number

### CPL (Component Placement List) (.csv)

Pick-and-place file for assembly machines:
- **Designator**: Component reference
- **Mid X/Y**: Component center coordinates (mm)
- **Rotation**: Component orientation (degrees)
- **Layer**: Top or Bottom

---

## Quality Control

### When Boards Arrive

**Visual Inspection:**
1. Check for physical damage (cracks, bent boards)
2. Verify all SMD components placed correctly
3. Check component orientation (IC pin 1, polarized capacitors)
4. Inspect solder joints (should be shiny, concave fillets)
5. Look for solder bridges (shorts between pins)
6. Verify silkscreen legibility

**Electrical Testing:**
1. **Continuity Check:**
   - Use multimeter in continuity mode
   - Verify power rails (5V to all VCC pins)
   - Verify ground connections (GND to all GND pins)
   
2. **Short Circuit Check:**
   - Measure resistance between 5V and GND (should be >10kΩ typically)
   - If <100Ω, likely short circuit (do NOT power on)
   
3. **Visual Inspection (Magnified):**
   - Use magnifying glass or microscope
   - Check for solder bridges (especially fine-pitch ICs)
   - Check for cold solder joints (dull, grainy appearance)
   - Verify component values (especially resistors/capacitors if hand-placed)

**Common Issues:**

| Issue | Cause | Solution |
|-------|-------|----------|
| Wrong component placed | BOM error or JLCPCB misidentification | Desolder and replace with correct part |
| Component rotated 90/180° | CPL rotation error | Desolder and rotate to correct orientation |
| Solder bridge | Excess solder or component misalignment | Remove with solder wick or solder sucker |
| Missing component | Component out of stock | Hand-solder component |
| Tombstoning (standing component) | Uneven heating during reflow | Reheat or replace component |

---

## Through-Hole Assembly

Most boards require some manual through-hole assembly after SMD assembly.

### Typical Through-Hole Components:
- IC sockets (CPU, RAM, ROM)
- Connectors (IDC headers, barrel jacks, DB9)
- Electrolytic capacitors (polarized)
- Crystals/oscillators
- Some resistors/capacitors (if space allows)

### Assembly Order (Recommended):
1. Shortest components first (resistors, diodes, small capacitors)
2. IC sockets (don't insert ICs yet)
3. Medium-height components (ceramic capacitors, headers)
4. Tall components (electrolytic capacitors, connectors)
5. Inspect all solder joints
6. Clean flux residue (isopropyl alcohol + soft brush)
7. Insert ICs into sockets (verify pin 1 orientation!)

### Soldering Tips:
- Use 330-350°C soldering iron (lead-free solder) or 300-320°C (leaded)
- Apply flux to through-hole pads before soldering
- Heat pad + lead simultaneously (1-2 seconds)
- Feed solder to joint (not to iron tip)
- Remove solder wire, then iron
- Ideal joint: shiny, concave fillet around lead

---

## Troubleshooting Orders

**Problem: Component marked unavailable in JLCPCB BOM**
- **Solution 1:** Search for alternate LCSC part number (similar specs)
- **Solution 2:** Mark "Do not place" and hand-solder later
- **Solution 3:** Redesign board to use available component (long-term)

**Problem: CPL placement preview shows wrong orientation**
- **Solution:** Edit CPL file rotation column (add/subtract 90° or 180°)
- Re-upload corrected CPL file

**Problem: Order rejected due to design rule violation**
- **Solution:** Download JLCPCB DRC report
- Identify violation (trace too thin, clearance too small, via size, etc.)
- Fix in KiCad and regenerate production files
- Re-upload corrected Gerber files

**Problem: Boards arrive with solder bridges**
- **Solution:** Use solder wick or desoldering braid to remove excess solder
- Apply flux, heat with soldering iron, wick absorbs solder

**Problem: Wrong component value assembled**
- **Solution:** Verify BOM LCSC part numbers match intended components
- Check LCSC website for component specifications
- Desolder incorrect component, solder correct one

---

**For more information, see:**
- [Main README](../README.md) - Project overview and assembly guide
- [Hardware README](../Hardware/README.md) - PCB specifications and design details
- [JLCPCB Capabilities](https://jlcpcb.com/capabilities/Capabilities) - Current manufacturing limits

---