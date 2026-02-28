# PS2 Keyboard Controller

## Overview

This firmware runs on an ATmega328/ATmega328P microcontroller and acts as a bridge between a PS/2 keyboard and a custom keyboard matrix interface. It reads PS/2 scan codes from a standard PS/2 keyboard and translates them to control an MT8808 analog crosspoint switch, which in turn drives an 8×8 keyboard matrix connected to a 65C22 VIA (Versatile Interface Adapter).

## Features

- **PS/2 Keyboard Interface**: Reads scan codes from PS/2 keyboards using interrupt-driven communication
- **Make/Break Detection**: Handles both key press (make) and key release (break) events
- **Extended Scan Codes**: Supports extended PS/2 scan codes (E0 prefix) for special keys
- **Function Key Emulation**: Maps F1-F10 function keys to FN + number key combinations
- **Buffered Input**: Uses a 16-byte circular buffer to prevent scan code loss
- **MT8808 Control**: Drives an 8×8 analog crosspoint switch matrix via 3-bit X/Y addressing

## Hardware Requirements

### Microcontroller
- ATmega328 or ATmega328P (DIP28 package)
- 16 MHz crystal oscillator recommended for accurate timing

### External Components
- **MT8808**: 8×8 analog crosspoint switch IC
- **PS/2 Keyboard**: Standard PS/2 keyboard with clock and data lines
- **65C22 VIA**: Connected to MT8808 matrix output (Port A = rows, Port B = columns)

### Pin Mapping

| Function | ATmega328 Pin | Arduino Pin |
|----------|---------------|-------------|
| PS2 CLK  | PD2 (INT0)    | D2          |
| PS2 DATA | PD3 (INT1)    | D3          |
| AX0      | PC0 (A0)      | A0/D14      |
| AX1      | PC1 (A1)      | A1/D15      |
| AX2      | PC2 (A2)      | A2/D16      |
| AY0      | PC3 (A3)      | A3/D17      |
| AY1      | PC4 (A4)      | A4/D18      |
| AY2      | PC5 (A5)      | A5/D19      |
| DATA     | PB3           | D11         |
| STROBE   | PB4           | D12         |
| RESET    | PB5           | D13         |

## Keyboard Matrix Layout

The firmware maps PS/2 scan codes to an 8×8 matrix layout:

```
      PB0    PB1    PB2    PB3    PB4    PB5    PB6    PB7
PA0:  `      1      2      3      4      5      6      7
PA1:  8      9      0      -      =      BS     ESC    TAB
PA2:  q      w      e      r      t      y      u      i
PA3:  o      p      [      ]      \      INS    CAPS   a
PA4:  s      d      f      g      h      j      k      l
PA5:  ;      '      ENTER  DEL    SHIFT  z      x      c
PA6:  v      b      n      m      ,      .      /      UP
PA7:  CTRL   GUI    ALT    SPACE  FN     LEFT   DOWN   RIGHT
```

### Special Keys
- **Backspace**: 0x08
- **Escape**: 0x1B
- **Tab**: 0x09
- **Insert**: 0x1A
- **Delete**: 0x7F
- **Enter**: 0x0D
- **Arrow Keys**: UP (0x1E), LEFT (0x1C), DOWN (0x1F), RIGHT (0x1D)
- **Function Keys**: F1-F10 mapped as FN + number keys 1-0

## How It Works

### PS/2 Communication Protocol

The firmware uses interrupt-driven PS/2 communication:

1. **Interrupt Handler**: Triggered on falling edge of PS/2 CLK line
2. **Data Reception**: Reads 11-bit frames (1 start, 8 data, 1 parity, 1 stop)
3. **Parity Checking**: Validates odd parity; discards invalid frames
4. **Buffering**: Stores valid scan codes in a 16-byte circular buffer
5. **Timeout Handling**: Resets state machine if >250ms between clock pulses

### Scan Code Translation

The main loop processes buffered scan codes:

1. **Prefix Detection**: Handles E0 (extended) and F0 (break) prefixes
2. **Extended Keys**: Maps navigation keys (arrows, insert, delete) and modifier keys
3. **Make/Break**: Sets matrix switches high (1) for key press, low (0) for key release
4. **Function Keys**: Simultaneously activates FN key + number key positions

### MT8808 Matrix Control

The `writeMatrix()` function controls the crosspoint switch:

1. Sets 3-bit X address (columns PB0-PB7)
2. Sets 3-bit Y address (rows PA0-PA7)
3. Sets DATA line (1 = close switch, 0 = open switch)
4. Pulses STROBE to latch the command
5. Each operation takes ~3ms with delays for signal stability

## Build Instructions

### Prerequisites

1. **PlatformIO**: Install PlatformIO IDE or PlatformIO Core
   ```bash
   # Install PlatformIO Core (if not using IDE)
   pip install platformio
   ```

2. **TL866 Programmer** (optional): For uploading via minipro
   ```bash
   # macOS
   brew install minipro
   
   # Linux
   sudo apt-get install minipro
   ```

### Building the Firmware

#### Using PlatformIO IDE

1. Open the project folder in VS Code with PlatformIO extension
2. Select the environment (atmega328p or atmega328)
3. Click **Build** from the PlatformIO toolbar (or press `Ctrl+Alt+B`)

#### Using PlatformIO Core

```bash
# Navigate to project directory
cd "/Users/acwright/Developer/6502/Firmware/PS2 Keyboard Controller"

