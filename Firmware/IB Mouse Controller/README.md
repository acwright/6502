# IB Mouse Controller

PS/2 Mouse to Serial Interface Controller firmware for ATTiny85 microcontroller. This firmware bridges a PS/2 mouse to a custom serial interface designed for the 6502 Input Board.

## Overview

The IB Mouse Controller reads PS/2 mouse data (movement, button states, and scroll wheel) and converts it to a serial interface that can be read by the 6502-based system. The controller only transmits data when there's actual movement or button state changes, reducing unnecessary interrupts.

## Features

- **PS/2 Mouse Interface**: Reads standard PS/2 mouse protocol including scroll wheel support
- **Serial Output**: Provides mouse data via shift register to host system
- **Interrupt-Driven**: Generates interrupts only when new data is available
- **Chip Select**: Can be enabled/disabled via CS pin for bus sharing
- **Low Power**: Tri-states outputs when not selected
- **Optimized Polling**: 250ms polling interval with change detection

## Hardware Requirements

### Target Microcontroller
- **ATTiny85** in DIP-8 package
- Clock: 8MHz internal oscillator
- Fuse configuration provided in `fuses.cfg`

### Pin Assignment

| Pin | Function | Direction | Description |
|-----|----------|-----------|-------------|
| PB0 | CLK | Output | Serial clock output to shift register |
| PB1 | DATA | Output | Serial data output to shift register |
| PB2 | PS2CLK | Bidirectional | PS/2 mouse clock line |
| PB3 | PS2DATA | Bidirectional | PS/2 mouse data line |
| PB4 | INT | Output | Interrupt signal to host system |
| PB5 | CS | Input | Chip select (active HIGH) |

## Communication Protocol

When the controller detects mouse movement or button changes, it transmits 4 bytes in sequence:

1. **Status Byte**: Mouse button states and overflow flags
   - Bit 0: Left button (1 = pressed)
   - Bit 1: Right button (1 = pressed)
   - Bit 2: Middle button (1 = pressed)
   - Bits 3-7: PS/2 status flags

2. **X Movement**: Signed 8-bit X-axis displacement
3. **Y Movement**: Signed 8-bit Y-axis displacement
4. **Scroll Wheel**: Signed 8-bit scroll wheel displacement

Each byte is followed by an interrupt pulse on the INT pin to signal data availability to the host.

## Building with PlatformIO

### Prerequisites

1. **Install PlatformIO**
   - Via VS Code: Install the PlatformIO IDE extension
   - Via CLI: `pip install platformio`

2. **Install MinIPRO** (for programming)
   - macOS: `brew install minipro`
   - Linux: Install from [gitlab.com/DavidGriffith/minipro](https://gitlab.com/DavidGriffith/minipro)
   - Requires TL866 series programmer hardware

### Build Instructions

1. **Clone/Navigate to Project Directory**
   ```bash
   cd "Firmware/IB Mouse Controller"
   ```

2. **Build the Firmware**
   ```bash
   platformio run
   ```
   
   This compiles the code for the ATTiny85 target. Build output will be in `.pio/build/attiny85/`

3. **Clean Build** (if needed)
   ```bash
   platformio run --target clean
   ```

### Programming the ATTiny85

The project is configured to use MinIPRO with a TL866 programmer.

1. **Connect TL866 Programmer**
   - Insert ATTiny85 into DIP-8 socket on programmer
   - Connect programmer to USB port

2. **Upload Firmware and Fuses**
   ```bash
   platformio run --target upload
   ```
   
   This command performs two operations:
   - Writes the compiled firmware to flash memory
   - Programs the fuse bits from `fuses.cfg`

### Manual Programming (Alternative)

If you need to program without PlatformIO:

1. **Flash the Firmware**
   ```bash
   minipro -p "ATTINY85@DIP8" -c code -w .pio/build/attiny85/firmware.hex
   ```

2. **Program Fuses**
   ```bash
   minipro -p "ATTINY85@DIP8" -c config -w fuses.cfg --fuses --lock
   ```

## Fuse Configuration

The `fuses.cfg` file contains the following settings:

```
lfuse = 0xe2  # 8MHz internal oscillator, slowly rising power
hfuse = 0x5f  # Serial programming enabled, brown-out at 2.7V
efuse = 0xff  # Default
lock  = 0xff  # No memory lock
```

**Important**: Incorrect fuse settings can brick your ATTiny85. Double-check values before programming.

## Dependencies

- **PS2Mouse Library**: Custom library (included in `lib/PS2Mouse/`)
  - Handles PS/2 protocol communication
  - Provides mouse data structure with position, status, and wheel data
  
- **Arduino Framework**: Used for GPIO and timing functions

## Development

### Project Structure

```
IB Mouse Controller/
├── src/
│   └── main.cpp           # Main firmware code
├── lib/
│   └── PS2Mouse/          # PS/2 mouse protocol library
│       ├── PS2Mouse.h
│       └── PS2Mouse.cpp
├── include/               # Additional headers (if needed)
├── platformio.ini         # PlatformIO configuration
├── fuses.cfg             # ATTiny85 fuse configuration
└── README.md             # This file
```

### Modifying the Code

1. Main application logic is in `src/main.cpp`
2. Modify polling interval by changing the value in: `if (millis() - lastRun > 250)`
3. Adjust timing delays as needed for your shift register requirements
4. PS/2 library can be modified in `lib/PS2Mouse/`

### Debugging Tips

- The firmware has no serial debug output (ATTiny85 doesn't have hardware UART)
- Use an oscilloscope or logic analyzer to monitor the CLK, DATA, and INT pins
- Verify PS/2 mouse is properly powered (5V) and connected
- Check that pull-up resistors are present on PS2CLK and PS2DATA lines

## Circuit Notes

### Required External Components

- **Pull-up resistors** on PS2CLK and PS2DATA lines (typically 10kΩ)
- **Pull-up resistor** on CS pin if using open-collector/open-drain driver
- Proper **power supply decoupling** (0.1µF ceramic capacitor near VCC/GND)

### Power Consumption

- Active mode: ~10-15mA @ 5V (with mouse connected)
- Disabled mode: ~1-2mA @ 5V (CS LOW, pins tri-stated)

## License

See the main repository LICENSE file for details.

## Version History

- **v1.0**: Initial release
  - PS/2 mouse support with scroll wheel
  - Chip select functionality
  - Interrupt-driven data output
