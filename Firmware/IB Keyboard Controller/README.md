# IB Keyboard Controller

PS/2 Keyboard to Serial Interface Controller firmware for ATTiny85 microcontroller. This firmware bridges a PS/2 keyboard to a custom serial interface designed for the 6502 Input Board.

## Overview

The IB (Input Board) Keyboard Controller is firmware for an ATTiny85 microcontroller that converts PS/2 keyboard signals into ASCII codes and outputs them via a parallel shift register interface. This allows a modern PS/2 keyboard to be used with the 6502 Input Board.

## Features

- **PS/2 Keyboard Protocol Decoding**: Complete implementation of PS/2 scancode set 2 protocol with parity checking
- **ASCII Conversion**: Real-time conversion of PS/2 scancodes to ASCII characters
- **Modifier Key Support**: Full support for Shift, Control, Alt, and Caps Lock keys
- **Extended Keycodes**: Handles extended scancodes (E0 prefix) for arrow keys, navigation keys, etc.
- **Control Character Generation**: Supports standard control character combinations (Ctrl+A through Ctrl+Z)
- **Function Keys**: Support for F1-F12 with Alt+Fn combinations
- **16-Byte Circular Buffer**: Prevents data loss during rapid typing
- **Interrupt-Driven**: Low latency PS/2 signal reception using hardware interrupts

## Hardware Requirements

- **Microcontroller**: ATTiny85 (DIP-8 package recommended for breadboard use)
- **Clock Speed**: 8 MHz internal oscillator
- **PS/2 Keyboard**: Any standard PS/2 keyboard
- **Programmer**: TL866 or compatible MiniPro programmer (for initial flashing)

## Pin Configuration

| ATTiny85 Pin | Function | Description |
|-------------|----------|-------------|
| PB0 (Pin 5) | CLK | Shift register clock output |
| PB1 (Pin 6) | DATA | Shift register data output |
| PB2 (Pin 7) | PS2CLK | PS/2 keyboard clock input (with interrupt) |
| PB3 (Pin 2) | PS2DATA | PS/2 keyboard data input |
| PB4 (Pin 3) | INT | Data ready interrupt output |
| PB5 (Pin 1) | CSB | Chip select (active low input) |

## Building with PlatformIO

### Prerequisites

1. **Install PlatformIO**: 
   - [PlatformIO IDE](https://platformio.org/platformio-ide) (VS Code extension), or
   - [PlatformIO Core](https://platformio.org/install/cli) (command-line)

2. **Install MiniPro Software** (for uploading):
   ```bash
   # macOS (using Homebrew)
   brew install minipro
   
   # Linux (Debian/Ubuntu)
   sudo apt-get install minipro
   ```

### Build Instructions

1. **Navigate to the firmware directory**:
   ```bash
   cd "Firmware/IB Keyboard Controller"
   ```

2. **Install dependencies**:
   ```bash
   pio lib install
   ```
   This will install the CircularBuffer library dependency.

3. **Build the firmware**:
   ```bash
   pio run
   ```
   
   The compiled firmware will be located at:
   ```
   .pio/build/attiny85/firmware.hex
   ```

4. **Upload to ATTiny85**:
   
   Connect your MiniPro programmer with the ATTiny85 inserted, then run:
   ```bash
   pio run -t upload
   ```
   
   This will:
   - Flash the compiled firmware to the ATTiny85
   - Set the fuse bits according to `fuses.cfg`
   - Configure the device for 8 MHz internal oscillator

### Manual Programming (Alternative)

If you prefer to program manually or need to separate the steps:

```bash
# Flash firmware
minipro -p "ATTINY85@DIP8" -c code -w .pio/build/attiny85/firmware.hex

# Set fuses
minipro -p "ATTINY85@DIP8" -c config -w fuses.cfg --fuses --lock
```

## Fuse Configuration

The firmware requires specific fuse settings for proper operation:

| Fuse | Value | Description |
|------|-------|-------------|
| Low Fuse | 0xE2 | 8 MHz internal oscillator, no divide-by-8 |
| High Fuse | 0x5F | Standard settings with Brown-out detection |
| Extended | 0xFF | No self-programming |
| Lock | 0xFF | No memory lock |

These settings are automatically applied when using `pio run -t upload`.

## Usage

### Startup
1. Connect a PS/2 keyboard to the PS2CLK and PS2DATA pins
2. Power the ATTiny85 with 5V
3. Pull CSB low to enable the keyboard controller

### Operation
- When CSB is LOW, the controller is active and will output keystrokes
- When a key is pressed, the controller converts it to ASCII and stores it in the buffer
- Data is shifted out on the DATA and CLK lines when CSB is selected
- The INT line pulses HIGH briefly when new data is available
- The shift register outputs 8-bit ASCII values in MSB-first format

### Key Mappings

#### Standard Keys
- All alphanumeric keys map to standard ASCII values
- Caps Lock toggles between uppercase and lowercase letters
- Shift modifier produces uppercase letters and shifted symbols

#### Control Characters
Press Ctrl + letter to generate ASCII control codes:
- Ctrl+A = 0x01 (SOH), Ctrl+B = 0x02 (STX), ..., Ctrl+Z = 0x1A (SUB)
- Ctrl+2 = 0x00 (NULL)
- Ctrl+[ = 0x1B (ESC)
- Ctrl+\ = 0x1C (FS/LEFT)
- Ctrl+] = 0x1D (GS/RIGHT)
- Ctrl+6 = 0x1E (RS/UP)
- Ctrl+- = 0x1F (US/DOWN)

#### Special Keys
- Enter: 0x0D (CR)
- Tab: 0x09 (HT)
- Backspace: 0x08 (BS)
- Escape: 0x1B (ESC)
- Delete: 0x7F (DEL)

#### Arrow Keys
- Up: 0x1E
- Down: 0x1F
- Left: 0x1C
- Right: 0x1D

#### Function Keys
- F1-F12: 0x81-0x8C
- Alt+F1 through Alt+F12: 0x91-0x9C

#### Alt Modifier
- Alt+any key sets the high bit (adds 0x80), producing extended ASCII

## Troubleshooting

### Build Issues
- **"Platform atmelavr not found"**: Run `pio platform install atmelavr`
- **Library errors**: Ensure CircularBuffer library is installed with `pio lib install`

### Upload Issues
- **"Device not found"**: Ensure MiniPro programmer is connected and recognized
- **"Verification failed"**: Check ATTiny85 is properly seated in programmer socket
- **Permission denied (Linux)**: Add your user to the appropriate group or use `sudo`

### Runtime Issues
- **No output**: Check CSB is pulled LOW to enable the controller
- **Garbled output**: Verify PS/2 connections and ensure proper pull-up resistors
- **Missing keys**: Check PS/2 CLK interrupt is properly configured (Pin PB2)

## Development

### Project Structure
```
IB Keyboard Controller/
├── src/
│   └── main.cpp          # Main firmware source
├── include/              # Header files (empty)
├── lib/                  # Custom libraries (empty)
├── test/                 # Unit tests (empty)
├── platformio.ini        # PlatformIO configuration
├── fuses.cfg             # ATTiny85 fuse configuration
└── README.md             # This file
```

### Dependencies
- **Arduino Framework**: Provides core Arduino functions
- **CircularBuffer** (v1.4.0): Thread-safe circular buffer implementation

### Modifying the Firmware
After making changes to `src/main.cpp`:
1. Build: `pio run`
2. Upload: `pio run -t upload`
3. Monitor serial output (if debugging): `pio device monitor`

## License

See the project root LICENSE file for licensing information.

## Credits

Developed for the 6502 Computer Project by A.C. Wright.