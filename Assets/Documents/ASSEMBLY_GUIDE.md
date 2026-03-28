6502 Physical Assembly Guide
============================

Detailed physical assembly instructions with diagrams, measurements, and best practices for building 6502 systems. Covers PCB assembly, card installation, cabling, enclosures, and testing.

---

## Table of Contents

- [Required Tools and Materials](#required-tools-and-materials)
- [PCB Assembly](#pcb-assembly)
- [Component Identification](#component-identification)
- [Soldering Techniques](#soldering-techniques)
- [System Assembly](#system-assembly)
- [Cable Assembly](#cable-assembly)
- [Enclosure Assembly](#enclosure-assembly)
- [Final Testing](#final-testing)

---

## Required Tools and Materials

### Essential Tools

**Soldering Station:**
- **Temperature-controlled soldering iron** (recommended: Hakko FX-888D, Weller WES51)
- Temperature range: 300-400°C (575-750°F)
- Fine tip (chisel or conical, 1-2mm)
- Tip cleaner (brass wool preferred over wet sponge)
- Soldering iron stand

**Hand Tools:**
- Flush-cut side cutters (for trimming component leads)
- Needle-nose pliers (long nose)
- Wire strippers (for 22-26 AWG wire)
- Precision screwdriver set (Phillips and flathead)
- IC insertion/extraction tool
- Tweezers (fine tip, anti-static preferred)
- Helping hands or PCB holder

**Measurement Tools:**
- Digital multimeter (essential)
  - Continuity tester
  - DC voltage measurement
  - Resistance measurement
- Logic probe (optional but helpful)
- Oscilloscope (optional, for advanced debugging)

**Optical Tools:**
- Magnifying glass (5-10x magnification)
- OR headband magnifier
- OR USB microscope (for detailed inspection)
- Good lighting (LED desk lamp with adjustable arm)

### Programming Tools

**EEPROM Programmer:**
- **TL866II Plus** (recommended, supports wide range of chips)
- **TL866A** (older model, still widely used)
- **Alternatives:** MiniPro, GQ-4X, or other universal programmers
- **Supported chips:** AT28C256, AT28C64, W27C512, SST39SF040, etc.
- **Software:** Xgpro (Windows), minipro CLI (Linux/macOS)

**Note:** Required for programming ROM chips (EEPROM/EPROM) on Main Board, Memory Card, and ROM Carts. Not needed for systems using only pre-programmed ROMs or firmware-based solutions (e.g., DEV system).

### Soldering Materials

**Solder:**
- **Lead-free**: SAC305 (96.5% Sn, 3% Ag, 0.5% Cu), 0.8mm diameter
  - Melting point: 217-220°C
  - Requires higher temperature (330-350°C)
- **Leaded** (if preferred/legal): 63/37 Sn/Pb, 0.8mm diameter
  - Melting point: 183°C
  - Lower temperature (300-320°C)
  - Easier for beginners (wets faster, shinier joints)

**Flux:**
- Rosin flux pen (for difficult joints)
- Liquid flux (for drag soldering SMD components)
- No-clean flux (minimal residue)

**Solder Wick:**
- Copper braid for removing excess solder
- 2-3mm width
- Apply flux to wick before use

**Cleaning:**
- Isopropyl alcohol (IPA), 90%+ purity
- Flux remover spray (optional, for stubborn residue)
- Soft-bristle brush or toothbrush
- Lint-free wipes or paper towels

### Safety Equipment

- **Safety glasses** (always wear when soldering or cutting)
- **Fume extractor** or work in well-ventilated area
- **Anti-static wrist strap** (ground to prevent ESD damage to ICs)
- **Heat-resistant work surface** (silicone mat)

### Component Materials

**IC Sockets (HIGHLY RECOMMENDED):**
- Purchase for all ICs (CPU, RAM, ROM, support chips)
- Types:
  - Machine pin (gold-plated, best quality)
  - Turned pin (standard quality)
  - DIP sizes: 8, 14, 16, 20, 24, 28, 40 pin
  - Wide vs. narrow body (0.3" vs. 0.6" row spacing)

**Standoffs and Hardware:**
- M2.5 or #4-40 standoffs (for board mounting)
- Heights: 10mm (typical for card spacing)
- Screws: matching thread
- Nylon washers (insulate from enclosure if metal)

**Wire:**
- 22 AWG stranded (for power connections)
- 26 AWG solid (for jumpers, signals)
- Ribbon cable (for multi-wire connections)
- Colors: Red (5V), Black (GND), other colors (signals)

**Connectors:**
- IDC ribbon cable connectors (for card-to-card)
- Crimp pins and housings (Dupont-style for jumpers)
- DB9/DB25 connectors (for serial)
- VGA connector (if video output)
- USB cables (for power and programming)

---

## PCB Assembly

### Pre-Assembly Inspection

**Before soldering anything:**

1. **Visual inspection:**
   - [ ] No physical damage (cracks, warped board)
   - [ ] No missing pads or traces
   - [ ] Silkscreen legible
   - [ ] Correct board received (check name on silkscreen)

2. **Electrical check (bare PCB):**
   - [ ] Multimeter in continuity mode
   - [ ] Check power planes (5V to GND should NOT beep)
   - [ ] Verify key traces (address bus, data bus continuity)

### Assembly Order: SMD Components First

**If PCB has SMD components and NOT assembled by JLCPCB:**

**Smallest to largest:**
1. Resistors and capacitors (0805, 0603, etc.)
2. SOT-23 transistors
3. SOIC/TSSOP ICs
4. Larger SMD components

**Soldering SMD components:**

**Method 1: Soldering iron (for individual components)**
1. Apply flux to pads
2. Tin one pad (apply small amount of solder)
3. Place component with tweezers
4. Heat tinned pad, slide component into position
5. Solder opposite side
6. Re-heat first side to ensure good joint

**Method 2: Hot air rework station (for multiple components)**
1. Apply solder paste to pads (use syringe or stencil)
2. Place components
3. Heat with hot air (around 250-300°C) until solder reflows
4. Let cool

**Method 3: Drag soldering (for IC pins)**
1. Align IC on pads, tack down one corner pin
2. Apply flux liberally across all pins
3. Load iron tip with solder
4. Drag tip across row of pins (flux and surface tension separate joints)
5. Inspect for bridges, remove with solder wick if needed

### Assembly Order: Through-Hole Components

**General rule: Shortest components first, tallest last.**

This allows PCB to lay flat for each soldering step.

**Detailed order:**

**1. Resistors (15-30 minutes depending on count)**
- Identify value (color code or marking)
- Bend leads to fit hole spacing (typically 0.4" or 0.5")
- Insert from component side (top of PCB)
- Bend leads slightly on solder side to hold in place
- Solder all resistors
- Clip leads flush with flush-cut side cutters
- Orientation: does NOT matter (non-polarized)

**Color code reference:**
```
Black  = 0   Green  = 5
Brown  = 1   Blue   = 6
Red    = 2   Violet = 7
Orange = 3   Gray   = 8
Yellow = 4   White  = 9

Multiplier:  Brown=×10, Red=×100, Orange=×1K, Yellow=×10K, etc.
Tolerance:   Gold=±5%, Silver=±10%

Example: Brown-Black-Red-Gold = 1, 0, ×100 = 1000Ω ±5% (1KΩ)
```

**2. Diodes (5-10 minutes)**
- **CRITICAL: Polarity matters!**
- Match cathode stripe on diode to stripe on silkscreen
- Cathode = negative end (current flows from anode to cathode)
- Insert, solder, clip

**3. Ceramic capacitors (10-20 minutes)**
- Non-polarized (orientation does NOT matter)
- Insert, solder, clip
- Typical values: 100nF (0.1µF), 10nF, 1nF, etc.
- Markings: "104" = 10×10^4 pF = 100,000pF = 100nF = 0.1µF

**4. Film capacitors (if present)**
- Non-polarized
- Insert, solder, clip

**5. IC sockets (30-60 minutes)**
- **CRITICAL: Orientation matters!**
- Match notch/dot on socket to notch on silkscreen
- Align all pins with holes (check carefully!)
- **Tape socket in place** (from top) to hold during soldering
- OR use helping hands
- Solder **one corner pin** first
- Check alignment (reheat and adjust if needed)
- Solder **opposite corner pin**
- Solder remaining pins
- Remove tape

**Socket soldering tip:**
- Heat pad and socket pin simultaneously (1-2 seconds)
- Feed solder to joint (not to iron tip)
- Good joint: shiny, concave fillet around pin
- Bad joint: dull, blobby, or insufficient solder

**6. Crystals/Oscillators (5 minutes)**
- 2-pin crystal: non-polarized
- 4-pin oscillator can: match pin 1 (dot/notch) to silkscreen
- Insert, solder (do NOT overheat! Max 2-3 seconds per pin)

**7. Electrolytic capacitors (15-30 minutes)**
- **CRITICAL: Polarity matters!**
- Long lead = positive (anode)
- Short lead = negative (cathode, marked with stripe)
- Match to silkscreen (usually + marked, or filled circle = positive hole)
- Insert, solder, clip
- Typical values: 10µF, 100µF, 1000µF
- Voltage rating: must exceed circuit voltage (use ≥16V for 5V circuit)

**8. Voltage regulators (if present)**
- Match tab orientation to silkscreen outline
- Insert, solder
- May need heat sink (if >500mA current)

**9. Connectors and headers (30-60 minutes)**

**Pin headers:**
- Break to correct length (for IDC, programming headers)
- Insert, ensure perpendicular to board
- Solder one pin, check alignment
- Solder remaining pins

**Barrel jack (power connector):**
- Insert (only fits one way)
- Solder all pins/tabs
- Verify polarity marking (center positive typically)

**DB9/DB25 connectors:**
- Insert, align with board edge
- Solder pins
- May have mounting tabs (solder for mechanical strength)

**IDC shrouded headers:**
- Match notch to silkscreen
- Ensures cable cannot be inserted backward

**10. Switches and buttons (10 minutes)**
- Reset button (tactile switch)
- Power switch (if present)
- Insert, solder

**11. LEDs (5-10 minutes)**
- **CRITICAL: Polarity matters!**
- Long lead = anode (positive)
- Short lead = cathode (negative)
- Flat side of LED = cathode
- Match to silkscreen (usually square pad = anode, round = cathode)
- Insert, solder, clip
- May need to bend leads to mount flush or at angle

**12. Large/tall components**
- Transformers (if present)
- Large inductors
- Heat sinks

### Post-Assembly Inspection

**After soldering all components:**

1. **Visual inspection (with magnification):**
   - [ ] All components soldered
   - [ ] All solder joints shiny (not dull/cold)
   - [ ] No solder bridges between adjacent pins/pads
   - [ ] No unsoldered pins ("missed pins")
   - [ ] Correct component orientation (ICs, diodes, electrolytic caps, LEDs)
   - [ ] No components touching (shorts)

2. **Clean flux residue:**
   - Apply isopropyl alcohol with brush
   - Scrub around solder joints (gentle)
   - Wipe with lint-free wipe
   - Repeat until clean
   - Let dry (IPA evaporates quickly)

3. **Electrical check:**
   - [ ] Power rails (5V to GND): >10kΩ (no shorts)
   - [ ] IC socket pins: continuity to connected traces
   - [ ] Connectors: pins not shorted together

**DO NOT INSERT ICs YET!**

---

## Component Identification

### Reading Resistor Color Codes

**4-band resistors:**
- Band 1: First digit
- Band 2: Second digit
- Band 3: Multiplier
- Band 4: Tolerance

**5-band resistors (precision):**
- Band 1-3: Three digits
- Band 4: Multiplier
- Band 5: Tolerance (brown=±1%, red=±2%)

**Surface mount resistors:**
- 3-digit: 103 = 10×10^3 = 10KΩ
- 4-digit: 1002 = 100×10^2 = 10KΩ

### Reading Capacitor Markings

**Ceramic (small, usually brown/yellow/blue):**
- 104 = 10×10^4 pF = 100nF = 0.1µF
- 103 = 10×10^3 pF = 10nF
- 105 = 10×10^5 pF = 1µF

**Electrolytic (cylindrical, polarized):**
- Printed directly: "100µF 16V"
- Stripe = negative side

**Film capacitors:**
- Printed directly (e.g., "0.1µF", "100nF")

### IC Identification

**Reading IC part numbers:**
```
Example: W65C02S6TPG-14

W65C02S = Part family (Western Design Center 65C02)
6 = Speed grade (6 MHz)
TP = Package type (PDIP)
G = Green (RoHS compliant)
-14 = Manufacturer code
```

**IC orientation:**
- **Notch**: Semicircular cutout at one end → pin 1 is to the left of notch (when notch at top)
- **Dot**: Small circle near pin 1 corner
- **Pin numbering**: Counter-clockwise from pin 1 (U-shape)

```
    _____
   |  U  |    Pin 1 (marked)
 1 |     | 8
 2 |     | 7
 3 |     | 6
 4 |_____| 5
   
```

---

## Soldering Techniques

### Perfect Solder Joint

**Characteristics:**
- **Shiny** (not dull or grainy)
- **Concave fillet** (smooth curve from pad to component lead)
- **Complete coverage** (solder flows onto both pad and lead)
- **No excess solder** (not blobby)

**Defects:**

| Defect | Appearance | Cause | Fix |
|--------|-----------|-------|-----|
| Cold joint | Dull, grainy, weak | Insufficient heat/time | Reheat until shiny |
| Insufficient solder | Thin, gaps | Too little solder | Add more solder |
| Excess solder | Large blob | Too much solder | Remove with wick |
| Solder bridge | Connects adjacent pins | Too much solder or movement | Remove with wick |
| Lifted pad | Pad separated from board | Excessive heat/force | Difficult, may need jumper wire |

### Through-Hole Soldering Steps

1. **Heat the joint** (pad AND component lead simultaneously)
   - Place iron tip touching both pad and lead
   - Wait 1-2 seconds (heat flows into joint)

2. **Feed solder to joint** (NOT to iron tip)
   - Touch solder wire to joint (near iron tip but on pad/lead)
   - Solder melts and flows onto heated surfaces
   - Apply enough to form good fillet (1-2mm of solder wire)

3. **Remove solder wire** (stop feeding)

4. **Remove iron** (after ~1 second more)
   - Joint should look shiny as it cools

5. **Don't move component** until solder solidifies (1-2 seconds)

**Total time per joint: 3-5 seconds**

### SMD Soldering Tips

**For small SMD resistors/capacitors:**
- Use fine-tip iron (0.5-1mm chisel or conical)
- Apply flux to pads
- Tin one pad
- Place component with tweezers, heat tinned pad
- Solder opposite end
- Reheat first side for good joint

**For SMD ICs:**
- Align IC on pads, tack one corner pin
- Check alignment (view from multiple angles)
- Solder opposite corner pin
- Use drag soldering for remaining pins (see earlier section)

### Desoldering

**Removing component:**

**Method 1: Solder sucker (desoldering pump)**
1. Heat joint with iron
2. When solder melts, quickly place sucker tip over joint
3. Press button to suck molten solder
4. Repeat for each pin
5. Component should lift out

**Method 2: Solder wick (braid)**
1. Place wick over joint
2. Heat wick with iron (solder wicks into braid)
3. Remove wick and iron together
4. Cut off used portion of wick
5. Repeat until solder removed

**For ICs (many pins):**
- Use both methods (wick to remove bulk, sucker for cleanup)
- OR use low-melt solder (ChipQuik) - add to joints, lowers melting point, all pins melt together
- OR use hot air (heat all pins simultaneously)
- **Be patient!** Rushing damages pads.

---

## System Assembly

### Backplane System (COB)

**Step 1: Prepare Backplane**
1. Assemble backplane PCB (follow PCB assembly section)
2. Configure slot jumpers (if present):
   - Set address ranges for each slot
   - Set bus loading (termination resistors if needed)
3. Mount backplane in enclosure or on base
4. Secure with standoffs and screws

**Step 2: Install Cards (One at a Time!)**

**CRITICAL: Add one card at a time, test after each addition.**

1. **First card: CPU Card**
   - Inspect card edge connector (gold fingers clean, no damage)
   - Align card with backplane slot
   - Insert vertically, apply even pressure
   - Card should seat fully (connector bottomed out)
   - Secure with bracket screw (if present)
   - **Test:** Power on, verify CPU clock running (oscilloscope recommended)

2. **Second card: Memory Card**
   - Insert RAM and ROM chips (sockets)
   - Ensure ROM pre-programmed
   - Insert card into backplane
   - **Test:** Power on, CPU should fetch from ROM (use logic analyzer or monitor address bus activity)

3. **Third card: Serial Card**
   - Configure address jumpers (typically $C100-$C1FF)
   - Verify no conflict with other cards
   - Insert into backplane
   - Connect serial cable to terminal/computer
   - **Test:** Monitor program should output to terminal

4. **Subsequent cards:**
   - Add one at a time
   - Verify address ranges (no conflicts!)
   - Test after each addition
   - If system fails, remove last card and troubleshoot

**Step 3: Cable Management**
- Route power cables neatly
- Secure with zip ties or cable channels
- Keep signal cables away from power cables (reduce interference)
- Label cables (especially if multiple identical connectors)

### Single-Board System (KIM, VCS)

**Step 1: Assemble PCB**
- Follow PCB assembly section

**Step 2: Prepare External Components**
- Keypad (if separate): Connect ribbon cable
- LCD: Connect to LCD header (may use ribbon cable or pin headers)
- Serial cable: DB9 or USB-to-serial adapter

**Step 3: Mounting**
- Use standoffs to mount board in enclosure or on base
- 10mm standoffs typical (provides clearance for solder joints underneath)
- Ensure no shorts to conductive surfaces

**Step 4: Connect Peripherals**
- LCD: Align connector, press firmly
- Keypad: Connect ribbon cable (note orientation: red stripe = pin 1)
- Serial: DB9 connector or USB cable

**Step 5: Power Connection**
- 5V barrel jack (center positive) OR
- USB power cable
- **Double-check polarity before connecting!**

### DEV System

**Step 1: Assemble Both Boards**
- Dev Board (Teensy 4.1)
- Dev Output Board (Teensy 4.0)

**Step 2: Program Firmware**
- Before connecting boards, program both
- Teensy 4.1: Upload DB Emulator firmware
- Teensy 4.0: Upload DOB Display firmware

**Step 3: Connect Boards**
- Verify all connections before powering on

**Step 4: Connect USB**
- USB cable from Dev Board to computer (power + serial terminal)

**Step 5: Power On**
- Single USB power to Teensy sufficient (powers both boards via cable)

---

## Cable Assembly

### IDC Ribbon Cable

**Tools required:**
- IDC ribbon cable (flat multi-conductor)
- IDC connectors (shrouded header)
- Bench vise or cable crimper

**Steps:**
1. Cut ribbon cable to length (add extra for bends/routing)
2. Identify pin 1 (red stripe or marked edge)
3. Position connector:
   - Align pin 1 of connector with red stripe
   - Cable sits in connector channel
4. Close connector (squeeze with vise or crimper)
   - Metal teeth bite through cable insulation, make contact
5. Verify continuity with multimeter
6. Repeat for other end of cable

**Keying:**
- Use shrouded connectors (prevent backward insertion)
- Cut off pin 10 (or other pin) to physically prevent wrong orientation

### Serial Cable (DB9)

**For manual assembly:**
1. DB9 connector kit (solder-cup type)
2. Solder wires to appropriate pins:
   ```
   Pin 2: RX (receive data)
   Pin 3: TX (transmit data)
   Pin 5: GND (ground)
   (Other pins optional: RTS, CTS, etc.)
   ```
3. Assemble connector housing
4. Strain relief with cable clamp

**Easier: Buy pre-made cables**
- USB-to-serial adapter (FTDI, Prolific, etc.)
- Verify driver support for your OS

### Power Cables

**Barrel jack to PCB:**
- Red wire: +5V (center pin of barrel jack)
- Black wire: GND (outer sleeve)
- Solder to PCB pads or terminal block
- Add fuse in +5V line (1-2A for safety)

**Molex/JST connectors:**
- Use crimp tool for proper pins
- Insert crimped pins into housing
- Verify orientation (connector keyed or labeled)

---

## Enclosure Assembly

### 3D Printed Enclosures

**Step 1: Print Parts**
- Use STL files from CAD folder
- Recommended settings:
  - PLA or PETG
  - 0.2mm layer height
  - 20% infill
  - 3-4 perimeters
  - Supports as needed

**Step 2: Post-Processing**
- Remove supports
- Sand edges (200→400 grit)
- Install threaded inserts (heat with soldering iron, press into holes)

**Step 3: Mount PCB**
- Use standoffs (M2.5 or M3)
- Secure PCB to bottom shell
- Verify clearances (no components touching enclosure)

**Step 4: Route Cables**
- Connector cutouts in enclosure
- Route power, video, serial cables through cutouts
- Secure with strain relief

**Step 5: Attach Top Shell**
- Align screw holes
- Secure with screws (don't overtighten plastic!)

### Laser-Cut Acrylic Enclosures

**Step 1: Cut Panels**
- Use DXF files from CAD folder
- 3mm acrylic typical
- Laser-cut or CNC router

**Step 2: Assemble Panels**
- Slot-together design OR
- Screw/bolt assembly
- Use acrylic solvent cement for permanent joints

**Step 3: Mount PCB**
- Standoffs attached to bottom panel
- Nylon or metal (use insulating washers if metal)

**Step 4: Final Assembly**
- Attach top panel
- Install any front-panel controls (buttons, LEDs)
- Label connectors (laser-engrave or vinyl labels)

---

## Final Testing

### Power-On Checklist

**Before first power-on:**
- [ ] All ICs inserted with correct orientation
- [ ] No loose screws or metal debris on/near PCB
- [ ] All cables connected
- [ ] Power supply set to correct voltage (5V)
- [ ] Power supply polarity correct
- [ ] Multimeter ready for voltage checks

**Initial power-on (no ICs):**
1. Connect power (don't turn on yet)
2. Measure voltage at power connector with multimeter
3. Turn on power
4. Verify 5V at test points or IC socket VCC pins
5. If wrong voltage, power off immediately and troubleshoot
6. Turn off power

**Power-on with ICs:**
1. Insert all ICs (verify orientation one last time!)
2. Turn on power
3. Immediately check for:
   - [ ] No smoke
   - [ ] No burning smell
   - [ ] No excessive heat (touch ICs gently - should be warm, not hot)
4. Verify expected behavior (LED blink, display output, etc.)

### Functional Tests

**Test 1: Clock Signal**
- Use oscilloscope or logic probe
- Measure PHI2 signal (CPU clock)
- Expected: Square wave at design frequency (e.g., 1 MHz)
- If no clock or wrong frequency, check crystal/oscillator circuit

**Test 2: Reset**
- Press reset button
- System should restart (reset vector fetched, execution resumes)
- Monitor should print startup message (if serial output)

**Test 3: Memory Access**
- Write data to RAM address
- Read back, verify correct data
- Example (in Wozmon):
  ```
  0200: 42    (Write $42 to address $0200)
  0200        (Read from $0200, should return $42)
  ```

**Test 4: I/O**
- Read/write to peripheral register
- Example: Toggle GPIO pin, verify with LED or multimeter

**Test 5: Program Execution**
- Load simple program (e.g., count to 10, output to serial)
- Execute, verify correct output

### Troubleshooting Failed Tests

See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for detailed diagnostic procedures.

**Quick checks:**
- **No power LED:** Check power supply, power switch, LED orientation
- **No display:** Check contrast pot (LCD), check firmware upload, check connections
- **No serial output:** Check baud rate, cable connections, ACIA registers
- **Erratic behavior:** Check for solder bridges, verify IC orientations, check power supply stability

---

## Assembly Time Estimates

| System | PCB Assembly | System Integration | Testing | Total |
|--------|-------------|-------------------|---------|--------|
| The KIM | 3-4 hours | 1 hour | 1-2 hours | 5-7 hours |
| The DEV | 2-3 hours (2 boards) | 1 hour | 1-2 hours | 4-6 hours |
| The COB (minimal) | 6-8 hours (3-4 cards) | 2-3 hours | 2-3 hours | 10-14 hours |
| The COB (full) | 12-20 hours | 4-6 hours | 4-6 hours | 20-32 hours |

**Note:** Times assume prior soldering experience. First-time builders may take 2-3× longer.

---

## Tips for Success

**Organization:**
- Sort components before assembly (resistors, capacitors, ICs, etc.)
- Label resistor values (write on tape)
- Use compartmented storage (pill organizer, craft boxes)

**Patience:**
- Don't rush soldering (quality > speed)
- Take breaks (fatigue leads to mistakes)
- Work in good lighting

**Double-Check:**
- **Orientation matters:** ICs, diodes, electrolytic caps, LEDs
- **Triple-check CPU orientation** (expensive mistake!)
- **Measure before power-on** (5V to GND resistance)

**Testing:**
- Test early, test often
- Add ICs one at a time if troubleshooting
- Document results (what works, what doesn't)

**Ask for Help:**
- GitHub Discussions (community support)
- Post photos for troubleshooting
- Search for similar issues

---

**For more information, see:**
- [Getting Started Guide](GETTING_STARTED.md) - Beginner walkthrough
- [Troubleshooting Guide](TROUBLESHOOTING.md) - Diagnostic procedures
- [Hardware README](../../Hardware/README.md) - PCB specifications

---
