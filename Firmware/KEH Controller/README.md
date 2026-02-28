# KEH Controller

## Overview

The KEH (Keyboard Encoder Helper) Controller is firmware for the ATMega1284P microcontroller that provides dual keyboard input support for 6502-based computer systems. It can simultaneously handle both PS/2 keyboard input and custom keyboard matrix input, converting keypresses to ASCII codes and outputting them through a 6522 VIA (Versatile Interface Adapter) interface.

## Features

- **Dual Input Support**: Handles both PS/2 keyboards and 8x8 keyboard matrix simultaneously
- **ASCII Conversion**: Converts PS/2 scancodes and matrix keypresses to ASCII characters
- **Extended Character Set**: Supports standard ASCII (0x00-0x7F) plus extended codes (0x80-0xFF)
- **Function Keys**: Full support for F1-F12 function keys with Alt modifier
- **Modifier Keys**: Complete support for Shift, Ctrl, Alt, Caps Lock, and Fn keys
- **Buffered Input**: Uses circular buffers to prevent data loss during rapid typing
- **Debounced Matrix Scanning**: Hardware debouncing for reliable matrix keyboard operation
- **Enable/Disable Control**: Independent enable signals for PS/2 and matrix inputs

## Hardware Connections

### ATMega1284P Pin Assignments

#### VIA PORT A (Pins 24-31)
Used for row scanning (matrix mode) or ASCII output (PS/2 mode):
- PA0 (Pin 24) - VIA_PA0 / Row 0 / Data Bit 0
- PA1 (Pin 25) - VIA_PA1 / Row 1 / Data Bit 1
- PA2 (Pin 26) - VIA_PA2 / Row 2 / Data Bit 2
- PA3 (Pin 27) - VIA_PA3 / Row 3 / Data Bit 3
- PA4 (Pin 28) - VIA_PA4 / Row 4 / Data Bit 4
- PA5 (Pin 29) - VIA_PA5 / Row 5 / Data Bit 5
- PA6 (Pin 30) - VIA_PA6 / Row 6 / Data Bit 6
- PA7 (Pin 31) - VIA_PA7 / Row 7 / Data Bit 7

#### VIA PORT B (Pins 0-7)
Used for column scanning (matrix mode) or ASCII output (matrix output):
- PB0 (Pin 0) - VIA_PB0 / Column 0 / Data Bit 0
- PB1 (Pin 1) - VIA_PB1 / Column 1 / Data Bit 1
- PB2 (Pin 2) - VIA_PB2 / Column 2 / Data Bit 2
- PB3 (Pin 3) - VIA_PB3 / Column 3 / Data Bit 3
- PB4 (Pin 4) - VIA_PB4 / Column 4 / Data Bit 4
- PB5 (Pin 5) - VIA_PB5 / Column 5 / Data Bit 5
- PB6 (Pin 6) - VIA_PB6 / Column 6 / Data Bit 6
- PB7 (Pin 7) - VIA_PB7 / Column 7 / Data Bit 7

#### Control Signals
- Pin 8 (VIA_CA1) - PS/2 Data Ready strobe (output)
- Pin 9 (VIA_CA2) - PS/2 Enable (input, active low)
- Pin 10 (PS2CLK) - PS/2 Clock input
- Pin 11 (PS2DATA) - PS/2 Data input
- Pin 12 (VIA_CB1) - Matrix Data Ready strobe (output)
- Pin 13 (VIA_CB2) - Matrix Enable (input, active low)

### PS/2 Keyboard Interface

Connect a PS/2 keyboard to:
- **CLK**: Pin 10 (PS2CLK) with pull-up resistor
- **DATA**: Pin 11 (PS2DATA) with pull-up resistor
- **VCC**: +5V
- **GND**: Ground

### Keyboard Matrix Interface

The firmware supports an 8x8 keyboard matrix (64 keys maximum):
- **Rows**: Connected to PA0-PA7 (Pins 24-31)
- **Columns**: Connected to PB0-PB7 (Pins 0-7)

