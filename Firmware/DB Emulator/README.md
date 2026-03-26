# DB Emulator

A 65C02 CPU emulator firmware for the Teensy 4.1 microcontroller. This project provides a complete hardware-accelerated 65C02 emulation platform with support for RAM, ROM, cartridge loading, I/O devices, and network control.

## Overview

DB Emulator transforms a Teensy 4.1 into a powerful 65C02 computer system emulator. It emulates the complete 65C02 CPU instruction set, memory management, and provides hardware interfaces for keyboards, joysticks, and network connectivity. The emulator can be controlled via serial terminal or through the embedded [web interface](html/README.md). Audio (SID) and video (TMS9918A) are streamed over USB to the browser for real-time rendering.

### Features

- **Complete 65C02 CPU Emulation**: Accurate cycle-by-cycle emulation using [vrEmu6502](https://github.com/visrealm/vrEmu6502)
- **Memory Management**: 
  - Base RAM: up to 32KB (configurable)
  - Banked RAM: 32KB (default), up to 512KB with PSRAM installed
  - ROM support (up to 24KB)
  - Cartridge loading (16KB)
- **I/O Support**:
  - USB Keyboard input
  - USB Joystick support (Xbox 360/One controllers)
  - Sound Card (SID — MOS 6581 emulation, streamed to web interface)
  - Video Card (TMS9918A emulation, streamed to web interface)
- **Storage**:
  - SD card support for loading ROMs, cartridges, and programs
  - Memory snapshot functionality
- **Network Features**:
  - Ethernet connectivity via [QNEthernet](https://github.com/ssilverman/QNEthernet)
  - mDNS discovery (hostname: `6502.local`)
  - Embedded web server with REST API
  - Embedded web control interface (served from SD card)
- **Audio/Video Streaming**:
  - SID register writes streamed over USB to browser for Web Audio API synthesis
  - TMS9918A register writes streamed over USB to browser for canvas rendering
  - Real-time AV via Web Serial API (Chrome or Edge required)
- **Browser Keyboard Input**: Forwards keyboard events from the web interface to the emulator via REST API
- **Control Options**:
  - Serial terminal interface (115200 baud)
  - Physical buttons (Run/Stop, Step, Reset, Frequency)
  - Web API control
- **Variable Clock Speed**: Adjustable CPU frequency from very slow to maximum speed
- **Real-Time Clock**: TimeLib integration for timestamped operations

### Hardware Variants

The firmware supports two development board configurations:

- **DevBoard 0** (`devboard_0`): Original development board
- **DevBoard 1** (`devboard_1`): Latest revision with improved layout and additional features (recommended)

## Prerequisites

### Hardware Requirements

- **Teensy 4.1** microcontroller
- **Ethernet module** (built-in on Teensy 4.1)
- **MicroSD card** (optional, for loading ROMs/programs)
- **Development board** compatible with one of the supported configurations
- **USB cable** for programming and serial communication
- **Optional**:
  - USB keyboard
  - USB joystick (Xbox 360/One)
  - PSRAM chip (for extended memory support)

### Software Requirements

- **PlatformIO Core** or **PlatformIO IDE** (VS Code extension)
- **Python 3.x** (usually installed with PlatformIO)
- **Git** (for cloning dependencies)
- **Chromium-based browser** (Chrome or Edge) required for audio/video streaming via Web Serial API
- **Serial terminal software** (optional):
  - CoolTerm (recommended)
  - Arduino Serial Monitor
  - PlatformIO Serial Monitor
  - screen/minicom (Linux/macOS)

### Development Tools (Optional)

- **Visual Studio Code** with PlatformIO extension
- **Teensy Loader Application** (for manual flashing)

## Installation

### 1. Install PlatformIO

#### Option A: PlatformIO IDE (Recommended)

1. **Install Visual Studio Code** from [code.visualstudio.com](https://code.visualstudio.com/)

2. **Install PlatformIO IDE extension**:
   - Open VS Code
   - Go to Extensions (Ctrl+Shift+X / Cmd+Shift+X)
   - Search for "PlatformIO IDE"
   - Click Install

#### Option B: PlatformIO Core (Command Line)

```bash
# macOS/Linux
python3 -c "$(curl -fsSL https://raw.githubusercontent.com/platformio/platformio/master/scripts/get-platformio.py)"

# Or using pip
pip install -U platformio
```

### 2. Clone or Navigate to Project

```bash
cd /path/to/6502/Firmware/DB\ Emulator
```

### 3. Install Dependencies

PlatformIO will automatically install the required libraries on first build:

- **[QNEthernet](https://github.com/ssilverman/QNEthernet)** (v0.27.0+): Ethernet library for Teensy 4.1
- **[ArduinoJson](https://github.com/bblanchon/ArduinoJson)** (v7.0.4+): JSON parsing and generation
- **[AsyncWebServer_Teensy41](https://github.com/khoih-prog/AsyncWebServer_Teensy41)** (v1.7.0+): Asynchronous web server

Additional dependencies included in the project:
- **[vrEmu6502](https://github.com/visrealm/vrEmu6502)**: 6502 CPU emulator core (included in `lib/6502/vrEmu6502/`)
- **TimeLib**: Real-time clock support
- **Bounce2**: Button debouncing library
- **SD**: SD card file system
- **EEPROM**: Non-volatile storage
- **USBHost_t36**: USB host support for Teensy

## Configuration

### Select Your Development Board

Edit `platformio.ini` or use the PlatformIO environment selector to choose your board configuration:

- **devboard_0**: Original development board
- **devboard_1**: Latest revision with improved layout and additional features (recommended)

### Build Flags

You can enable/disable features by uncommenting build flags in `platformio.ini`:

```ini
build_flags = 
    -D DEVBOARD_1           ; Board version
    -D USB_TRIPLE_SERIAL    ; Enable triple serial ports (SerialUSB1 + SerialUSB2 for AV streaming)
    ; -D MOUSE_DEBUG        ; Enable mouse debugging output
    ; -D JOYSTICK_DEBUG     ; Enable joystick debugging output
    ; -D KEYBOARD_DEBUG     ; Enable keyboard debugging output
    -D MEM_EXTMEM           ; Use external PSRAM
```

### SD Card Setup (Optional)

1. **Format SD card** as FAT32
2. **Create directories** on the SD card:
   ```
   /ROMs/       - BIOS and operating system ROMs
   /Carts/      - Cartridge images
   /Programs/   - User programs
   /Snapshots/  - Memory snapshots
   /WWW/        - Embedded web application (See README in /html folder)
   ```
3. **Place ROM or PRG files** (.bin or .prg format) in appropriate directories

**Auto-boot Configuration**:
- Place `BIOS.bin` in the `ROMs` directory for automatic loading

## Building and Uploading

### Using PlatformIO IDE (VS Code)

1. **Open the project** in VS Code (File → Open Folder)

2. **Select your environment**:
   - Click the environment selector in the status bar
   - Choose `devboard_0`, or `devboard_1`

3. **Build the project**:
   - Click the ✓ (Build) button in the status bar
   - Or: Terminal → Run Build Task
   - Or: Press `Ctrl+Alt+B` / `Cmd+Alt+B`

4. **Upload to Teensy**:
   - Connect Teensy 4.1 via USB
   - Click the → (Upload) button in the status bar
   - Or: Use the upload task from the command palette
   - The Teensy will automatically enter bootloader mode

### Using PlatformIO Core (Command Line)

```bash
# Build for DevBoard 1
platformio run -e devboard_1

# Upload to Teensy
platformio run -e devboard_1 --target upload

# Build and upload
platformio run -e devboard_1 -t upload

# Open serial monitor
platformio device monitor -b 115200

# Build, upload, and monitor
platformio run -e devboard_1 -t upload && platformio device monitor -b 115200
```

### Manual Upload with Teensy Loader

1. Build the firmware using PlatformIO
2. Locate the compiled `.hex` file in `.pio/build/devboard_X/firmware.hex`
3. Open Teensy Loader application
4. Load the `.hex` file
5. Press the button on the Teensy to enter bootloader mode
6. Click "Program" in Teensy Loader

## Usage

### Serial Terminal Control

Connect to the Teensy via serial terminal at **115200 baud**:

```bash
# Using PlatformIO
platformio device monitor -b 115200

# Using screen (macOS/Linux)
screen /dev/ttyACM0 115200

# Using CoolTerm
# Set port to Teensy, baud rate to 115200
```

### Serial Commands

Once connected, you'll see the emulator banner and status. Available commands:

#### Execution Control
- **`R`** - Run/Stop: Toggle emulator execution
- **`S`** - Step: Execute one CPU instruction (when stopped)
- **`K`** - Tick: Execute one clock cycle (when stopped)
- **`T`** - Reset: Hardware reset of the emulator

#### Memory Management
- **`A`** - Toggle RAM: Enable/disable RAM
- **`O`** - Toggle ROM: Enable/disable ROM
- **`L`** - Toggle Cart: Enable/disable cartridge

#### File Management
- **`M`** - List ROMs: Show available ROM files from SD card
- **`C`** - List Carts: Show available cartridge files
- **`U`** - List Programs: Show available user programs
- **`0-7`** - Load File: Load the selected file (after listing)
- **`[`** - Previous Page: Navigate file lists
- **`]`** - Next Page: Navigate file lists

#### Configuration
- **`+`** - Increase Frequency: Speed up CPU clock
- **`-`** - Decrease Frequency: Slow down CPU clock

#### Information & Debugging
- **`F`** - Info: Display system information and status
- **`G`** - Log: Show current CPU state
- **`P`** - Snapshot: Save current RAM state to SD card

### Web Control Interface

1. **Ensure Teensy is connected** to your network via Ethernet

2. **Note the IP address** from the serial output on boot

3. **Access via mDNS** (if supported):
   ```
   http://6502.local/
   ```

4. **Or access via direct IP**:
   ```
   http://192.168.1.XXX/
   ```

5. **Use the web interface** to control the emulator (see [Web Interface documentation](html/README.md))

#### Audio/Video Streaming

The Teensy streams SID and TMS9918A register writes over USB (SerialUSB2) to the browser for real-time audio synthesis and video rendering.

- The Teensy must be USB-connected to the same machine running the browser
- Click **Connect AV** in the VIDEO panel to open the USB serial link
- Requires **Chrome or Edge** (Web Serial API); requires HTTPS or localhost
- Audio and video are rendered entirely in the browser; the Teensy streams register writes only
- Click **Mute** to toggle audio output

**Alternative: External Hardware Rendering**

In addition to browser-based AV streaming via USB, the emulator supports streaming AV data to external hardware devices via the **HWSERIAL** interface. This allows connecting dedicated hardware (such as external sound synthesizers or video display units) for rendering audio and video output. The HWSERIAL port uses the same AV streaming protocol as SerialUSB2, enabling hardware devices to receive SID and TMS9918A register writes for real-time rendering.

### Physical Controls

If your development board has physical buttons:

- **INT/CLK Button**: Change clock frequency
- **STEP Button**: Execute one instruction
- **RUN/STOP Button**: Toggle execution
- **RESET Button**: Hardware reset (DevBoard 1 only)

## API Endpoints

The embedded web server provides RESTful API endpoints:

### GET /info
Returns system information and CPU state in JSON format:
```json
{
  "address": 65535,
  "cartEnabled": true,
  "cartFile": "game.bin",
  "cpuAccumulator": 0,
  "cpuOpcodeCycle": 0,
  "cpuPC": 65520,
  "cpuStackPointer": 255,
  "cpuStatus": 32,
  "cpuX": 0,
  "cpuY": 0,
  "data": 234,
  "freqLabel": "1 MHz",
  "freqPeriod": 1,
  "ipAddress": "192.168.1.100",
  "isRunning": false,
  "lastSnapshot": 1234567890,
  "programFile": "",
  "romFile": "BIOS.bin",
  "rtc": 1737590400,
  "rw": 1,
  "soundEnabled": true,
  "version": "1.0",
  "videoEnabled": true,
  "avStreamConnected": false
}
```

### GET /memory?target={ram|rom}&page={n}
Returns memory dump (1024 bytes per page):
- `target`: `ram` or `rom`
- `page`: Page number (0-31 for RAM, 0-23 for ROM)
- Optional: `formatted=true` for hex-formatted output

### GET /storage?target={rom|cart|program}&page={n}
Lists files in storage:
- `target`: `rom`, `cart`, or `program`
- `page`: Page number for pagination
- Returns JSON array of filenames

### GET /load?target={rom|cart|program}&filename={name}
Loads a file from SD card:
- `target`: Memory region to load into
- `filename`: Name of file to load

### GET /control?command={cmd}
Executes control command:
- `command`: Single letter command (same as serial commands)
- Examples: `r` (run/stop), `s` (step), `t` (reset), `+` (inc freq)

### GET /keyboard?action={down|up}&keycode={XX}
Forwards browser keyboard events to the emulator:
- `action`: `down` or `up` (required; returns 400 if missing or invalid)
- `keycode`: 1–2 character hex string representing a USB HID keycode (e.g. `04` for 'a') (required; returns 400 if missing)
- Returns 200 on success

**Note:** Audio/video data flows via Web Serial API (USB SerialUSB2), not REST. See the Audio/Video Streaming section above.

## Troubleshooting

### Build Errors

**Problem**: Library dependencies not found

**Solutions**:
- Run `platformio lib install` to fetch dependencies
- Delete `.pio` folder and rebuild
- Check internet connection for library downloads

**Problem**: Compilation errors with PSRAM

**Solutions**:
- Ensure `MEM_EXTMEM` flag matches your hardware configuration
- Check PSRAM wiring and connections

### Upload Issues

**Problem**: Teensy not detected

**Solutions**:
- Install Teensy Udev rules (Linux) or drivers (Windows)
- Press the physical button on Teensy to enter bootloader mode
- Try a different USB cable or port
- Check `platformio device list` to see connected devices

**Problem**: Upload fails or times out

**Solutions**:
- Manually press Teensy button during upload
- Use Teensy Loader application instead
- Update Teensy udev rules/drivers

### Runtime Issues

**Problem**: Emulator doesn't start or crashes

**Solutions**:
- Check serial output for error messages
- Verify SD card is properly formatted (FAT32)
- Ensure ROM files are valid 65C02 binaries
- Reset via serial command `T` or power cycle

**Problem**: Network features not working

**Solutions**:
- Verify Ethernet cable is connected
- Check DHCP is available on your network
- Look for IP address in serial output
- Try accessing via direct IP instead of mDNS
- Ensure firewall allows port 80

**Problem**: USB devices not recognized

**Solutions**:
- Check device compatibility (Xbox controllers work best)
- Try different USB ports on the hub
- Verify USB host power supply is adequate
- Enable debug flags to see USB enumeration

**Problem**: SD card files not loading

**Solutions**:
- Verify SD card is formatted as FAT32
- Check file permissions and naming
- Ensure directories exist: `/ROMs/`, `/Carts/`, `/Programs/`
- Files should have `.bin` extension
- Try reinserting SD card

### AV Streaming Issues

**Problem**: Connect AV button not visible

**Solutions**:
- Must use Chrome or Edge (Web Serial API is not supported in Firefox or Safari)
- The Web Serial API requires a **secure context**. When accessing via `http://6502.local` (plain HTTP), Chrome hides the API even though your browser supports it. Choose one of the following workarounds:

  **Option 1 — Chrome flag (easiest, for local development)**
  1. In Chrome, navigate to `chrome://flags/#unsafely-treat-insecure-origin-as-secure`
  2. Add `http://6602.local` to the list and set it to **Enabled**
  3. Relaunch Chrome

  **Option 2 — Serve over HTTPS**
  Configure the Teensy's web server with a self-signed TLS certificate. The browser will show a certificate warning; accept it once and the API will become available.

  **Option 3 — Local proxy via localhost**
  Run a local reverse proxy (e.g. `nginx` or `caddy`) that forwards `http://localhost` to `http://6602.local`. Because `localhost` is always treated as a secure context, the Web Serial API will be available.

**Problem**: No video or audio after connecting

**Solutions**:
- Verify USB cable supports data (not charge-only)
- Check `SerialUSB2` is initialized (`USB_TRIPLE_SERIAL` build flag must be set)
- Try disconnecting and reconnecting

**Problem**: Audio glitches or dropouts

**Solutions**:
- Lower the CPU frequency
- Ensure the browser tab is not throttled or backgrounded
- Close other audio-intensive applications

## Development

### Project Structure

```
DB Emulator/
├── lib/
│   └── 6502/                 # Custom 6502 emulation library
│       ├── 6502.h            # Main library header
│       ├── constants.h       # System constants
│       ├── macros.h          # Utility macros
│       ├── pins.h            # Pin definitions
│       ├── CPU/              # CPU emulator wrapper
│       │   └── [vrEmu6502](https://github.com/visrealm/vrEmu6502)/        # Core 6502 emulator
│       ├── RAM/              # RAM emulation
│       ├── ROM/              # ROM emulation
│       ├── Cart/             # Cartridge support
│       └── IO/               # I/O device emulation
│           ├── AVStream.h    # AV streaming protocol (SerialUSB2)
│           ├── SoundCard.*   # SID (MOS 6581) emulation
│           └── VideoCard.*   # TMS9918A emulation
├── src/
│   └── main.cpp              # Main firmware code
├── include/                  # Additional headers
├── test/                     # Unit tests
├── html/                     # Web interface source
└── platformio.ini            # PlatformIO configuration
```

### Adding Features

1. **New I/O Device**: Extend the `IO` class in `lib/6502/IO/`
2. **Memory Extensions**: Modify memory maps in `constants.h`
3. **New Commands**: Add cases to `onCommand()` in `main.cpp`
4. **Web API**: Add routes in `initServer()` function

### Memory Map

```
$0000 - $00FF : Zero Page
$0100 - $01FF : Stack
$0200 - $02FF : Input Buffer
$0300 - $03FF : Kernal Variables
$0400 - $07FF : User Variables
$0800 - $7FFF : Program RAM (30KB)
$8000 - $83FF : I/O Slot 0 (1KB) - RAM Card
$8400 - $87FF : I/O Slot 1 (1KB) - RAM Card
$8800 - $8BFF : I/O Slot 2 (1KB) - RTC Card
$8C00 - $8FFF : I/O Slot 3 (1KB) - Storage Card
$9000 - $93FF : I/O Slot 4 (1KB) - Serial Card
$9400 - $97FF : I/O Slot 5 (1KB) - GPIO Card
$9800 - $9BFF : I/O Slot 6 (1KB) - Sound Card (SID)
$9C00 - $9FFF : I/O Slot 7 (1KB) - Video Card (TMS9918A)
$A000 - $BFFF : ROM (8KB)
$C000 - $FFFF : ROM/Cartridge (16KB)
```

**Note**: The cartridge space ($C000-$FFFF) can be used for either ROM or cartridge data depending on configuration.

### Debug Output

Enable debug flags in `platformio.ini` for verbose logging:

```ini
-D KEYBOARD_DEBUG    ; Log all keyboard events
-D JOYSTICK_DEBUG    ; Log joystick button presses
```

## Performance

- **Maximum Emulation Speed**: 1 MHz (Teensy 4.1 @ 600 MHz)
- **Accurate Timing**: Down to microsecond precision
- **Memory**: 32KB base RAM + 32KB banked RAM (default), up to 544KB total with PSRAM (32KB base + 512KB banked)
- **Storage**: Limited only by SD card size

## Related Documentation

- [Web Interface (html/)](html/README.md)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [Teensy 4.1 Documentation](https://www.pjrc.com/store/teensy41.html)
- [6502 CPU Reference](http://www.6502.org/)

## License

See LICENSE file in project root.

## Credits

- Created by A.C. Wright © 2024
- Based on [vrEmu6502](https://github.com/visrealm/vrEmu6502) by Troy Schrapel
- Uses [QNEthernet](https://github.com/ssilverman/QNEthernet) library by Shawn Silverman
- Uses [AsyncWebServer_Teensy41](https://github.com/khoih-prog/AsyncWebServer_Teensy41) library by Khoi Hoang
- Uses [ArduinoJson](https://github.com/bblanchon/ArduinoJson) library by Benoit Blanchon

## Version History

- **v1.0.0**: Initial release with full 65C02 emulation, network support, and web API