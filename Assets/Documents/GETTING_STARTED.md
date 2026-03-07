Getting Started with the 6502 Project
======================================

Welcome to the 6502 Project! This guide will walk you through building your first 6502 system from scratch. We recommend starting with **The KIM** system as it's the easiest entry point.

**Last Updated:** March 7, 2026

---

## Table of Contents

- [Which System Should I Build?](#which-system-should-i-build)
- [Before You Begin](#before-you-begin)
- [Building The KIM (Recommended First Build)](#building-the-kim-recommended-first-build)
- [Building The DEV System](#building-the-dev-system)
- [Building The COB System](#building-the-cob-system)
- [First Boot Checklist](#first-boot-checklist)
- [Next Steps](#next-steps)
- [Common Beginner Mistakes](#common-beginner-mistakes)

---

## Which System Should I Build?

### The KIM - Best for Beginners ⭐ RECOMMENDED

**Why start here:**
- Single board design (Main Board only)
- No backplane or card assembly required
- Built-in keypad and LCD display
- Fully tested and documented
- Complete project in one PCB

**What you'll learn:**
- PCB assembly basics
- Through-hole soldering
- Testing and debugging
- 6502 programming fundamentals

**Time to complete:** 4-8 hours (assembly + testing)

**Cost:** ~$50-100 (PCB + components + assembly)

### The DEV - Best for Software Development

**Why choose this:**
- Cycle-accurate 6502 emulation on Teensy 4.1
- No vintage chips needed
- USB power and programming
- Serial terminal interface
- Fast iteration for firmware development

**What you'll learn:**
- Microcontroller programming (Teensy)
- 6502 emulation concepts
- Serial communication
- Firmware development workflow

**Time to complete:** 6-10 hours (2 boards + firmware)

**Cost:** ~$80-150

### The COB - Advanced Modular System

**Why choose this:**
- Maximum expandability
- Mix and match cards
- Learn about bus systems
- Professional computer architecture

**Prerequisites:**
- Experience with The KIM or similar project
- Understanding of bus protocols
- Advanced soldering skills

**Time to complete:** 20-40 hours (backplane + multiple cards)

**Cost:** ~$200-500 (depending on cards chosen)

---

## Before You Begin

### Required Tools

**Soldering Equipment:**
- [ ] Soldering iron (adjustable temperature, 300-400°C)
- [ ] Solder wire (0.8mm diameter, lead-free or 63/37 leaded)
- [ ] Soldering iron tip cleaner (brass wool or wet sponge)
- [ ] Solder wick or desoldering pump (for mistakes)
- [ ] Tip tinner (maintains soldering iron tip)

**Measurement Tools:**
- [ ] Digital multimeter (for continuity and voltage testing)
- [ ] Logic probe or logic analyzer (optional but helpful)

**Assembly Tools:**
- [ ] Needle-nose pliers
- [ ] Wire cutters (flush cut)
- [ ] IC insertion tool or small flathead screwdriver
- [ ] Tweezers (for small components)
- [ ] Magnifying glass or loupe (for inspection)

**Programming Tools:**
- [ ] EEPROM programmer (TL866II Plus, TL866A, or similar)
- [ ] USB cable for programmer
- [ ] Programmer software (Xgpro for TL866 series)

**Cleaning Supplies:
- [ ] Isopropyl alcohol (90%+ recommended)
- [ ] Soft brush or toothbrush
- [ ] Lint-free wipes or paper towels

**Power Supply:**
- [ ] 5V DC power supply (2A minimum)
- [ ] 2.1mm barrel jack connector (center positive)
- [ ] OR USB power cable (if supported by board)

### Required Knowledge

**Essential (must have):**
- Basic soldering skills (practice on scrap PCB first!)
- Reading schematics and datasheets
- Using a multimeter (continuity, voltage measurement)
- Following assembly instructions

**Helpful (nice to have):**
- 6502 assembly language basics
- Digital logic fundamentals (logic gates, flip-flops)
- Serial terminal usage (PuTTY, screen, minicom)

**Resources to learn:**
- [Learn to Solder](https://learn.adafruit.com/adafruit-guide-excellent-soldering) - Adafruit tutorial
- [6502.org](http://www.6502.org/) - 6502 programming and architecture
- [Easy 6502](https://skilldrick.github.io/easy6502/) - Interactive 6502 assembly tutorial

### Component Sourcing

**PCB Manufacturing:**
- Order from [JLCPCB](https://jlcpcb.com/) using files in `Production/<BoardName>/`
- Minimum order: 5 PCBs (costs ~$2-5 for basic boards)
- Lead time: 7-14 days (shipping included)
- See [Production README](../../Production/README.md) for ordering guide

**Components:**
- **BOM (Bill of Materials):** Located in `Production/<BoardName>/<BoardName>_BOM.csv`
- **Suppliers:**
  - Mouser Electronics (mouser.com) - Comprehensive selection
  - Digikey (digikey.com) - Fast shipping
  - LCSC (lcsc.com) - Budget-friendly, JLCPCB integration
  - eBay/AliExpress - Vintage chips (buyer beware!)

**IC Sockets (HIGHLY RECOMMENDED):**
- Use IC sockets for all ICs (CPU, RAM, ROM, support chips)
- Prevents damage from soldering heat
- Allows easy chip replacement if defective
- Types: DIP sockets (0.3" or 0.6" wide depending on IC)

---

## Building The KIM (Recommended First Build)

### Overview

The KIM system is a single-board computer with:
- ATmega1284p microcontroller (handles keyboard/LCD)
- W65C02S CPU (8-bit processor)
- 32KB RAM (AS6C62256 or similar)
- 32KB ROM (AT28C256 EEPROM)
- 4x5 matrix keypad
- 16x2 LCD character display
- Serial port (for terminal communication)

**Total Components:** ~50-70 parts

### Step 1: Order PCB and Components

**PCB:**
1. Go to [JLCPCB.com](https://jlcpcb.com/)
2. Upload `Production/Main Board/Main Board_Gerber.zip`
3. Select options:
   - Quantity: 5 or 10
   - PCB Color: Your choice (green is cheapest)
   - Surface Finish: HASL (lead-free)
   - **SMD Assembly:** Optional (recommended for SMD components)
4. If using SMD assembly:
   - Upload `Main Board_BOM.csv` and `Main Board_CPL.csv`
   - Review component placement
5. Order (including shipping: ~$20-40)

**Components:**
1. Download BOM: `Production/Main Board/Main Board_BOM.csv`
2. Create shopping cart at Mouser or Digikey
3. Add all components from BOM
4. Add IC sockets:
   - 1× 40-pin DIP socket (W65C02S CPU)
   - 1× 40-pin DIP socket (ATmega1284p)
   - 1× 28-pin DIP socket (RAM)
   - 1× 28-pin DIP socket (ROM)
   - Additional sockets as needed for other ICs
5. Order (total: ~$40-80 depending on component sources)

**Keypad and LCD:**
- 4x5 matrix keypad (membrane or mechanical)
- 16x2 LCD character display (HD44780 compatible)
- Available from Amazon, Adafruit, or electronics suppliers

### Step 2: Inspect PCB

When PCB arrives:
1. **Visual check:**
   - No cracks or damage
   - Silkscreen legible
   - Correct board (check name on silkscreen)
   
2. **SMD assembly check (if ordered):**
   - All components placed
   - Correct orientation (check IC pin 1 markers)
   - No solder bridges (especially between IC pins)
   
3. **Electrical check:**
   - Multimeter in continuity mode
   - Test 5V power rail to GND (should NOT beep = no short)
   - If short detected (<100Ω), inspect for solder bridges

### Step 3: Assemble Through-Hole Components

**Recommended order (shortest to tallest):**

**A. Resistors and diodes (30 minutes)**
1. Identify resistor values (color code or markings)
2. Insert resistors into PCB holes (orientation doesn't matter for non-polarized)
3. Bend leads on bottom to hold in place
4. Solder on bottom of board
5. Clip excess leads with flush cutters
6. **For diodes:** Match stripe on diode to stripe on silkscreen (polarity matters!)

**B. Ceramic capacitors (15 minutes)**
1. Insert capacitors (non-polarized, orientation doesn't matter)
2. Solder and clip leads

**C. IC sockets (45 minutes)**
1. **Critical:** Match notch on socket to notch on silkscreen
2. Align socket carefully (all pins in holes)
3. Tape socket in place or use helping hands
4. Solder one corner pin
5. Check alignment, reheat and adjust if needed
6. Solder remaining pins
7. Inspect for cold solder joints (dull, grainy = bad; shiny, smooth = good)

**D. Crystal oscillator (5 minutes)**
1. Insert crystal (Y1)
2. Solder (orientation doesn't matter for 2-pin crystals)

**E. Electrolytic capacitors (15 minutes)**
1. **Critical:** Match polarity! Long lead = positive
2. Check silkscreen for + marking
3. Insert capacitors
4. Solder and clip leads

**F. Connectors and headers (30 minutes)**
1. Power jack (barrel connector)
2. Serial connector (DB9 or pin header)
3. LCD header (16-pin)
4. Keypad connector (20-pin)
5. Programming header (if present)

**G. Switches and buttons (10 minutes)**
1. Reset button
2. Power switch (if present)

**Total assembly time:** ~2.5 hours

### Step 4: Clean and Inspect

1. **Clean flux residue:**
   - Apply isopropyl alcohol with brush
   - Scrub gently around solder joints
   - Wipe with lint-free cloth
   - Let dry completely (5-10 minutes)

2. **Visual inspection (with magnification):**
   - All components soldered
   - No solder bridges between pins/pads
   - No cold solder joints (reheat any suspicious joints)
   - Component orientation correct (especially ICs, electrolytic caps, diodes)

### Step 5: Pre-Power Testing

**Before inserting ICs, test power rails:**

1. **Resistance test:**
   - Multimeter in resistance mode (Ω)
   - Measure between 5V and GND
   - Should read >10kΩ (if <100Ω, there's likely a short)

2. **Visual double-check:**
   - IC sockets oriented correctly (notch matches silkscreen)
   - Power connector polarity correct (center positive for barrel jack)

### Step 6: First Power-On (WITHOUT ICs)

**Safety first: NO ICs installed yet!**

1. Connect 5V power supply (do NOT turn on yet)
2. Set multimeter to DC voltage (20V range)
3. **Turn on power supply**
4. Measure voltage between 5V test point (or IC socket VCC pin) and GND
   - **Expected:** 4.75V - 5.25V
   - **If wrong voltage:** Power off immediately, check power supply
5. Verify voltage at multiple IC socket VCC pins (should all be ~5V)
6. **Power off**

### Step 7: Insert ICs

**Order matters! Start with least critical, end with CPU:**

1. **ATmega1284p (keyboard/LCD controller):**
   - Verify chip orientation (pin 1 dot/notch matches socket notch)
   - Gently bend pins inward (on flat surface) if too wide
   - Align all pins with socket holes
   - Press firmly but gently until fully seated
   - **Double-check pin 1 orientation!**

2. **RAM (AS6C62256):**
   - Same insertion procedure
   - Check pin 1 orientation

3. **ROM (AT28C256 - if pre-programmed):**
   - Same insertion procedure
   - If blank, you'll need to program it first (see firmware section)

4. **W65C02S CPU:**
   - **Triple-check orientation** (CPU is expensive!)
   - Insert carefully
   - Verify all pins seated

5. **Other ICs** (logic chips, VIA, etc.):
   - Insert in any order
   - Always check orientation

### Step 8: Program ROM (if not pre-programmed)

**Requires TL866 programmer or similar:**

1. Download BIOS/monitor from `Firmware/` folder (`.bin` or `.hex` file)
2. Insert blank EEPROM into programmer
3. Use programmer software (Xgpro for TL866):
   - Select chip type: AT28C256 or compatible
   - Load firmware file
   - **Program** (write to chip)
   - **Verify** (confirm correct programming)
4. Insert programmed ROM into board

### Step 9: Program ATmega1284p

**Using Arduino IDE or PlatformIO:**

1. Open `Firmware/KEH Controller/` project
2. Connect ISP programmer (USBasp, Arduino as ISP, etc.) to programming header
3. Select board: ATmega1284p
4. Select programmer type
5. **Upload** firmware
6. Disconnect programmer

**Alternatively:** Order PCBs with pre-programmed ATmega from assembly service

### Step 10: Final Power-On and Testing

1. Connect LCD to LCD header
2. Connect keypad to keypad connector
3. Connect 5V power supply
4. **Power on**

**Expected behavior:**
- Power LED illuminates (if present)
- LCD shows startup message or prompt
- CPU begins executing ROM code

**If no LCD output:**
- Check LCD contrast potentiometer (adjust with screwdriver)
- Verify LCD connections (ribbon cable seated fully)
- Check ATmega firmware uploaded correctly

**If system appears dead:**
- See [Troubleshooting Guide](TROUBLESHOOTING.md)
- Check all IC orientations (especially CPU!)
- Verify power supply voltage (5V)

### Step 11: First Program

**Using keypad (KIM-1 style monitor):**
1. Press keys to enter addresses and data
2. See keyboard map in [Assets/Documents/Keyboard Map/](../Keyboard%20Map/)
3. Write simple program (e.g., loop that increments a register)
4. Execute and observe

**Using serial terminal:**
1. Connect serial cable (USB-to-serial adapter)
2. Open terminal (PuTTY, screen, minicom)
3. Settings: 9600 baud, 8N1
4. Type commands to monitor/BIOS
5. Load and run programs

---

## Building The DEV System

### Overview

The DEV system consists of:
- **Dev Board:** Teensy 4.1 running 6502 emulator
- **Dev Output Board:** VGA output via Raspberry Pi Pico

**Total time:** 6-10 hours (both boards + firmware)

### Step-by-Step Guide

**1. Order PCBs:**
- Dev Board: `Production/Dev Board/`
- Dev Output Board: `Production/Dev Output Board/`

**2. Order Components:**
- Teensy 4.1 (without headers or with headers pre-soldered)
- Raspberry Pi Pico
- Components from BOM files
- VGA connector
- Power supply (5V 2A via USB or barrel jack)

**3. Assemble Dev Board:**
- Solder Teensy 4.1 socket or headers
- Solder remaining through-hole components
- Insert Teensy 4.1

**4. Assemble Dev Output Board:**
- Solder Raspberry Pi Pico socket or headers
- Solder VGA connector
- Solder remaining components
- Insert Pico

**5. Program Firmware:**

**Teensy 4.1 (Dev Board):**
```bash
cd Firmware/DB\ Emulator/
pio run -t upload
```

**Raspberry Pi Pico (Dev Output Board):**
```bash
cd Firmware/DOB\ Terminal/
pio run -t upload
```

**6. Connect Boards:**
- Use included cable or jumper wires
- Connect Dev Board output header to Dev Output Board input header
- Pin mapping in Hardware README

**7. Connect Display:**
- VGA monitor to Dev Output Board VGA port
- Or serial terminal to Dev Board USB (for text output)

**8. Power On:**
- USB power to Teensy 4.1
- System should boot and display on VGA
- Serial console available at 115200 baud

**9. Test:**
- System boots to monitor prompt
- Run demo programs (included in firmware)
- Write simple 6502 assembly programs

---

## Building The COB System

### Overview

The COB (Computer On Backplane) is an advanced modular system. **Not recommended for first build** unless you have prior experience.

### Recommended Build Order

**Phase 1: Core System (Minimal Bootable)**
1. Backplane or Backplane Pro
2. CPU Card
3. Memory Card (RAM + ROM)
4. Serial Card (for terminal communication)

**Phase 2: Add I/O**
5. GPIO Card (for peripherals)
6. Video Card or VGA Card (for display)

**Phase 3: Expansion**
7. Sound Card
8. Storage Card
9. Additional cards as desired

### Build Process

**For each card:**
1. Order PCB and components (see Production README)
2. Assemble through-hole components (same process as KIM)
3. Insert into backplane slot
4. Verify card-edge connector alignment
5. Power on and test individual card
6. Add next card

**Critical:**
- **One card at a time!** Don't insert all cards at once
- Test after each card addition
- Check for address conflicts (no two cards at same I/O address)
- Verify backplane slot jumpers configured correctly

---

## First Boot Checklist

**⚠️ Before powering on any system for the first time:**

- [ ] All ICs inserted with correct orientation (pin 1 matched)
- [ ] No solder bridges visible (inspect under magnification)
- [ ] Power supply correct voltage (5V DC)
- [ ] Power supply correct polarity (center positive for barrel jack)
- [ ] Resistance between 5V and GND >10kΩ (no short circuit)
- [ ] ROM programmed with firmware/BIOS
- [ ] Microcontrollers programmed with firmware (if applicable)
- [ ] All connectors seated fully (displays, keyboards, cables)
- [ ] Safety glasses on (in case of component failure)
- [ ] Fire extinguisher nearby (paranoid but safe!)

**When powered on:**

- [ ] No smoke or burning smell (if present, power off immediately!)
- [ ] No excessive heat from components (touch gently)
- [ ] Power LED illuminated (if present)
- [ ] Expected voltage present at all IC VCC pins (~5V)
- [ ] Clock signal present (measure with oscilloscope or logic probe)
- [ ] Display shows output (or serial terminal receives data)

---

## Next Steps

### Learn 6502 Programming

**Resources:**
- [6502.org](http://www.6502.org/) - Reference, tutorials, forums
- [Easy 6502](https://skilldrick.github.io/easy6502/) - Interactive browser tutorial
- [6502 Assembly Book](http://www.6502.org/books/) - Classic references

**First Programs:**
1. Blinking LED (control GPIO output)
2. Hello World (serial output)
3. Keystroke echo (read keyboard, display character)
4. Simple calculator
5. Graphics demo (if video card present)

### Expand Your System

**KIM System:**
- Add serial card for better terminal interface
- Add expansion connector for peripherals
- Design custom helper boards

**DEV System:**
- Write custom ROMs
- Add networking features (Teensy supports Ethernet)
- Port vintage software

**COB System:**
- Add more cards (sound, storage, video)
- Design custom cards for specialized functions
- Build complete retro computer workstation

### Join the Community

- GitHub Discussions: Ask questions, share builds
- Submit pull requests: Fix bugs, improve docs, contribute designs
- Share photos: Show off your builds!

---

## Common Beginner Mistakes

### Assembly Errors

**❌ Inserting ICs backwards**
- **Symptom:** No boot, sometimes component damage
- **Prevention:** Always triple-check pin 1 orientation BEFORE inserting
- **Fix:** Remove IC (carefully!), check if damaged, reinsert correctly

**❌ Solder bridges (shorts between pins)**
- **Symptom:** Erratic behavior, non-functional circuits
- **Prevention:** Use magnification, inspect after soldering each IC
- **Fix:** Remove excess solder with solder wick

**❌ Cold solder joints**
- **Symptom:** Intermittent connections, unreliable operation
- **Prevention:** Heat pad and component lead simultaneously, use adequate temperature (320-350°C)
- **Fix:** Reheat joints until solder flows smoothly

**❌ Wrong component values**
- **Symptom:** Circuit doesn't work as expected
- **Prevention:** Double-check resistor color codes, capacitor markings against BOM
- **Fix:** Desolder incorrect component, replace with correct value

### Power Issues

**❌ Wrong power supply polarity**
- **Symptom:** Immediate damage to components (magic smoke!)
- **Prevention:** Verify center positive on barrel jack, check voltage before connecting
- **Fix:** Replace damaged components (regulators, ICs)

**❌ Insufficient power supply current**
- **Symptom:** Voltage drops under load, system resets randomly
- **Prevention:** Use 2A minimum power supply, check current requirements in docs
- **Fix:** Upgrade to higher-current power supply

### Testing Errors

**❌ Powering on with ICs inserted before testing power rails**
- **Symptom:** Damaged ICs if power supply wrong voltage or shorted
- **Prevention:** ALWAYS test power rails WITHOUT ICs first
- **Fix:** Replace damaged ICs, fix power issue

**❌ Not programming ROM before inserting**
- **Symptom:** CPU executes random data, unpredictable behavior
- **Prevention:** Program ROM first, verify before inserting
- **Fix:** Remove ROM, program, reinsert

**❌ Skipping visual inspection**
- **Symptom:** Hard-to-diagnose issues from solder bridges or wrong components
- **Prevention:** Inspect thoroughly with magnification after assembly
- **Fix:** Find and fix issues (takes much longer than prevention!)

---

## Troubleshooting

For detailed troubleshooting, see:
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Comprehensive troubleshooting guide
- [Main README](../../README.md#troubleshooting) - Quick troubleshooting section
- [Hardware README](../../Hardware/README.md) - Hardware-specific notes

---

**Good luck with your build! Join the community on GitHub if you need help.**

**Last Updated:** March 7, 2026