Each key connects a row to a column when pressed. The matrix is scanned with rows driven low and columns read with pull-ups.

## Keyboard Matrix Layout

```
       PB0    PB1    PB2    PB3    PB4    PB5    PB6    PB7
PA0:    `      1      2      3      4      5      6      7
PA1:    8      9      0      -      =      BS     ESC    TAB
PA2:    q      w      e      r      t      y      u      i
PA3:    o      p      [      ]      \      INS    CAPS   a
PA4:    s      d      f      g      h      j      k      l
PA5:    ;      '    ENTER   DEL   SHIFT    z      x      c
PA6:    v      b      n      m      ,      .      /      UP
PA7:  CTRL   MENU    ALT   SPACE   FN    LEFT   DOWN  RIGHT
```

**Special Keys:**
- BS = Backspace (0x08)
- ESC = Escape (0x1B)
- TAB = Tab (0x09)
- INS = Insert (0x1A)
- DEL = Delete (0x7F)
- ENTER = Enter (0x0D)
- CAPS = Caps Lock (toggle)
- Arrow keys: UP (0x1E), LEFT (0x1C), DOWN (0x1F), RIGHT (0x1D)
- MENU = Menu/Windows key (0x80, or 0x90 with Alt)

## Operation Modes

### PS/2 Mode
When VIA_CA2 (Pin 9) is pulled LOW:
- PS/2 keyboard is enabled
- Scancodes are converted to ASCII
- Output appears on PORT A (PA0-PA7)
- VIA_CA1 pulses low when data is ready

### Matrix Mode
When VIA_CB2 (Pin 13) is pulled LOW:
- Matrix keyboard is enabled
- Matrix is scanned every 10ms
- Keys are debounced (stable for 2 scans)
- Output appears on PORT B (PB0-PB7)
- VIA_CB1 pulses low when data is ready

### Dual Mode
Both keyboards can operate simultaneously if both enable signals are active.

## ASCII Character Mapping

### Standard ASCII (0x00-0x7F)

**Control Characters (Ctrl+Key):**
- Ctrl+2 = 0x00 (NUL)
- Ctrl+A-Z = 0x01-0x1A
- Ctrl+[ = 0x1B (ESC)
- Ctrl+\ = 0x1C (FS)
- Ctrl+] = 0x1D (GS)
- Ctrl+6 = 0x1E (RS)
- Ctrl+- = 0x1F (US)

**Printable Characters:**
- Space, numbers, letters, and symbols follow standard ASCII

### Extended Codes (0x80-0xFF)

**Function Keys:**
- F1-F10 = 0x81-0x8A
- F11-F12 = 0x8B-0x8C
- Alt+F1-F10 = 0x91-0x9A
- Alt+F11-F12 = 0x9B-0x9C

**Matrix Fn Key Combinations:**
- Fn+1 through Fn+0 = F1-F10 (0x81-0x8A)
- Alt+Fn+1 through Alt+Fn+0 = Alt+F1-F10 (0x91-0x9A)
- Fn+ESC = F11 (0x9B)
- Fn+LEFT = F12 (0x9C)
- Fn+RIGHT = F13 (0x9D)
- Fn+UP = F14 (0x9E)
- Fn+DOWN = F15 (0x9F)

**Special Keys:**
- Menu/Windows = 0x80
- Alt+Menu = 0x90

**Alt Key Combinations:**
- Alt+letters = 0xE1-0xFA (a-z)
- Alt+numbers = 0xB0-0xB9 (0-9)
- Alt+Shift+letters = 0xC1-0xDA (A-Z)
- Alt+other symbols = Various codes in 0xA0-0xFF range

## Build Instructions

### Prerequisites

1. **Install PlatformIO**
   ```bash
   # Using pip
   pip install platformio
   
   # Or install PlatformIO IDE extension for VS Code
   ```

2. **Install MiniPro Programmer Software** (for uploading)
   ```bash
   # macOS (using Homebrew)
   brew install minipro
   
   # Linux
   sudo apt-get install minipro
   
   # Or build from source: https://gitlab.com/DavidGriffith/minipro
   ```

### Building the Firmware

1. **Navigate to the project directory:**
   ```bash
   cd "Firmware/KEH Controller"
   ```

2. **Build the firmware:**
   ```bash
   # Build for ATMega1284P (default)
   pio run -e atmega1284p
   
   # Or build for ATMega1284
   pio run -e atmega1284
   ```

3. **Build output:**
   The compiled firmware will be located at:
   ```
   .pio/build/atmega1284p/firmware.hex
   ```

### Uploading the Firmware

The project uses a MiniPro TL866 programmer for uploading.

1. **Connect the MiniPro programmer** to your ATMega1284P chip

2. **Upload firmware and fuses:**
   ```bash
   # Upload to ATMega1284P
   pio run -e atmega1284p -t upload
   
   # Or upload to ATMega1284
   pio run -e atmega1284 -t upload
   ```

3. **Manual upload (if needed):**
   ```bash
   # Flash the program
   minipro -p "ATMEGA1284P@DIP40" -c code -w .pio/build/atmega1284p/firmware.hex
   
   # Flash the fuses and lock bits
   minipro -p "ATMEGA1284P@DIP40" -c config -w fuses.cfg --fuses --lock
   ```

## Fuse Configuration

The `fuses.cfg` file contains the ATMega1284P fuse settings:

```properties
lfuse = 0xff   # Low fuse
hfuse = 0xff   # High fuse  
efuse = 0xff   # Extended fuse
lock = 0xff    # Lock bits (unprogrammed)
```

**Fuse Settings:**
- **Low Fuse (0xFF)**: Full-swing crystal oscillator, no clock divide
- **High Fuse (0xFF)**: Default settings
- **Extended Fuse (0xFF)**: Default settings
- **Lock Bits (0xFF)**: No memory lock protection

⚠️ **Warning**: Incorrect fuse settings can brick your microcontroller. Verify fuse values are appropriate for your hardware configuration before programming.

## Dependencies

The firmware requires the following library (automatically installed by PlatformIO):

- **CircularBuffer** (v1.4.0+) by Roberto Lo Giacco
  - Provides interrupt-safe circular buffers for keyboard data

## Project Structure

```
KEH Controller/
├── platformio.ini          # PlatformIO configuration
├── fuses.cfg              # AVR fuse configuration
├── src/
│   └── main.cpp           # Main firmware source code
├── include/               # Header files (if any)
├── lib/                   # Local libraries
└── test/                  # Unit tests
```

## Troubleshooting

### Build Issues

**Problem**: PlatformIO not found
```bash
# Solution: Install or update PlatformIO
pip install -U platformio
```

**Problem**: Library dependency errors
```bash
# Solution: Clean and rebuild
pio run -t clean
pio run
```

### Upload Issues

**Problem**: MiniPro not found
```bash
# Solution: Ensure minipro is installed and in PATH
which minipro
```

**Problem**: Chip not detected
- Check that the chip is properly seated in the programmer
- Verify you're using the correct chip model (ATMEGA1284P vs ATMEGA1284)
- Check for proper power supply to the programmer

### Runtime Issues

**Problem**: No keyboard response
- Verify enable signals (CA2/CB2) are at correct logic levels
- Check PS/2 clock and data connections
- Verify matrix keyboard connections
- Ensure VIA interface connections are correct

**Problem**: Incorrect characters output
- Check for proper pull-up resistors on PS/2 lines
- Verify keyboard matrix wiring matches the defined layout
- Test with a known-good PS/2 keyboard

## License

See the main repository LICENSE file for licensing information.

## Contributing

This firmware is part of the 6502 computer project. Contributions and improvements are welcome through the main repository.

