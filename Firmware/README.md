6502 Firmware
=============

This folder contains firmware for microcontrollers used throughout the 6502 project. Firmware is written in C/C++ using the **PlatformIO** development environment and targets various microcontrollers including Teensy 4.1, ATmega328p, ATmega1284p, and ATTiny85.

---

## Table of Contents

- [Overview](#overview)
- [Firmware Projects](#firmware-projects)
- [Development Environment Setup](#development-environment-setup)
- [Building and Uploading](#building-and-uploading)
- [Debugging](#debugging)
- [Firmware by System](#firmware-by-system)
- [Common Libraries](#common-libraries)
- [Naming Conventions](#naming-conventions)

---

## Overview

The 6502 project uses microcontrollers to provide:
- **CPU Emulation**: Teensy 4.1 emulates 65C02 with cycle accuracy
- **Peripheral Controllers**: Microcontrollers manage keyboards, mice, displays, storage
- **Protocol Translation**: Convert modern protocols (PS/2, USB) to 6502-compatible interfaces
- **Enhanced Features**: Add capabilities not available in vintage hardware (networking, web interfaces)

### Microcontroller Platforms

| Platform | Clock Speed | Flash | RAM | Used In |
|----------|-------------|-------|-----|---------|
| Teensy 4.1 | 600 MHz | 8MB | 1MB (+8MB PSRAM) | Dev Board, Dev Output Board |
| ATmega1284p | 16 MHz | 128KB | 16KB | Keyboard Encoder Helper, Input Board Rev 1.0 |
| ATmega328p | 16 MHz | 32KB | 2KB | PS2 Helper |
| ATTiny85 | 8 MHz | 8KB | 512 bytes | Input Board Rev 0.0 (keyboard + mouse) |

---

## Firmware Projects

### By Hardware Board

| Firmware Project | Target Board | Microcontroller | System | Purpose |
|-----------------|--------------|-----------------|--------|---------|
| [DB Emulator](./DB%20Emulator/) | Dev Board | Teensy 4.1 | DEV | 65C02 emulation + networking + web control |
| [DOB Video Display](./DOB%20Video%20Display/) | Dev Output Board | Teensy 4.0 | DEV | TMS9918A VDP + SID audio emulation on 2.4" LCD |
| [IB Keyboard Controller](./IB%20Keyboard%20Controller/) | Input Board Rev 0.0 | ATTiny85 | VCS | PS/2 keyboard to serial interface |
| [IB Mouse Controller](./IB%20Mouse%20Controller/) | Input Board Rev 0.0 | ATTiny85 | VCS | PS/2 mouse interface |
| [KEH Controller](./KEH%20Controller/) | Keyboard Encoder Helper, Input Board 1.0 | ATmega1284p | COB/KIM | PS/2 + matrix keyboard to ASCII |
| [PS2 Keyboard Controller](./PS2%20Keyboard%20Controller/) | PS2 Helper | ATmega328p | COB/KIM | PS/2 keyboard to matrix emulation |
| [STP Controller](./STP%20Controller/) | Storage Card Pro | (varies) | COB | SD + Flash storage controller |

---

## Development Environment Setup

### Prerequisites

**Required Software:**
- [PlatformIO Core](https://platformio.org/install/cli) (CLI) or [PlatformIO IDE](https://platformio.org/platformio-ide) (VS Code extension)
- [Python 3.7+](https://www.python.org/) (usually bundled with PlatformIO)
- [Git](https://git-scm.com/) (for cloning repositories and dependencies)

**Optional Tools:**
- [Visual Studio Code](https://code.visualstudio.com/) (recommended editor with PlatformIO extension)
- [Teensy Loader](https://www.pjrc.com/teensy/loader.html) (alternative upload tool for Teensy)
- [AVRDUDE](https://www.nongnu.org/avrdude/) (alternative programmer for AVR chips)

### PlatformIO Installation

#### Option 1: PlatformIO IDE (Recommended for Beginners)

1. Install Visual Studio Code from [code.visualstudio.com](https://code.visualstudio.com/)
2. Open VS Code
3. Go to Extensions (Ctrl+Shift+X / Cmd+Shift+X)
4. Search for "PlatformIO IDE"
5. Click Install
6. Restart VS Code

#### Option 2: PlatformIO Core (CLI)

**macOS/Linux:**
```bash
# Using curl
python3 -c "$(curl -fsSL https://raw.githubusercontent.com/platformio/platformio-master/scripts/get-platformio.py)"

# Or using pip
pip install -U platformio
```

**Windows:**
```bash
pip install -U platformio
```

**Verify Installation:**
```bash
pio --version
# Should print: PlatformIO Core, version X.X.X
```

### Cloning the Repository

```bash
git clone https://github.com/acwright/6502.git
cd 6502/Firmware
```

---

## Building and Uploading

### Using PlatformIO Core (CLI)

**Build firmware:**
```bash
cd <FirmwareProject>    # e.g., cd DB\ Emulator
pio run                  # Compile only
```

**Upload to board:**
```bash
pio run --target upload  # Compile and upload
```

**Clean build:**
```bash
pio run --target clean   # Remove build artifacts
```

**Specific environment:**
```bash
pio run -e devboard_1    # Build for specific environment (if multiple defined)
```

### Using PlatformIO IDE (VS Code)

1. Open firmware project folder in VS Code (File → Open Folder)
2. PlatformIO should auto-detect `platformio.ini`
3. Bottom toolbar will show PlatformIO tasks:
   - **Build** (✓ checkmark icon): Compile firmware
   - **Upload** (→ arrow icon): Compile and upload
   - **Clean** (🗑 trash icon): Clean build files
   - **Serial Monitor** (🔌 plug icon): Open serial monitor

**Keyboard Shortcuts:**
- Build: Ctrl+Alt+B / Cmd+Alt+B
- Upload: Ctrl+Alt+U / Cmd+Alt+U
- Serial Monitor: Ctrl+Alt+S / Cmd+Alt+S

### Serial Monitoring

**Using PlatformIO:**
```bash
pio device monitor
# Default baud rate from platformio.ini (usually 115200)
```

**Specify baud rate:**
```bash
pio device monitor -b 115200
```

**Exit serial monitor:** Ctrl+C

**Using CoolTerm (Recommended):**
1. Install [CoolTerm](https://freeware.the-meiers.org/)
2. Select serial port (usually /dev/ttyUSB0 or COM3 on Windows)
3. Set baud rate (115200 typical for Teensy, varies for others)
4. Click Connect

---

## Debugging

### Common Issues

**Upload fails:**
- Verify correct USB port selected
- Press reset/bootloader button on board (especially Teensy)
- Try different USB cable (some are charge-only, not data)
- Check USB drivers (Windows: Teensy drivers, AVR: FTDI/CH340 drivers)

**Code compiles but doesn't run:**
- Verify correct board environment in `platformio.ini`
- Check fuse settings (AVR microcontrollers)
- Verify clock source (external crystal vs. internal oscillator)
- Use serial output to check if code is executing

**Serial monitor shows garbage:**
- Wrong baud rate (check platformio.ini or firmware source)
- Line ending settings (usually CR+LF)
- Try different baud rates: 9600, 19200, 38400, 57600, 115200

**Crashes or resets:**
- Stack overflow (reduce local variable size, or increase stack)
- Watchdog timer (ensure watchdog properly configured)
- Power supply issues (check voltage and current)
- Buffer overruns (check array bounds)

---

## Firmware by System

### DEV System Firmware

**[DB Emulator](./DB%20Emulator/)** (Dev Board):
- Cycle-accurate 65C02 emulation via [vrEmu6502](https://github.com/visrealm/vrEmu6502)
- Variable CPU speed (0 Hz to maximum, exceeding real 65C02)
- Ethernet with mDNS discovery (`6502.local`)
- Embedded web server (REST API for control)
- SD card support (load ROMs, programs, save snapshots)
- USB host (keyboard, joystick input)
- Serial terminal (115200 baud)
- Real-time clock

**[DOB Video Display](./DOB%20Video%20Display/)** (Dev Output Board):
- Full TMS9918A VDP emulation (Graphics I/II, Text, Multicolor modes)
- SID 6581 3-voice audio synthesis (triangle, sawtooth, pulse, noise + full ADSR)
- ILI9341 2.4" TFT LCD (320×240) with 256×192 active area and TMS9918A color borders
- AV packet protocol at 6 Mbps via hardware UART (Serial1) from DB Emulator
- ~60 FPS rendering from VRAM state

### COB/KIM System Firmware

**[KEH Controller](./KEH%20Controller/)** (Keyboard Encoder Helper):
- Dual input: PS/2 keyboard + 8×8 keyboard matrix (simultaneous)
- ASCII conversion (0x00-0xFF extended character set)
- Full modifier support (Shift, Ctrl, Alt, Fn)
- Function keys F1-F12
- Circular buffer (16 bytes)
- Output via 6522 VIA ports

**[PS2 Keyboard Controller](./PS2%20Keyboard%20Controller/)** (PS2 Helper):
- PS/2 protocol decoding (with parity checking)
- MT8808 8×8 crosspoint switch control
- Emulates keyboard matrix
- Extended scancode support (E0 prefix for arrows, nav keys)
- Function key emulation (Fn + number keys = F1-F10)

### VCS System Firmware

**[KEH Controller](./KEH%20Controller/)** (Input Board Rev 1.0):
- Dual input: PS/2 keyboard + 8×8 keyboard matrix (simultaneous)
- ASCII conversion (0x00-0xFF extended character set)
- Full modifier support (Shift, Ctrl, Alt, Fn)
- Function keys F1-F12
- Circular buffer (16 bytes)
- Output via 6522 VIA ports

**[IB Keyboard Controller](./IB%20Keyboard%20Controller/)** (Input Board Rev 0.0 - ATTiny85 #1):
- PS/2 keyboard protocol decoding
- ASCII conversion
- Shift register output (serial)
- Circular buffer (16 bytes)
- Interrupt-driven (low latency)

**[IB Mouse Controller](./IB%20Mouse%20Controller/)** (Input Board Rev 0.0 - ATTiny85 #2):
- PS/2 mouse protocol decoding
- X/Y coordinate tracking
- Button state (left, right, middle)
- Interrupt-driven (low latency)

### Other Firmware

**[STP Controller](./STP%20Controller/)** (Storage Card Pro):
- SPI controller for 6502
- SD card interface (SPI)
- SPI Flash interface (16MB onboard)
- External SPI header
- **Status:** Firmware not verified

---

## Common Libraries

Firmware projects use these common libraries (managed via PlatformIO):

### Arduino Core Libraries
- **Wire**: I2C communication
- **SPI**: SPI communication (SD cards, displays)
- **Serial**: UART communication

### Third-Party Libraries

**DB Emulator:**
- [vrEmu6502](https://github.com/visrealm/vrEmu6502): 65C02 CPU emulation
- [QNEthernet](https://github.com/ssilverman/QNEthernet): Teensy Ethernet
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson): JSON parsing (REST API)
- [AsyncWebServer_Teensy41](https://github.com/khoih-prog/AsyncWebServer_Teensy41): Async web server
- [Bounce2](https://github.com/thomasfredericks/Bounce2): Button debouncing
- [TimeLib](https://github.com/PaulStoffregen/Time): Time management

**DOB Video Display:**
- [ILI9341_t3n](https://github.com/KurtE/ILI9341_t3n): Optimized ILI9341 driver for Teensy (replaces Adafruit_GFX + Adafruit_ILI9341)

**KEH/PS2 Controllers:**
- [PS2Keyboard](https://github.com/PaulStoffregen/PS2Keyboard): PS/2 protocol (or custom implementation)

### Installing Libraries

Libraries are automatically installed by PlatformIO based on `platformio.ini`:

```ini
[env:myboard]
lib_deps =
    bblanchon/ArduinoJson@^6.21.0
    paulstoffregen/Time@^1.6.1
```

**Manual installation (if needed):**
```bash
pio lib install "ArduinoJson"
```

---

## Naming Conventions

### Folder Name Prefixes

Firmware folders use prefixes to indicate target hardware:

- **DB**: Dev Board (e.g., DB Emulator)
- **DOB**: Dev Output Board (e.g., DOB Video Display)
- **IB**: Input Board (e.g., IB Keyboard Controller, IB Mouse Controller)
- **KEH**: Keyboard Encoder Helper (e.g., KEH Controller)
- **PS2**: PS2 Helper (e.g., PS2 Keyboard Controller)
- **STP**: Storage Card Pro (e.g., STP Controller)
- **MH**: Mega Helper (if firmware exists)
- **VGAP**: VGA Card Pro (if firmware exists)
- **VCP**: Video Card Pro (if firmware exists)

### File Structure (Typical)

```
<FirmwareProject>/
├── platformio.ini          # PlatformIO configuration
├── README.md               # Project-specific documentation
├── src/
│   ├── main.cpp            # Main firmware source
│   ├── config.h            # Configuration options
│   └── <other .cpp/.h>     # Additional source files
├── include/                # Header files
├── lib/                    # Project-specific libraries
└── test/                   # Unit tests (if any)
```

### PlatformIO Environments

`platformio.ini` may define multiple environments for different board revisions:

**Example (DB Emulator):**
```ini
[env:devboard_0]
board = teensy41
platform = teensy
framework = arduino
; Configuration for original Dev Board revision

[env:devboard_1]
board = teensy41
platform = teensy
framework = arduino
; Configuration for Dev Board Rev 1.1
```

Build specific environment:
```bash
pio run -e devboard_1
```

---

## Working with Firmware

### Modifying Existing Firmware

1. Navigate to firmware folder:
   ```bash
   cd Firmware/<ProjectName>
   ```

2. Edit source files in `src/` folder

3. Build and test:
   ```bash
   pio run                 # Compile
   pio run --target upload # Upload
   pio device monitor      # Monitor output
   ```

4. Commit changes:
   ```bash
   git add src/
   git commit -m "Description of changes"
   ```

### Creating New Firmware

1. Create project folder:
   ```bash
   mkdir Firmware/<NewProject>
   cd Firmware/<NewProject>
   ```

2. Initialize PlatformIO project:
   ```bash
   pio init -b <board_id>
   # Examples: teensy41, ATmega328P, ATmega1284P, attiny85
   ```

3. Edit `platformio.ini` to add libraries, build flags, etc.

4. Write firmware in `src/main.cpp`

5. Create README.md documenting:
   - Purpose and features
   - Hardware requirements
   - Build instructions
   - Configuration options
   - Register/API documentation (if applicable)

---

## Resources

**PlatformIO Documentation:**
- [PlatformIO Core](https://docs.platformio.org/en/latest/core/index.html)
- [PlatformIO IDE](https://docs.platformio.org/en/latest/ide/index.html)
- [Library Management](https://docs.platformio.org/en/latest/librarymanager/index.html)

**Microcontroller Documentation:**
- [Teensy 4.1](https://www.pjrc.com/store/teensy41.html)
- [ATmega1284p Datasheet](https://www.microchip.com/en-us/product/ATmega1284P)
- [ATmega328p Datasheet](https://www.microchip.com/en-us/product/ATmega328P)
- [ATTiny85 Datasheet](https://www.microchip.com/en-us/product/ATtiny85)

**6502 Resources:**
- [6502.org](http://www.6502.org/) - Community forum and resources
- [vrEmu6502 Documentation](https://github.com/visrealm/vrEmu6502)
- [6502 Instruction Reference](http://www.6502.org/tutorials/6502opcodes.html)

---

**For more information, see:**
- [Main README](../README.md) - Project overview
- [Hardware README](../Hardware/README.md) - Hardware specifications and pin assignments
- Individual firmware project READMEs for detailed documentation

---
