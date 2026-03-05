# SCP Controller

An SPI Controller Peripheral for 6502-based computer systems. This firmware runs on an ATmega328P microcontroller and acts as a memory-mapped I/O device, providing the 6502 CPU with access to SPI peripherals including SD cards, Flash memory, and external SPI devices.

## Features

- **Memory-Mapped I/O Interface**: Two registers accessible from 6502 address space (DATA and CTRL)
- **Multiple SPI Devices**: Support for up to 3 SPI slave devices via chip select lines
  - CS0: SD Card
  - CS1: Flash Memory
  - CS2: External SPI Header
- **Dual Clock Speeds**: 
  - Fast mode: 4 MHz (normal operation)
  - Slow mode: 400 kHz (SD card initialization)
- **Auto-Transmit Mode**: Automatically send dummy bytes when reading data, useful for consecutive reads
- **Status Monitoring**: Real-time monitoring of BUSY, Card Detect (CD), and Write Protect (WP) flags
- **6502 Synchronization**: PHI2-synchronized interrupt-driven interface for reliable communication

## Hardware Overview

### Microcontroller
- **MCU**: ATmega328P @ DIP28 package
- **Architecture**: AVR 8-bit
- **Framework**: Arduino

### Pin Connections

| Pin Group | ATmega328P Pins | Function |
|-----------|-----------------|----------|
| **6502 Data Bus** | PD0-PD7 (D0-D7) | Bidirectional 8-bit data bus |
| **6502 Control** | A0 (RS0) | Register select (0=DATA, 1=CTRL) |
| | A1 (PHI2) | 6502 clock - triggers interrupt |
| | A2 (CSB) | Chip select (active low) |
| | A3 (RWB) | Read/Write signal |
| **SD Card Status** | A4 (CD) | Card detect input (active low) |
| | A5 (WP) | Write protect input (active low) |
| **SPI Interface** | 11 (MOSI) | SPI Master Out Slave In |
| | 12 (MISO) | SPI Master In Slave Out |
| | 13 (SCK) | SPI Clock |
| **Chip Selects** | 8 (CS0) | SD Card chip select |
| | 9 (CS1) | Flash Memory chip select |
| | 10 (CS2) | External SPI header chip select |

## Register Map

The SCP Controller presents two 8-bit registers to the 6502:

### DATA Register ($0)
**Address Offset**: 0  
**Access**: Read/Write

| Bit 7-0 | Description |
|---------|-------------|
| DATA[7:0] | SPI data byte to transmit or last received byte |

**Write Operation**: Initiates an SPI transfer with the written byte  
**Read Operation**: Returns the last received SPI byte. If AUTO-TX is enabled, automatically starts a new transfer with dummy byte $FF

### CTRL Register ($1)
**Address Offset**: 1  
**Access**: Read/Write (some bits read-only)

| Bit | Name | R/W | Description |
|-----|------|-----|-------------|
| 7 | BUSY | R | SPI transfer in progress (1=busy, 0=idle) |
| 6 | CD | R | SD Card detected (1=card present, 0=no card) |
| 5 | WP | R | Write protect status (1=protected, 0=writable) |
| 4 | SS1 | R/W | Slave select bit 1 |
| 3 | SS0 | R/W | Slave select bit 0 |
| 2 | AUTO-TX | R/W | Auto-transmit on read (1=enabled, 0=disabled) |
| 1 | SLOW CLK | R/W | SPI clock speed (1=400kHz, 0=4MHz) |
| 0 | SELECT | R/W | Enable chip select (1=active, 0=inactive) |

**Slave Select Encoding** (SS1:SS0):
- `00`: CS0 - SD Card
- `01`: CS1 - Flash Memory
- `10`: CS2 - External SPI Header
- `11`: Reserved (no device selected)

## Building the Firmware

### Prerequisites

1. **PlatformIO**: Install PlatformIO Core or PlatformIO IDE
   ```bash
   # Via pip
   pip install platformio
   
   # Or via Homebrew (macOS)
   brew install platformio
   ```

2. **MiniPro Programmer**: Required for uploading firmware
   ```bash
   # macOS
   brew install minipro
   
   # Linux
   # Follow installation instructions at: https://gitlab.com/DavidGriffith/minipro
   ```

