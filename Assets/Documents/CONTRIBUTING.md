Contributing to the 6502 Project
==================================

Thank you for your interest in contributing! This guide explains how to contribute hardware designs, firmware, documentation, and other improvements to the 6502 project.

**Last Updated:** March 7, 2026

---

## Table of Contents

- [Ways to Contribute](#ways-to-contribute)
- [Getting Started](#getting-started)
- [Hardware Contributions](#hardware-contributions)
- [Firmware Contributions](#firmware-contributions)
- [Documentation Contributions](#documentation-contributions)
- [Code Style Guidelines](#code-style-guidelines)
- [Design Guidelines](#design-guidelines)
- [Submission Process](#submission-process)
- [Testing Requirements](#testing-requirements)
- [Code of Conduct](#code-of-conduct)

---

## Ways to Contribute

### Hardware Design
- **New cards/boards**: Design expansion cards, helper boards, or complete systems
- **Board improvements**: Optimize existing designs, fix bugs, add features
- **Enclosures**: Create 3D-printed or laser-cut enclosures
- **Cable/connector designs**: Solutions for interconnections

### Firmware Development
- **New firmware**: Drivers for new peripherals
- **BIOS improvements**: Bootloader enhancements, new features
- **Monitor/debugger**: Enhanced debugging tools
- **Software libraries**: Reusable code modules

### Documentation
- **Tutorials**: Step-by-step guides for specific tasks
- **Examples**: Well-commented example programs
- **Datasheets**: Component documentation
- **Videos**: Assembly guides, demonstrations

### Testing and Bug Reports
- **Build testing**: Assemble and test boards, report issues
- **Bug reports**: Document problems with clear reproduction steps
- **Validation**: Verify fixes and improvements

### Community Support
- **Answer questions**: Help others in GitHub Discussions
- **Write FAQs**: Document common questions and answers
- **Share builds**: Post photos and experiences

---

## Getting Started

### Prerequisites

**For hardware contributions:**
- [ ] KiCad 7.0 or later installed
- [ ] Familiarity with PCB design principles
- [ ] Understanding of 6502 architecture (see [ARCHITECTURE.md](ARCHITECTURE.md))
- [ ] Read existing hardware designs in `Hardware/` folder

**For firmware contributions:**
- [ ] PlatformIO installed (see [Firmware README](../../Firmware/README.md))
- [ ] 6502 assembly knowledge OR C programming for microcontrollers
- [ ] Ability to test on real hardware or emulator

**For documentation:**
- [ ] Markdown knowledge
- [ ] Clear technical writing skills
- [ ] Understanding of the component/system being documented

### Fork and Clone Repository

1. **Fork the repository** on GitHub (click "Fork" button)
2. **Clone your fork:**
   ```bash
   git clone https://github.com/YOUR-USERNAME/6502.git
   cd 6502
   ```
3. **Add upstream remote:**
   ```bash
   git remote add upstream https://github.com/acwright/6502.git
   ```
4. **Create a branch** for your contribution:
   ```bash
   git checkout -b feature/my-new-card
   # or
   git checkout -b fix/serial-card-timing
   # or
   git checkout -b docs/assembly-guide-photos
   ```

---

## Hardware Contributions

### Designing New Boards/Cards

**Step 1: Plan your design**
- Define purpose and specifications
- Check for existing similar designs (avoid duplication)
- Post in GitHub Discussions to get feedback on concept

**Step 2: Create schematic**
- Use KiCad 7.0+
- Follow [Design Guidelines](#design-guidelines) below
- Use project libraries (`Libraries/6502 Parts.kicad_sym`, etc.)
- Add reference designators (U1, R1, C1, etc.)
- Include title block with:
  - Board name
  - Revision number
  - Your name/attribution
  - Date
  - License: CERN-OHL-P-2.0

**Step 3: Design PCB**
- Follow JLCPCB design rules (see `Production/` examples)
- Add mounting holes (M2.5 or M3, spaced for standard standoffs)
- Route power traces wider (20-40 mil for 1-2A)
- Add decoupling caps near all ICs (100nF)
- Include silkscreen labels (component references, connector pinouts)
- Add your logo/attribution on silkscreen (optional)

**Step 4: Generate production files**
- Gerber files (PCB layers)
- Drill files (PTH and NPTH)
- BOM (bill of materials) with LCSC part numbers
- CPL (component placement list) for assembly
- Place in `Production/<YourBoardName>/`

**Step 5: Documentation**
- Create README in `Hardware/<YourBoardName>/README.md`
- Include:
  - Description and purpose
  - Specifications (size, layers, power, I/O)
  - Register map (if applicable)
  - Connection/jumper settings
  - Assembly notes
  - Testing status (**UNTESTED** until verified!)
  - Photos (if assembled)

**Step 6: Test prototype**
- Order prototype from JLCPCB
- Assemble and test thoroughly
- Document any issues found
- Update design if needed
- Change status from **UNTESTED** to tested when verified

**Step 7: Submit pull request**
- See [Submission Process](#submission-process) below

### Modifying Existing Boards

**Step 1: Identify issue/improvement**
- Bug fix: Describe problem, root cause, solution
- Enhancement: Describe benefit, impact on existing users

**Step 2: Open issue on GitHub**
- Describe proposed change
- Get feedback before starting work (may save time!)

**Step 3: Make changes**
- Open existing KiCad project
- Modify schematic and/or PCB
- **Increment revision number** (e.g., Rev 1.0 → Rev 1.1)
- Update title block date
- Document changes in commit message

**Step 4: Regenerate production files**
- Update all Gerber, BOM, CPL files
- Place in `Production/<BoardName>/`

**Step 5: Update documentation**
- Update README with changes
- Note incompatibilities with previous revision (if any)

**Step 6: Test if possible**
- Order and build modified design
- Verify fix/improvement
- Report results

**Step 7: Submit pull request**

---

## Firmware Contributions

### Firmware Structure

All firmware projects use PlatformIO structure:
```
Firmware/<ProjectName>/
├── platformio.ini       # Build configuration
├── src/
│   └── main.cpp         # Main code (or main.c, main.asm)
├── include/             # Header files
├── lib/                 # Project-specific libraries
└── README.md            # Project documentation
```

### Coding Standards

**General:**
- Use descriptive variable/function names
- Comment complex logic
- No dead code (remove commented-out code before submitting)
- Test thoroughly on real hardware

**C/C++ (for microcontroller firmware):**
```c
// Constants: UPPER_CASE
#define LED_PIN 13
#define BAUD_RATE 115200

// Functions: camelCase or snake_case (be consistent)
void initializeSerial() {
  Serial.begin(BAUD_RATE);
}

// Variables: camelCase or snake_case
int buttonState = 0;
uint8_t dataBuffer[256];

// Indentation: 2 or 4 spaces (no tabs)
if (condition) {
  doSomething();
} else {
  doSomethingElse();
}

// Comments: Explain WHY, not WHAT
// Delay needed for EEPROM write cycle to complete
delay(10);
```

**6502 Assembly:**
```assembly
; Comments: Describe purpose of routine
; Initialize ACIA for 9600 baud, 8-N-1
InitACIA:
    LDA #$1F        ; Set baud rate and format
    STA ACIA_CTRL   ; Write to control register
    LDA #$0B        ; Enable TX/RX, no parity
    STA ACIA_CMD    ; Write to command register
    RTS

; Labels: PascalCase or descriptive
WaitForKey:
    LDA ACIA_STATUS ; Read status register
    AND #$08        ; Check RX buffer full flag
    BEQ WaitForKey  ; Loop until character received
    LDA ACIA_DATA   ; Read received character
    RTS

; Constants: Use EQU or define
ACIA_DATA    = $C100
ACIA_STATUS  = $C101
ACIA_CMD     = $C102
ACIA_CTRL    = $C103
```

### Adding New Firmware Project

**Step 1: Create project structure**
```bash
cd Firmware
mkdir "My New Project"
cd "My New Project"
```

**Step 2: Initialize PlatformIO project**
```bash
pio init --board <board_type>
# Examples:
#   ATmega1284p:  pio init --board ATmega1284P
#   Teensy 4.1:   pio init --board teensy41
#   RPi Pico:     pio init --board pico
```

**Step 3: Configure platformio.ini**
```ini
[env:myboard]
platform = <platform>
board = <board>
framework = arduino  ; or none for bare metal

; Add dependencies
lib_deps =
    somelib@^1.0.0

; Build flags
build_flags = 
    -DDEBUG_MODE=1

; Upload settings
upload_protocol = <protocol>
```

**Step 4: Write code**
- Implement functionality in `src/main.cpp` (or .c, .asm)
- Add libraries to `lib/` if needed
- Create header files in `include/`

**Step 5: Test**
- Build: `pio run`
- Upload: `pio run -t upload`
- Monitor: `pio device monitor`
- Fix any issues

**Step 6: Document**
- Create `README.md`:
  - Purpose of firmware
  - Target hardware
  - Features
  - Build/upload instructions
  - Configuration (if any)
  - Usage examples

**Step 7: Submit pull request**

### Modifying Existing Firmware

**Step 1: Open issue**
- Describe bug or enhancement
- Get feedback

**Step 2: Make changes**
- Edit code
- Follow existing code style
- Add comments explaining non-obvious changes

**Step 3: Test thoroughly**
- Verify fix/enhancement works
- Ensure no regressions (existing functionality still works)
- Test edge cases

**Step 4: Update documentation**
- Update README if behavior changed
- Add comments in code

**Step 5: Submit pull request**

---

## Documentation Contributions

### Types of Documentation

**README files:**
- Project overview and structure
- Quick start guides
- Reference information

**Standalone guides:**
- Detailed tutorials (GETTING_STARTED.md, ASSEMBLY_GUIDE.md, etc.)
- In-depth technical documentation (ARCHITECTURE.md)
- Troubleshooting procedures (TROUBLESHOOTING.md)

**Code comments:**
- Inline explanations in firmware or complex schematics
- Functional descriptions for modules

**Examples:**
- Well-commented sample programs
- Demonstrate specific features or techniques

### Documentation Standards

**Markdown formatting:**
```markdown
# Top-level heading (one per document)

## Second-level heading

### Third-level heading

**Bold text** for emphasis
*Italic text* for terms

- Bullet lists
- Use for non-ordered items

1. Numbered lists
2. Use for sequential steps

`Inline code` for commands, filenames, values

\```
Code blocks for longer code samples
Use language specifier: ```c, ```bash, ```assembly
\```

[Links](URL) for references
[Relative links](../Hardware/README.md) within project

> Blockquotes for important notes

Tables:
| Column 1 | Column 2 |
|----------|----------|
| Data 1   | Data 2   |
```

**Writing style:**
- Clear and concise
- Use active voice ("Connect the cable" not "The cable should be connected")
- Define acronyms on first use
- Assume beginner knowledge level (but provide depth for experts)

**Screenshots and diagrams:**
- Place in `Assets/Images/` or `Assets/Documents/`
- Use descriptive filenames
- Reference in markdown: `![Alt text](path/to/image.png)`

### Improving Existing Documentation

**Step 1: Identify issue**
- Unclear explanation
- Missing information
- Outdated content
- Typos/errors

**Step 2: Make changes**
- Edit markdown file
- Follow existing format and style
- Verify all links work

**Step 3: Preview**
- View rendered markdown (GitHub, VS Code preview, etc.)
- Check formatting correct

**Step 4: Submit pull request**
- Describe what was improved and why

---

## Code Style Guidelines

### KiCad Schematic Conventions

**Component references:**
- U = IC (U1, U2, U3...)
- R = Resistor
- C = Capacitor
- D = Diode
- Q = Transistor
- J = Connector
- SW = Switch
- Y = Crystal/Oscillator

**Net naming:**
- Address bus: A0, A1, ..., A15
- Data bus: D0, D1, ..., D7
- Power: +5V, GND (or VCC, VSS)
- Active-low signals: RES (or RESET with overline), IRQ, NMI
- Descriptive names: PHI2, RW, ACIA_TX, VIA_CS

**Schematic organization:**
- Group related components (power supply, CPU, memory, I/O)
- Use hierarchical sheets for complex designs
- Add labels to indicate sheet boundaries

### KiCad PCB Conventions

**Layers:**
- F.Cu (top copper): Primary component side
- B.Cu (bottom copper): Mostly ground plane
- F.SilkS (top silkscreen): Component references, values, logos
- F.Fab (fabrication layer): Component outlines, values
- F.CrtYd (courtyard): Component clearance boundaries

**Trace widths:**
- Power (5V, GND): 20-40 mil (0.5-1.0mm)
- Signals: 10-15 mil (0.25-0.38mm)
- High-speed signals: Impedance-controlled (consult JLCPCB)

**Clearances:**
- Trace-to-trace: 8 mil minimum (10 mil preferred)
- Trace-to-pad: 8 mil minimum
- Via drill: 0.3mm minimum (larger preferred for hand assembly)

**Design rules:**
- Use JLCPCB capabilities as baseline (see Production README)
- Add margin for reliability (don't push limits)

---

## Design Guidelines

### Hardware Design Principles

1. **Modularity**: Design boards to be independent, interchangeable
2. **Testability**: Include test points for key signals
3. **Robustness**: Add protection (ESD, reverse polarity, overcurrent)
4. **Compatibility**: Follow standard pinouts and protocols
5. **Documentation**: Every design should be thoroughly documented
6. **Openness**: Use open standards, avoid proprietary components when possible

### Card Design (COB System)

**Standard card dimensions:**
- 100mm × 60mm typical (Euro card size)
- Adjust as needed for components

**Card edge connector:**
- Use standard pinout from backplane
- All cards share address/data/control bus
- Include address decoding logic on card

**Address assignment:**
- Check existing cards to avoid conflicts
- Document address range in README
- Use jumpers for user-configurable addressing (preferred)

**Power:**
- Don't exceed backplane current limit (check backplane specs)
- Add local decoupling (100nF per IC minimum)
- Calculate and document power consumption

### Firmware Design Principles

1. **Portability**: Abstract hardware dependencies when possible
2. **Modularity**: Use functions/libraries for reusable code
3. **Efficiency**: Optimize for speed/size where appropriate (but clarity first)
4. **Error handling**: Check return values, handle edge cases
5. **Documentation**: Comment non-obvious code, write README

---

## Submission Process

### Pull Request Checklist

**Before submitting:**
- [ ] Code/design builds without errors
- [ ] Tested on real hardware (or emulator if hardware unavailable)
- [ ] Documentation updated (README, comments, etc.)
- [ ] Production files generated (for hardware)
- [ ] Follows project style guidelines
- [ ] Commit messages descriptive
- [ ] No merge conflicts with main branch

### Creating Pull Request

**Step 1: Push changes to your fork**
```bash
git add .
git commit -m "Add Serial Card Pro design with flow control"
git push origin feature/serial-card-pro
```

**Step 2: Open pull request on GitHub**
- Go to original repository (acwright/6502)
- Click "Pull requests" → "New pull request"
- Select your fork and branch
- Click "Create pull request"

**Step 3: Write PR description**
```markdown
## Description
Adds new Serial Card Pro design with hardware flow control (RTS/CTS).

## Changes
- New KiCad schematic and PCB design
- Production files (Gerber, BOM, CPL)
- Hardware README with specifications
- Updated main README to include Serial Card Pro

## Testing
- [x] Schematic passes ERC (electrical rules check)
- [x] PCB passes DRC (design rules check)
- [ ] Prototype ordered (pending arrival)
- [ ] Tested on real hardware (pending prototype)

## Notes
Mark as UNTESTED until prototype verified.
```

**Step 4: Respond to review comments**
- Maintainer may request changes
- Make changes, push to same branch (PR auto-updates)
- Discuss if you disagree with feedback

**Step 5: Merge**
- Once approved, maintainer will merge
- Your contribution is now part of the project!

### Commit Message Format

**Format:**
```
<type>: <short summary>

<detailed description if needed>
```

**Types:**
- `feat`: New feature (new board, new firmware, etc.)
- `fix`: Bug fix
- `docs`: Documentation only
- `refactor`: Code restructure (no functionality change)
- `test`: Add or update tests
- `chore`: Maintenance (update dependencies, etc.)

**Examples:**
```
feat: Add Sound Card with AY-3-8910

Implements sound card for COB system using AY-3-8910 PSG.
Includes schematic, PCB, production files, and documentation.
Status: UNTESTED (prototype on order).

---

fix: Correct Serial Card RTS polarity

RTS signal was inverted. Changed to active-low to match RS-232 standard.
Tested on hardware, serial flow control now works correctly.

---

docs: Add 3D printing guide to CAD README

Explains recommended slicer settings, post-processing steps,
and threaded insert installation.
```

---

## Testing Requirements

### Hardware Testing

**Minimum testing before marking "tested":**
- [ ] PCB manufactured (inspect for defects)
- [ ] Components assembled (check solder joints)
- [ ] Power-on test (no smoke, correct voltages)
- [ ] Functional test (board performs intended function)
- [ ] Integration test (works with other boards/cards if applicable)
- [ ] Documentation verified (README matches actual behavior)

**Document test results:**
- Create file: `Hardware/<BoardName>/TESTING.md` (or section in README)
- Note:
  - Test date, tester name
  - Hardware revision tested
  - Test procedure used
  - Results (pass/fail, notes)
  - Known issues or limitations

### Firmware Testing

**Minimum testing:**
- [ ] Compiles without errors or warnings
- [ ] Uploads to target hardware successfully
- [ ] Performs intended function correctly
- [ ] Edge cases tested (boundary conditions, error cases)
- [ ] No regressions (existing features still work)

**Document testing:**
- Add test results to firmware README
- Note any hardware-specific requirements
- List configurations tested (if multiple supported)

---

## Code of Conduct

### Our Standards

**Positive behaviors:**
- Welcoming and inclusive language
- Respectful of differing viewpoints
- Gracefully accepting constructive criticism
- Focusing on what's best for the community
- Showing empathy towards others

**Unacceptable behaviors:**
- Harassment, insults, or derogatory comments
- Trolling or inflammatory comments
- Publishing others' private information
- Other conduct inappropriate in professional setting

### Enforcement

- Violations should be reported to project maintainer
- Maintainer will review and determine appropriate action
- Action may include warning, temporary ban, or permanent ban

### Attribution

Adapted from [Contributor Covenant v2.1](https://www.contributor-covenant.org/version/2/1/code_of_conduct/).

---

## Recognition

Contributors are recognized through:
- GitHub contributor list (automatic)
- Attribution in board silkscreen (optional, for hardware designers)
- README acknowledgments (for significant contributions)

---

## Questions?

- **GitHub Discussions**: Ask questions, discuss ideas
- **Issues**: Report bugs, request features
- **Email**: Contact maintainer for private inquiries

---

**Thank you for contributing to the 6502 project!**

---

**Last Updated:** March 7, 2026
