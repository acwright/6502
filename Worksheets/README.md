6502 Worksheets
===============

This folder contains KiCad worksheet (title block) templates for the 6502 project. Worksheets provide consistent title blocks across all schematics and PCB layouts, including project information, revision history, and branding.

**Last Updated:** March 7, 2026

---

## Overview

KiCad worksheets define the border and title block that appear on schematics and PCB layouts. Custom worksheets ensure:
- Consistent branding (project name, logo)
- Standard metadata fields (title, date, revision, author)
- Professional documentation appearance
- Revision tracking

---

## Worksheet Contents

Worksheets (.kicad_wks files) typically include:

**Border:**
- Page outline (A4, A3, Letter, etc.)
- Alignment marks

**Title Block Fields:**
- **Title**: PCB/schematic name (e.g., "Serial Card Rev 1.0")
- **Date**: Creation/modification date
- **Revision**: Version number (Rev 1.0, Rev 1.1, etc.)
- **Author**: Designer name
- **Company/Project**: "6502 Project" or branding
- **Sheet**: Sheet number (for multi-sheet schematics)

**Logos:**
- Project logo
- Designer logo or branding

---

## Using Worksheets

### Applying to Project

**Method 1: Project Settings**

1. Open KiCad project
2. Schematic Editor or PCB Editor
3. File → Page Settings
4. Click "Browse Worksheets"
5. Select `.kicad_wks` file from this folder
6. Click OK

**Method 2: Copy to Project Folder**

1. Copy `.kicad_wks` file to project folder
2. KiCad will auto-detect and use it (if properly named)

### Updating Metadata

1. File → Page Settings
2. Update fields:
   - **Title**: Board/schematic name
   - **Date**: Current date (or auto-update)
   - **Revision**: Current revision (e.g., "1.1")
   - **Author**: Your name
3. Click OK

---

## Customizing Worksheets

### Editing Worksheet

1. Tools → Worksheet Editor (from KiCad main window)
2. File → Open → select `.kicad_wks` file
3. Modify:
   - Add/remove text fields
   - Change logo images
   - Adjust layout and positioning
   - Modify border style
4. File → Save

**Common Customizations:**
- Add company logo
- Change border thickness/style
- Add custom fields (part number, approver, etc.)
- Adjust text size/font
- Reposition title block elements

### Adding Logos

1. Prepare logo image (PNG or bitmap, high resolution)
2. In Worksheet Editor:
   - Place → Bitmap
   - Click to place
   - Right-click → Properties
   - Browse to image file
   - Adjust size and position
3. Save worksheet

---

## Best Practices

**Revision Tracking:**
- Increment revision number with each significant change
- Update date when revision changes
- Document changes in schematic notes or separate changelog

**Consistency:**
- Use same worksheet across all project boards for consistency
- Update all boards to new worksheet when revising

**Printing:**
- Verify title block legibility when printing
- Ensure logos don't interfere with schematic content
- Use appropriate page size for board complexity

---

**For more information, see:**
- [Main README](../README.md) - Project overview
- [KiCad Worksheet Editor](https://docs.kicad.org/7.0/en/kicad/kicad.html#page-layout-editor) - Official documentation

---

**Last Updated:** March 7, 2026
