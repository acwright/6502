6502 Assets
===========

This folder contains supporting assets for the 6502 project including documentation, images, branding materials, and reference files.

---

## Folder Structure

```
Assets/
├── Branding/           # Logos, branding materials, visual identity
├── Documents/          # Technical documentation, datasheets, references
│   ├── Memory Map/
│   ├── Keyboard Map/
│   ├── Keyboard Layout/
│   └── Keyboard Matrix/
├── Images/             # Photos of assembled systems, boards
├── PCB Images/         # Renders and photos of individual PCBs
├── PCB Logos/          # Logo files for PCB silkscreen
└── PCB Models/         # 3D models for PCB visualization
```

---

## Folder Contents

### Branding

Logos and branding materials for the 6502 project:
- Project logo (various formats: SVG, PNG, AI)
- Color schemes and style guides
- Marketing materials

**Usage:**
- Documentation headers
- PCB silkscreen logos
- Enclosure labels
- Presentation materials

### Documents

Technical documentation and reference materials:

**Memory Map:**
- Complete memory map documentation (PDF)
- Shows memory ranges for RAM, ROM, I/O devices
- Standard across all systems

**Keyboard Map:**
- Complete keyboard layout and scancode mappings (Markdown)
- 8×8 matrix layout
- Modifier key combinations

**Keyboard Layout:**
- JSON format keyboard layout
- For use with keyboard design tools ([keyboard-layout-editor.com](https://www.keyboard-layout-editor.com))

**Keyboard Matrix:**
- Matrix scanning architecture
- Row/column assignments

### Images

Photos of complete systems:
- COB system photos
- DEV system photos
- KIM system photos
- VCS system photos
- Other general use photos
- Community contributions

**Usage:**
- Main README illustrations
- Documentation
- Social media
- Presentations

### PCB Images

Individual PCB renders and photos:
- 3D renders from KiCad
- Top/bottom layer views
- Manufactured board photos
- Assembly stages

**Usage:**
- Hardware README
- Assembly guides
- Production verification

### PCB Logos

Logo files optimized for PCB silkscreen:
- Vector formats (SVG, DXF)
- Footprint libraries for KiCad
- Various sizes for different board sizes

**Usage:**
- Import into KiCad PCB layouts
- Add to silkscreen layer
- Branding identifier on boards

### PCB Models

3D models (STEP format) of complete PCB assemblies:
- Boards with components
- For CAD assembly visualization
- Enclosure design reference

**Usage:**
- Import into enclosure CAD designs
- Verify component clearances
- System assembly visualization

---

## Key Documentation

### Memory Map

**File:** `Documents/Memory Map.png`

**Purpose:** Standard memory layout used across all 6502 systems

**Contents:**
- $0000-$7FFF: RAM (32KB)
  - $0000-$00FF: Zero Page
  - $0100-$01FF: Stack
  - $0200-$02FF: Input Buffer
  - $0300-$03FF: KERNAL Variables
  - $0400-$07FF: User Variables
  - $0800-$7FFF: Program RAM (~30KB)
- $8000-$9FFF: I/O space (8KB, 8 slots × 1KB each)
  - IO 1-2 ($8000-$87FF): RAM Card
  - IO 3 ($8800-$8BFF): RTC Card
  - IO 4 ($8C00-$8FFF): Storage Card
  - IO 5 ($9000-$93FF): Serial Card
  - IO 6 ($9400-$97FF): GPIO Card
  - IO 7 ($9800-$9BFF): Sound Card
  - IO 8 ($9C00-$9FFF): Video Card
- $A000-$BFFF: LO System ROM (KERNAL, 8KB)
- $C000-$FFFF: HI System ROM or Cart ROM (16KB)
- Interrupt vectors ($FFFA-$FFFF)

### Keyboard Map

**File:** `Documents/Keyboard Map/Keyboard Map.md`

**Purpose:** Complete keyboard matrix layout and key mappings

**Contents:**
- 8×8 matrix layout (64 keys)
- ASCII code mappings
- Modifier key combinations (Shift, Ctrl, Alt, Fn)
- Function key assignments (F1-F12)
- Extended characters ($80-$FF)

---

**For more information, see:**
- [Main README](../README.md) - Project overview
- [Hardware README](../Hardware/README.md) - Hardware specifications

---