# Build for ATmega328P
pio run -e atmega328p

# Build for ATmega328
pio run -e atmega328

# Clean build files
pio run --target clean
```

The compiled firmware will be located at:
- `.pio/build/atmega328p/firmware.hex` (for ATmega328P)
- `.pio/build/atmega328/firmware.hex` (for ATmega328)

### Build Environments

#### atmega328p
- **Platform**: Atmel AVR
- **Board**: ATmega328P
- **Framework**: Arduino
- **Clock**: 16 MHz (default)

#### atmega328
- **Platform**: Atmel AVR
- **Board**: ATmega328 (no "P" variant)
- **Framework**: Arduino
- **Clock**: 16 MHz (default)

### Dependencies

The firmware requires the following library (automatically installed by PlatformIO):
- **CircularBuffer** v1.4.0 by Roberto Lo Giacco

## Upload Instructions

### Using TL866 Programmer (minipro)

The project is configured to use a TL866 programmer by default:

```bash
# Upload to ATmega328P
pio run -e atmega328p --target upload

# Upload to ATmega328
pio run -e atmega328 --target upload
```

This command will:
1. Write the compiled firmware to the chip
2. Program the fuse bits from `fuses.cfg`
3. Set the lock bits

### Manual Upload with AVRdude

If using a different programmer:

```bash
# Using USBasp
avrdude -c usbasp -p m328p -U flash:w:.pio/build/atmega328p/firmware.hex:i

# Using Arduino as ISP
avrdude -c avrisp -p m328p -P /dev/ttyUSB0 -b 19200 \
        -U flash:w:.pio/build/atmega328p/firmware.hex:i
```

### Fuse Configuration

The `fuses.cfg` file contains the following settings:

```
lfuse = 0xFF  (External crystal, max startup time)
hfuse = 0xFF  (SPIEN enabled, bootloader disabled)
efuse = 0xFF  (Brown-out detection disabled)
lock  = 0xFF  (No memory lock)
```

**Note**: Fuse settings depend on your hardware configuration. Common settings for 16 MHz external crystal:
- Low Fuse: 0xFF (full swing crystal oscillator)
- High Fuse: 0xD9 (SPIEN enabled, EESAVE disabled, 2048 byte bootloader)
- Extended Fuse: 0xFD (BOD 2.7V)

## Configuration

### Adjusting the Buffer Size

To change the circular buffer size, modify the `BUFFER_SIZE` definition in [src/main.cpp](src/main.cpp):

```cpp
#define BUFFER_SIZE 16  // Change to desired size (must be power of 2)
```

### Customizing Key Mappings

Key mappings are defined in the `ps2ToMatrix()` function. To change a key mapping:

1. Find the PS/2 scan code in the switch statement
2. Modify the `writeMatrix(x, y, data)` call with new coordinates

Example - remapping the 'A' key from (7,3) to (4,2):

```cpp
case 0x1C:      // A
  writeMatrix(0x4, 0x2, release ? 0x0 : 0x1);  // Changed from (0x3, 0x7)
  break;
```

## Troubleshooting

### PS/2 Keyboard Not Responding
- Check PS/2 CLK and DATA connections
- Verify pull-up resistors on PS/2 lines (typically 10kΩ)
- Ensure PS/2 keyboard is receiving 5V power

### Keys Not Registering
- Verify MT8808 connections and power supply
- Check 3-bit address lines (AX0-AX2, AY0-AY2)
- Test DATA and STROBE signals with oscilloscope
- Ensure RESET line properly initializes MT8808

### Incorrect Key Mapping
- Verify scan code mapping in `ps2ToMatrix()` function
- Check physical matrix wiring to VIA ports
- Use serial debugging to monitor received scan codes

### Build Errors
- Ensure CircularBuffer library is installed
- Verify PlatformIO platform and framework are up to date
- Check that all required tools are in PATH

## Technical Details

### Timing Specifications

- **PS/2 Clock Frequency**: 10-16.7 kHz (typical)
- **PS/2 Timeout**: 250ms between clock pulses
- **Matrix Write Delay**: 1ms per signal transition
- **Total Key Event Time**: ~3ms per key press/release

### Memory Usage

- **Flash**: ~4KB (program memory)
- **SRAM**: ~200 bytes (including buffer)
- **EEPROM**: Not used

### Interrupt Safety

The firmware uses `CIRCULAR_BUFFER_INT_SAFE` to ensure thread-safe buffer operations between the interrupt handler and main loop.

## License

See the LICENSE file in the project root for licensing information.

## Author

A.C. Wright - 6502 Project

## Version History

- **1.0.0**: Initial release with PS/2 to matrix translation