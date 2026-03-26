# DOB Video Display

DOB Video Display is the AV output firmware for the DOB (Dev Output Board). It emulates a TMS9918A Video Display Processor and a MOS 6581 SID audio synthesizer, driven in real time by AV packets from the DB Emulator over high-speed hardware serial.

## Features

- **TMS9918A VDP Emulator** - Full software emulation of all four TMS9918A display modes
- **320x240 Display with Borders** - 256×192 active area centered in a 320×240 canvas with TMS9918-style color borders
- **8-bit Indexed Framebuffer** - 76,800-byte indexed framebuffer with 16-entry RGB565 TMS9918A palette; half the RAM of a 16-bit framebuffer
- **SID Audio Emulator** - 3-voice MOS 6581 software synthesizer with triangle, sawtooth, pulse, and noise waveforms and full ADSR envelopes, output via 8-bit PWM at 44.1 kHz
- **AV Packet Protocol** - 4-byte AV packet stream received at 6 Mbps over hardware UART (Serial1) from the DB Emulator
- **Dual Serial Input** - USB Serial accepted as a fallback for testing without hardware
- **~60 FPS Rendering** - Full-frame re-render from VRAM state every ~17 ms

## Hardware

- **MCU**: Teensy 4.0 (600 MHz ARM Cortex-M7)
- **Display**: ILI9341 320×240 TFT LCD
- **Audio**: 8-bit PWM output (pin 14) into passive speaker via RC low-pass filter
- **Communication**: USB Serial + hardware UART Serial1 at 6 Mbps

### Audio Hardware Notes

The audio output on pin 14 is a PWM signal with a ~2.34 MHz carrier. To drive a small 8Ω speaker you need an RC low-pass filter between pin 14 and the speaker:

- **R**: 1 kΩ
- **C**: 3.3 nF to ground (f_c ≈ 48 kHz)

This removes the PWM carrier while passing the full audio band.

## AV Packet Format

All communication uses 4-byte packets: `{ 0xAA, type, a, b }`

| Type | Value | Description |
|------|-------|-------------|
| `AV_TYPE_SOUND`       | `0x01` | SID register write: `a` = register (0x00–0x1F), `b` = value |
| `AV_TYPE_VIDEO_DATA`  | `0x02` | VRAM data write at current address pointer (auto-increments) |
| `AV_TYPE_VIDEO_REG`   | `0x03` | TMS9918A register write: `a` = register (0–7), `b` = value |
| `AV_TYPE_VIDEO_ADDR`  | `0x04` | Set VRAM address: `a` = high 6 bits, `b` = low 8 bits |
| `AV_TYPE_RESET`       | `0xFE` | Reset all VRAM, TMS9918A registers, SID state, and address pointer |

## Display Modes

| M1 | M2 | M3 | Mode |
|----|----|----|------|
| 0  | 0  | 0  | Graphics I — 32×24 tiles, per-8-char color |
| 0  | 0  | 1  | Graphics II — 32×24 tiles, per-pixel color |
| 1  | 0  | 0  | Text — 40×24 tiles, 6px wide, R7 fg/bg |
| 0  | 1  | 0  | Multicolor — 64×48 fat pixels (4×4 each) |

Sprites are rendered in all modes except Text (up to 32 sprites, max 4 visible per scanline, 8×8 or 16×16, with optional magnification).

## Installation

### Prerequisites

- [PlatformIO](https://platformio.org/) - Cross-platform build system
- [Visual Studio Code](https://code.visualstudio.com/) (recommended) with PlatformIO extension
- Teensy 4.0 board
- ILI9341 compatible TFT display

### Hardware Setup

Connect the display and speaker to your Teensy 4.0 according to the pin definitions in `lib/DOBVideoDisplay/pins.h`.

## Building and Uploading

### Using PlatformIO CLI

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd "DOB Video Display"
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

## Project Structure

```
DOB Video Display/
├── src/
│   └── main.cpp              # Main firmware — VDP emulator, SID engine, AV packet processing
├── lib/
│   └── DOBVideoDisplay/
│       ├── constants.h       # AV protocol constants, display geometry, TMS9918A RGB565 palette
│       ├── pins.h            # Hardware pin assignments
│       └── DOBVideoDisplay.h # Main library header
├── platformio.ini            # Build configuration
└── README.md
```

## License

See [LICENSE](LICENSE) for details.

## Author

Created by A.C. Wright © 2026