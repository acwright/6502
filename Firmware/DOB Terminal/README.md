# DOB Terminal

DOB Terminal is a hardware terminal emulator designed for the DOB (Dev Output Board). It provides a high-performance display and audio output system with serial command interface, ideal for retro computing projects and embedded system development.

## Features

- **320x240 Color Display** - ILI9341-based TFT display with 8-bit RGB332 palette
- **40x30 Character Grid** - 8x8 pixel character cells supporting text and graphics modes
- **Dual Serial Interfaces** - USB and hardware UART (2Mbps) for flexible connectivity
- **Audio Bell System** - Queue-based tone generator supporting musical note playback
- **VT-AC Protocol** - Custom serial command protocol for display control

## Hardware

- **MCU**: Teensy 4.0 (600 MHz ARM Cortex-M7)
- **Display**: ILI9341 320x240 TFT LCD
- **Audio**: PWM tone output
- **Communication**: USB Serial + UART Serial (Serial1)

## Installation

### Prerequisites

- [PlatformIO](https://platformio.org/) - Cross-platform build system
- [Visual Studio Code](https://code.visualstudio.com/) (recommended) with PlatformIO extension
- Teensy 4.0 board
- ILI9341 compatible TFT display

### Hardware Setup

Connect the display and audio to your Teensy 4.0 according to the pin definitions in `lib/DOBTerminal/pins.h`.

## Building and Uploading

### Using PlatformIO CLI

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd "DOB Terminal"
   ```

2. Build the project:
   ```bash
   pio run
   ```

3. Upload to Teensy 4.0:
   ```bash
   pio run --target upload
   ```

### Using VS Code

1. Open the project folder in VS Code
2. Click the PlatformIO icon in the sidebar
3. Under "Project Tasks" → "teensy" → "General":
   - Click "Build" to compile
   - Click "Upload" to flash the firmware

## Serial Protocol

DOB Terminal implements the VT-AC protocol, a custom command set for controlling display output, cursor positioning, colors, and audio. 

For complete protocol documentation, command reference, and usage examples, see:

**[VT-AC Protocol Documentation](https://github.com/acwright/VT-AC)**

## Project Structure

```
DOB Terminal/
├── src/
│   └── main.cpp           # Main application logic
├── lib/
│   └── DOBTerminal/
│       ├── characters.h   # 8x8 character bitmap definitions
│       ├── constants.h    # System constants and modes
│       ├── notes.h        # Musical note frequency table
│       ├── pins.h         # Hardware pin assignments
│       └── DOBTerminal.h  # Main library header
├── platformio.ini         # Build configuration
└── README.md
```

## License

See [LICENSE](LICENSE) for details.

## Author

Created by A.C. Wright © 2026