### Build Commands

```bash
# Build for ATmega328P (default)
pio run

# Build for ATmega328 (no 'P' variant)
pio run -e atmega328

# Clean build files
pio run --target clean

# Verbose build output
pio run -v
```

### Upload Firmware

The upload process programs both the firmware and fuse settings using a MiniPro programmer:

```bash
# Upload to ATmega328P
pio run --target upload

# Upload to ATmega328
pio run -e atmega328 --target upload
```

The upload command performs two operations:
1. Programs the compiled firmware to flash memory
2. Configures fuse bits from `fuses.cfg`

### Fuse Configuration

Fuse settings are defined in `fuses.cfg`:
```
lfuse = 0xFF  # Low fuse
hfuse = 0xFF  # High fuse
efuse = 0xFF  # Extended fuse
lock = 0xFF   # Lock bits
```

**Note**: Verify fuse values match your hardware configuration before uploading. Incorrect fuse settings can brick your microcontroller.

## Usage Examples

### Example 1: Initialize SD Card

```assembly
; Set slow clock for SD card initialization
LDA #$03        ; SLOW_CLK=1, SELECT=1
STA CTRL        ; Enable CS0 (SD Card) with slow clock

; Send CMD0 (reset)
LDA #$40
JSR SPI_SEND
LDA #$00
JSR SPI_SEND
; ... (continue with initialization sequence)

; Switch to fast clock after initialization
LDA #$01        ; SLOW_CLK=0, SELECT=1
STA CTRL
```

### Example 2: Read Multiple Bytes with Auto-Transmit

```assembly
; Enable auto-transmit for faster consecutive reads
LDA #$05        ; AUTO_TX=1, SELECT=1
STA CTRL        ; CS0 active with auto-transmit

; First byte (manual trigger)
LDA #$FF
STA DATA        ; Start transfer

:WAIT1
LDA CTRL
AND #$80        ; Check BUSY flag
BNE :WAIT1

; Subsequent bytes (auto-triggered on read)
LDX #$00
:LOOP
LDA DATA        ; Reading DATA automatically sends next $FF
STA BUFFER,X    ; Store received byte
INX
CPX #$10        ; Read 16 bytes
BNE :LOOP

; Disable chip select
LDA #$00
STA CTRL
```

### Example 3: Select Flash Memory

```assembly
; Select Flash chip (CS1)
LDA #$09        ; SS1:SS0 = 01, SELECT=1
STA CTRL        ; Enable CS1 (Flash)

; Send command
LDA #$03        ; READ command
JSR SPI_SEND
; ... (continue with flash operations)
```

## Development Environment

This project is configured for development with:
- **Platform**: Atmel AVR
- **Framework**: Arduino
- **Board**: ATmega328P or ATmega328
- **Build System**: PlatformIO

## Project Structure

```
SCP Controller/
├── src/
│   └── main.cpp          # Main firmware source
├── include/              # Header files (if any)
├── lib/                  # Project libraries
├── test/                 # Unit tests
├── platformio.ini        # PlatformIO configuration
├── fuses.cfg            # AVR fuse settings
├── SPI.txt              # SPI implementation notes
└── README.md            # This file
```

## Design Notes

The controller uses PHI2-synchronized interrupts to ensure proper timing with the 6502 bus. The data bus is configured as input with pull-ups by default and only switches to output mode when the 6502 performs a read operation.

SPI transfers are blocking and set the BUSY flag during operation. The 6502 software should check the BUSY flag before initiating new transfers.

The Card Detect (CD) and Write Protect (WP) inputs use internal pull-ups and are active-low, suitable for direct connection to SD card socket switches.

## References

- Inspired by the VERA SPI controller implementation (see `SPI.txt`)
- 6502 bus timing reference: [W65C02S Datasheet](http://www.westerndesigncenter.com/wdc/documentation/w65c02s.pdf)
- SD Card SPI Mode: [SD Specifications Part 1 Physical Layer Simplified Specification](https://www.sdcard.org/)

## License

See the main repository LICENSE file for details.

## Contributing

This is a personal project for a 6502-based computer system. Feel free to fork and adapt for your own hardware.

