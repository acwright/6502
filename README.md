6502
====

## A 65c02 Based 8-Bit Retro Computer Project

![The 6502 Project](./Assets/Images/6502.png?raw=true)
*Shown above are some of the systems that can be assembled from the 6502 project*

---

## Table of Contents

- [Overview](#overview)
- [Target Audience](#target-audience)
- [Design Philosophy](#design-philosophy)
- [Computer Systems](#computer-systems)
  - [The COB (Computer on a Backplane)](#the-cob-computer-on-a-backplane)
  - [The DEV (Development Environment Vehicle)](#the-dev-development-environment-vehicle)
  - [The VCS (Video Computer System)](#the-vcs-video-computer-system)
- [System Specifications](#system-specifications)
- [Board Compatibility Matrix](#board-compatibility-matrix)
- [Architecture Overview](#architecture-overview)
- [Hardware](#hardware)
  - [Boards](#boards)
  - [Cards](#cards)
  - [Helpers](#helpers)
  - [Carts](#carts)
- [Firmware](#firmware)
- [Software & Related Projects](#software--related-projects)
- [CAD](#cad)
- [Production](#production)
- [Schematics](#schematics)
- [Libraries](#libraries)
- [Development Workflow](#development-workflow)
- [Contributing](#contributing)
- [Licensing](#licensing)

---

## Overview

This project aims to create multiple retro-style 8-bit computers based on the 65(c)02 microprocessor. The project is centered around a number of custom-designed PCBs that can be combined in various configurations to create different computer systems based on a common [memory map](./Assets/Documents/Memory%20Map/Memory%20Map.pdf). The designs are inspired by classic 8-bit computers such as the KIM-1, Commodore 64, and Apple II, but with modern enhancements and flexibility.

The project is designed using **KiCad 7.0+** for PCB design and is **open-source** under the CERN Open Hardware Licence Version 2 - Permissive, allowing enthusiasts to contribute and modify the designs freely. The boards are manufactured by **JLCPCB** and use a combination of surface-mount and through-hole components for optimal assembly flexibility.

### Key Features

- **Modular Design**: Mix and match boards, cards, and helpers to create custom configurations
- **Multiple System Types**: Build a desktop computer (COB), development emulator (DEV), or gaming console (VCS)
- **Common Memory Map**: All systems share a consistent memory architecture for software compatibility
- **Modern Manufacturing**: Optimized for JLCPCB's assembly services with production-ready files included
- **Open-Source Hardware**: Complete KiCad schematics, PCB layouts, and 3D models available
- **Extensive Firmware**: PlatformIO-based firmware for microcontrollers with networking, USB, and storage support
- **Flexible I/O**: Support for PS/2 keyboards, joysticks, serial terminals, video output (composite/VGA), and more

---

## Target Audience

This project is designed for:

- **Retro Computing Enthusiasts**: Those who appreciate classic 8-bit computer architecture and want to build modern replicas
- **Hobbyists & Makers**: Electronics enthusiasts interested in hands-on PCB assembly and microcontroller programming
- **Educators**: Teachers and students learning about computer architecture, digital logic, and embedded systems
- **Game Developers**: Retro game developers seeking a flexible platform for 8-bit game development
- **Hardware Hackers**: Engineers wanting to experiment with custom peripherals and system extensions
- **Open-Source Contributors**: Developers interested in contributing to an active hardware/software project

**Skill Level**: Intermediate to advanced electronics knowledge recommended. Basic soldering skills, familiarity with microcontroller programming, and understanding of digital logic are helpful but not strictly required for simple builds.

---

## Design Philosophy

The 6502 project is guided by several core principles:

### 1. **Modularity & Flexibility**
Rather than designing monolithic systems, the project uses interchangeable boards, cards, and helpers that can be combined in different ways. This allows a single set of PCBs to create vastly different computer systems.

### 2. **Modern Manufacturing Meets Retro Design**
While the systems are inspired by 1970s-1980s computers like the KIM-1 and Commodore 64, they leverage modern PCB manufacturing capabilities (SMD components, JLCPCB assembly) and contemporary components (Teensy microcontrollers, Pi Pico, modern VIAs).

### 3. **Common Architecture**
All systems share a common memory map and bus structure. This ensures that software written for one system can often run on another with minimal modifications, and peripherals remain compatible across configurations.

### 4. **Open-Source Everything**
Complete transparency: all schematics, PCB layouts, 3D models, firmware source code, and production files are available under permissive open-source licenses. The community can learn from, modify, and improve any aspect of the project.

### 5. **Real Hardware + Emulation**
The project supports both real 65(c)02 hardware and accurate cycle-by-cycle emulation (via Teensy 4.1 + vrEmu6502). This provides flexibility for software development and testing without requiring physical chips.

### 6. **Educational Focus**
Documentation, schematics, and code are written with clarity in mind. The project serves as a learning platform for understanding computer architecture, digital design, and embedded programming.

---

## Computer Systems

The project supports the creation of several different computer systems, each optimized for different use cases. All systems share a common memory map and bus architecture, ensuring software compatibility where applicable.

### The COB (Computer on a Backplane)

**Type:** Full-featured desktop computer  
**Complexity:** Advanced  
**Best For:** Enthusiasts wanting a complete modular 8-bit system

![The COB](./CAD/Bases/The%20COB/The%20COB.png?raw=true)
![The COB](./Assets/Images/COB.png?raw=true)

The COB is the most versatile and expandable system configuration, featuring a backplane architecture that allows multiple peripheral cards to be added or swapped. This design provides maximum flexibility for experimentation and expansion.

**Required Components:**
  * 1x Backplane (provides card slot interconnection)
  * 1x Backplane Pro (enhanced backplane with clock/reset/power)
  * 1x Backplane Helper (adds 2 additional card slots)
  * 1x CPU Card (65c02) or 1x CPU Card Pro (65816, **UNTESTED**)
  * 1x Memory Card (32KB RAM + 32KB ROM with address decoding)
  * 1x Video Card (composite via TMS9918A) or 1x VGA Card (VGA via Pico9918) or 1x Video Card Pro (composite via ATmega1284p, **UNTESTED**)
  * 1x Sound Card (ARMSID audio module)
  * 1x Storage Card (CompactFlash) or 1x Storage Card Pro (SD + Flash + SPI, **UNTESTED**)
  * 1x Serial Card (6551 ACIA) or 1x Serial Card Pro (enhanced serial)
  * 1x RAM Card (512KB banked RAM)
  * 1x RTC Card (Real-time clock via DS1511Y)
  * 1x GPIO Card (6522 VIA for I/O)
    * 1x Keyboard Encoder Helper (ATmega1284p) or 1x PS2 Helper (ATmega328p)

**Key Features:**
- Real 65c02 (or 65816) CPU running at variable clock speeds (0.5 MHz - 8 MHz typical)
- Up to 544KB total RAM (32KB base + 512KB banked)
- Multiple video output options (composite or VGA)
- Persistent storage via CompactFlash or SD card
- Real-time clock for timestamped operations
- Serial terminal connectivity

---

### The DEV (Development Environment Vehicle)

**Type:** Emulation-based development system  
**Complexity:** Intermediate  
**Best For:** Software developers and debugging workflows

![The DEV](./CAD/Bases/The%20DEV/The%20DEV.png?raw=true)
![The DEV](./Assets/Images/DEV.png?raw=true)

The DEV replaces the physical 65(c)02 CPU with a Teensy 4.1 microcontroller running cycle-accurate emulation via [vrEmu6502](https://github.com/visrealm/vrEmu6502).

**Required Components:**
  * 1x Dev Board (Teensy 4.1-based 65c02 emulator)
  * 1x Dev Output Board (2.4" LCD with TMS9918A VDP and SID audio emulation)

**Key Features:**
- Cycle-accurate 65C02 emulation (no physical CPU needed)
- Teensy 4.1 platform (600 MHz ARM Cortex-M7)
- Variable CPU speed
- Ethernet connectivity via QNEthernet library
- Web-based control interface (DB Emulator) accessible at `http://6502.local`
- SD card support for loading ROMs, programs, and saving snapshots
- USB-C keyboard and joystick input (Xbox 360/One controllers)
- Serial terminal interface (115200 baud)
- mDNS discovery
- Real-time clock support
- Hardware buttons (Run/Stop, Step, Reset, Frequency)

**Development Advantages:**
- No need to burn ROMs for testing
- Step through code cycle-by-cycle
- Load programs instantly from SD card or network
- Monitor memory in real-time via web interface
- Snapshot memory (Save memory state to SD card)

**See Also:** [Firmware/DB Emulator/README.md](./Firmware/DB%20Emulator/README.md) for detailed setup instructions.

**IMPORTANT:**

The DEV does not output bus compatible signals (or any signals) for the 6502 backplane system with the current firmware and instead relies entirely on emulation. There is a bus connector and a card slot available on the Dev Board along with level shifters for all signals (see schematic). These are up to end-user to implement if you want to try to talk to real hardware using the bus connections. In past versions of the firmware (see commit history) I have worked with real hardware and it does work so give it try if you would like!

---

### The VCS (Video Computer System)

**Type:** Retro gaming console  
**Complexity:** Intermediate  
**Best For:** Retro game development and gaming

![The VCS](./CAD/Bases/The%20VCS/The%20VCS.png?raw=true)
![The VCS](./Assets/Images/VCS.png?raw=true)

A game-focused configuration designed for playing and developing retro-style games. Features cartridge-based ROM loading, multiple input options, and dedicated video/audio output.

**Required Components:**
  * 1x Main Board (65c02 CPU + 32KB RAM + 32KB ROM)
  * 1x Input Board (Matrix keyboard, PS/2 keyboard, controller ports)
  * 1x Output Board (VGA video via Pico9918 + ARMSID audio)
  * 1x ROM Cart (cartridge for game ROM images)

**Key Features:**
- Real 65c02 CPU
- Cartridge-based game loading (swappable ROM carts)
- Multiple input options:
  - Matrix keyboard
  - PS/2 keyboard
  - Atari 2600 compatible joysticks (via port header and Joystick Helper)
- VGA video output (via Pico9918)
- Audio output via ARMSID

**Typical Use Cases:**
- Playing homebrew 8-bit games
- Developing retro-style games
- Arcade-style gaming experiences
- Educational game programming

---

## System Specifications

Detailed specifications for each system configuration:

| Specification | COB | DEV | VCS |
|--------------|-----|-----|-----|
| **CPU** | 65c02 or 65816* | Teensy 4.1 (ARM Cortex-M7 emulating 65c02) | 65c02 |
| **Clock Speed** | 0.5-8 MHz (variable) | Emulated: 0 Hz to maximum | 0.5-8 MHz (variable) |
| **Base RAM** | 32KB (Memory Card) | Up to 32KB (configurable in firmware) | 32KB |
| **Banked RAM** | 512KB (RAM Card) | 32KB (default), up to 512KB with PSRAM | N/A |
| **Total RAM** | 544KB | 64KB (default), up to 544KB with PSRAM | 32KB |
| **ROM** | 32KB (Memory Card) | N/A (loads from SD/network) | 16KB (ROM Cart, swappable) |
| **Video Output** | Composite (TMS9918A) or VGA (Pico9918) | 2.4" LCD (VT-AC) | VGA (Pico9918) |
| **Video Resolution** | 256×192 (TMS9918A) or 640×480 VGA | 640×480 VGA or 320×240 LCD | 640×480 VGA |
| **Audio** | ARMSID (SID emulation) | N/A | ARMSID |
| **Storage** | CompactFlash or SD card* | SD card (onboard Teensy 4.1) | ROM cartridges |
| **Storage Capacity** | Up to 128GB (CF) or 32GB (SD) | Up to 32GB (SD) | 16KB per cartridge** |
| **Serial I/O** | 6551 ACIA (RS-232) | USB serial (115200 baud) | N/A |
| **Network** | N/A | Ethernet (10/100 Mbps) | N/A |
| **GPIO Ports** | 6522 VIA (Keyboard / Joystick) | USB (Keyboard / Joystick) | Input Board (Keyboard / Joystick) |
| **Input Methods** | PS/2 keyboard or matrix keyboard, joysticks | USB keyboard, USB joystick | PS/2 keyboard or matrix keyboard, joysticks |
| **Real-time Clock** | DS1511Y (Y2K compliant) | TimeLib software RTC | N/A |
| **Expansion Slots** | 9 slots (12 available with full backplane configuration) | N/A | N/A |
| **Power Requirement** | 5V DC, 2-3A | 5V DC, 2A | 5V DC, 1-2A |
| **Board Count** | 12+ boards/cards | 2 boards | 4 boards |
| **Complexity** | Advanced | Intermediate | Intermediate |

**Legend:**
- `*` = UNTESTED component or configuration
- `**` = Banked cartridge could be designed to allow for more cartridge capacity
- `N/A` = Not applicable or not available for this configuration

---

## Board Compatibility Matrix

This matrix shows which boards, cards, and helpers are compatible with each system configuration. Use this as a guide when planning your build.

### Core Compatibility

| Board/Card/Helper | COB | DEV | VCS | Notes |
|-------------------|:---:|:---:|:---:|-------|
| **BOARDS** |
| Main Board | ✓ | — | ✓ | Required for COB/VCS (not DEV) |
| Dev Board | — | ✓ | — | DEV system only |
| Dev Output Board | — | ✓ | — | Pairs with Dev Board for TMS9918A VDP and SID audio output |
| Input Board | ○ | — | ✓ | VCS primary; can be used with COB |
| Output Board | ○ | — | ✓ | VCS primary; can be used with COB |
| LCD Board | ○ | — | — | **UNTESTED**; COB optional, VCS optional |
| Backplane | ✓ | — | ○ | COB primary; can be used with VCS |
| Backplane Pro | ✓ | — | — | COB primary (enhanced backplane) |
| **CARDS** |
| Backplane Helper | ✓ | ○ | ○ | Adds card slots |
| CPU Card | ✓ | — | — | COB primary (hosts CPU) |
| CPU Card Pro | ○ | — | — | COB primary; 65816 CPU; **UNTESTED** |
| Memory Card | ✓ | — | — | COB primary (32K RAM + 32K ROM) |
| RAM Card | ✓ | — | ○ | COB optional (512KB banked RAM) |
| RTC Card | ✓ | — | ○ | COB optional (real-time clock) |
| GPIO Card | ✓ | — | — | COB required |
| Serial Card | ✓ | — | ○ | COB communication |
| Serial Card Pro | ✓ | — | ○ | Enhanced serial; COB |
| Video Card | ✓ | — | — | COB option (TMS9918A composite) |
| Video Card Pro | ○ | — | — | COB option; **UNTESTED** |
| VGA Card | ✓ | — | — | COB option (Pico9918 VGA) |
| VGA Card Pro | ○ | — | — | COB option; **UNTESTED** |
| Sound Card | ✓ | — | — | COB (ARMSID) |
| Storage Card | ✓ | — | ○ | COB (CompactFlash) |
| Storage Card Pro | ○ | — | ○ | SD card; **UNTESTED** |
| LCD Card | ○ | — | — | 16×2 LCD Display |
| Blinkenlights Card | ○ | — | ○ | Debug/visual output; any backplane system |
| Prototype Card | ○ | — | ○ | Custom circuits; any backplane system |
| **HELPERS** |
| Keyboard Encoder Helper | ✓ | — | ○ | COB GPIO keyboard input (ATmega1284p) |
| PS2 Helper | ✓ | — | ○ | COB GPIO PS/2 keyboard (ATmega328p) |
| Keyboard Helper | ✓ | — | ○ | COB 64-key matrix + joysticks |
| Keypad Helper | ○ | — | ○ | Keypad input (4×4 + 2×4) |
| Keypad LCD Helper | ○ | — | ○ | Keypad LCD display |
| GPIO Helper | ✓ | — | ○ | 8 buttons + 8 LEDs; testing/debug |
| GPIO Breadboard Helper | ✓ | — | — | Breadboard interface for GPIO |
| Joystick Helper | ✓ | — | ○ | Atari 2600-style joystick |
| Clock Helper | ✓ | — | ✓ | Manual clock control; any real CPU system |
| DB25 Helper | ✓ | — | — | GPIO to DB25 connector |
| Breadboard Helper | ✓ | ✓ | ✓ | Bus to breadboard; prototyping |
| Mega Helper | ✓ | — | — | Arduino Mega 2560 interface |
| VERA Helper | ○ | — | — | VERA module adapter |
| **CARTS** |
| ROM Cart | ✓ | — | ✓ | DEV/VCS game cartridges (16KB ROM) |

**Legend:**
- `✓` = Required or primary use case
- `○` = Optional or compatible but not typical
- `—` = Not applicable or incompatible
- **UNTESTED** = Hardware exists but not yet verified

### Card Slot Requirements

**COB System (9 slots typically used, 12 available):**
- Uses Backplane, Backplane Pro, and Backplane Helper together
- 12 total card slots available in full configuration
- Typical configuration uses 9 slots for: CPU, Memory, Video, Sound, Storage, Serial, RAM, RTC, GPIO cards
- Additional 3 slots available for expansion or future cards

**DEV and VCS Systems:**
- No backplane architecture (fixed board configurations)
- DEV or VCS systems *CAN* however be expanded using Backplane boards and cards

---

## Architecture Overview

All 6502 project systems share a common architecture based on the classic 6502 microprocessor bus design, with modern enhancements for flexibility and expansion.

### Memory Map

The project uses a standardized memory map across all systems (see [Memory Map.png](./Assets/Documents/Memory%20Map/Memory%20Map.png) for complete details):

```
$0000 ├─────────────────────────────────────┤
      │  Zero Page                          │  256 bytes ($00-$FF)
$0100 ├─────────────────────────────────────┤
      │  Stack                              │  256 bytes ($0100-$01FF)
$0200 ├─────────────────────────────────────┤
      │  Input Buffer                       │  256 bytes ($0200-$02FF)
$0300 ├─────────────────────────────────────┤
      │  KERNAL Variables                   │  256 bytes ($0300-$03FF)
$0400 ├─────────────────────────────────────┤
      │  User Variables                     │  1KB ($0400-$07FF)
$0800 ├─────────────────────────────────────┤
      │  Program RAM                        │  ~30KB ($0800-$7FFF)
      │  General purpose user RAM           │
$8000 ├─────────────────────────────────────┤
      │  8K I/O Space                       │  8 slots × 1KB each
      │  $8000-$83FF: IO 1 (RAM Card)       │
      │  $8400-$87FF: IO 2 (RAM Card)       │
      │  $8800-$8BFF: IO 3 (RTC Card)       │
      │  $8C00-$8FFF: IO 4 (Storage Card)   │
      │  $9000-$93FF: IO 5 (Serial Card)    │
      │  $9400-$97FF: IO 6 (GPIO Card)      │
      │  $9800-$9BFF: IO 7 (Sound Card)     │
      │  $9C00-$9FFF: IO 8 (Video Card)     │
$A000 ├─────────────────────────────────────┤
      │  LO System ROM (KERNAL)             │  8KB ($A000-$BFFF)
      │  Built-in, not replaceable          │
$C000 ├─────────────────────────────────────┤
      │  HI System ROM or Cart ROM          │  ~16KB ($C000-$FFFA)
      │  BASIC/Monitor, or removable cart   │
$FFFA ├─────────────────────────────────────┤
      │  Interrupt Vectors                  │  6 bytes
      │  $FFFA-$FFFB: NMI                   │
      │  $FFFC-$FFFD: RESET                 │
      │  $FFFE-$FFFF: IRQ/BRK               │
      ├─────────────────────────────────────┤
```

**I/O Slots** (8 slots of 1KB each, $8000-$9FFF):
- **IO 1 ($8000-$83FF)**: RAM Card (1KB window into first 256KB bank)
- **IO 2 ($8400-$87FF)**: RAM Card (1KB window into second 256KB bank)
- **IO 3 ($8800-$8BFF)**: RTC Card (DS1511Y)
- **IO 4 ($8C00-$8FFF)**: Storage Card (CompactFlash/SD controller)
- **IO 5 ($9000-$93FF)**: Serial Card (6551 ACIA)
- **IO 6 ($9400-$97FF)**: GPIO Card (6522 VIA)
- **IO 7 ($9800-$9BFF)**: Sound Card (ARMSID interface)
- **IO 8 ($9C00-$9FFF)**: Video Card (TMS9918A or Pico9918)

**Banked RAM** (RAM Card, if installed):
- 512KB total RAM organized as two 256KB banks
- Bank selection via single latch at $83FF or $87FF (write 0-255)
- Writing to latch selects which 1KB window (of 256) appears at IO 1 and IO 2
- IO 1 ($8000-$83FF): 1KB window into first 256KB bank
- IO 2 ($8400-$87FF): 1KB window into second 256KB bank
- Both windows show the same bank number from their respective 256KB regions
- Allows access to full 512KB by switching between 256 different 1KB windows

### Bus Structure

The 6502 bus includes:
- **16-bit Address Bus** (A0-A15): Addresses up to 64KB memory space
- **8-bit Data Bus** (D0-D7): Bidirectional data transfer
- **Control Signals**:
  - `RW` (Read/Write): Direction of data transfer (1=Read, 0=Write)
  - `PHI2` (Clock): System clock signal (0.5-8 MHz typical)
  - `RESET`: System reset (active low)
  - `IRQ`: Interrupt request (active low)
  - `NMI`: Non-maskable interrupt (active low)
  - `RDY`: Ready signal for clock stretching
  - `SYNC`: Instruction fetch indicator

### Clock & Reset Circuitry

**Clock Generation:**
- Oscillator: Crystal or ceramic resonator (1-8 MHz typical)
- Clock can be:
  - Free-running (normal operation)
  - Single-stepped (via Clock Helper)
  - Variable frequency (firmware in DEV)
- Clock Helper provides:
  - Run/Stop control
  - Single cycle stepping
  - Single instruction stepping

**Reset Circuit:**
- Power-on reset via RC circuit (Main Board or Backplane Pro)
- Manual reset button
- Watchdog timer reset (optional, via RTC Card)

### Interrupt Handling

- **IRQ (Maskable Interrupt)**: Used by serial (6551), timers (6522), and other peripherals
- **NMI (Non-Maskable Interrupt)**: Currently unused but can be configured on some cards
- Interrupt vectors stored at `$FFFA-$FFFF` in ROM

### Address Decoding

Each peripheral card uses address decoding logic to respond only to its assigned memory range:
- **I/O Cards**: Use 74HC138 3-to-8 decoder with address lines A15-A10, jumper-selectable for flexibility
- **Main Board & Memory Card**: Use 74HC00 NAND for RAM/ROM chip select
- Jumpers or DIP switches allow I/O address reconfiguration on cards

**Example:** Serial Card (6551 ACIA)
- Occupies 4 bytes in I/O space ($9000-$9003, IO 5)
- 74HC138 decoder uses A15-A10 to select I/O slot
- Address decoder activates chip enable when A15-A10 match configured slot
- Lower address bits (A1-A0) select specific ACIA registers

---

## Hardware

The [Hardware](./Hardware) folder contains KiCad files for the system's custom-designed PCBs. These are divided into the following categories:

- **Boards**: The main board, development board, backplane boards and other peripheral boards
- **Cards**: Various peripheral cards for plugging into the backplane or card slot(s)
- **Helpers**: Additional boards that provide specific functionalities or interfaces
- **Carts**: Cartridge boards for ROM images

See the [Hardware README](./Hardware/README.md) for detailed technical specifications, pin assignments, and assembly notes.

### Boards

These are the primary system boards that form the foundation of each computer configuration.

#### Backplane

#### Backplane
**Purpose:** Provides 5 card slots for peripheral cards and connects to the Main Board via ribbon cable  
**Key Components:** Card edge connectors (×5), ribbon cable headers   
**Status:** ✓ Tested  

A passive backplane that provides mechanical and electrical connectivity for up to 5 peripheral cards, directly interconnecting the bus signals across all slots.

#### Backplane Pro
**Purpose:** Enhanced backplane with 5 card slots plus integrated clock, reset, and power circuitry  
**Key Components:** Oscillator, reset IC, voltage regulators, card edge connectors (×5)  
**Clock Output:** 1-8 MHz (selectable by swapping oscillator)  
**Power:** 5V input, distributes regulated 5V to cards as needed  
**Status:** ✓ Tested  

An active backplane that not only interconnects cards but also provides centralized clock generation and power management, reducing component count on peripheral cards.

#### Dev Board
**Purpose:** Teensy 4.1-based 65C02 CPU emulator with networking and USB I/O  
**Key Components:** Teensy 4.1 (600 MHz ARM Cortex-M7), Ethernet PHY, SD card slot, USB host  
**Emulation:** Cycle-accurate 65C02 via [vrEmu6502](https://github.com/visrealm/vrEmu6502)  
**Memory:** Up to 32KB base RAM + 32KB banked RAM (default), expandable to 512KB banked RAM with PSRAM  
**Clock Speed:** Variable from very slow to maximum (exceeding real 65C02)  
**Networking:** Ethernet 10/100 Mbps with mDNS (hostname: `6502.local`)  
**Storage:** MicroSD card (up to 32GB)  
**I/O:** USB keyboard, USB joystick (Xbox 360/One), serial (115200 baud)  
**Firmware:** [DB Emulator](./Firmware/DB%20Emulator/) (PlatformIO project)  
**Status:** ✓ Tested  

The Dev Board eliminates the need for a physical 65C02 CPU by providing accurate emulation with enhanced debugging capabilities, network control, and instant program loading.

#### Dev Output Board
**Purpose:** 2.4" LCD display module providing TMS9918A VDP and SID audio emulation for the Dev Board  
**Key Components:** ILI9341 TFT display (320×240), level shifters  
**Display:** 2.4" TFT LCD, 320×240 resolution, 262K colors (256×192 active area with TMS9918A color borders)  
**Protocol:** AV packet stream (4-byte packets at 6 Mbps via hardware UART) from DB Emulator  
**Connection:** SPI interface to Dev Board  
**Firmware:** [DOB Display](./Firmware/DOB%20%20Display/) (PlatformIO project)  
**Audio:** 3-voice SID 6581 synthesis via 8-bit PWM at 44.1 kHz  
**Status:** ✓ Tested 

Provides dedicated video and audio output for the DEV system, emulating the TMS9918A VDP and MOS 6581 SID in real time at ~60 FPS.

#### Input Board
**Purpose:** Unified input board for matrix keyboard, PS/2 keyboard, and joysticks
**Key Components:** 6522 VIA (Versatile Interface Adapter), ATmega1284p microcontroller
**Inputs:** Matrix keyboard, PS/2 keyboard, and joysticks  
**Interface:** Parallel I/O to 6502 bus (memory-mapped registers)  
**Firmware:** [KEH Controller](./Firmware/KEH%20Controller/)   
**Status:** ✓ Tested  

Consolidates multiple input devices onto a single board, providing input for gaming and general use.

#### LCD Board
**Purpose:** 320×240 TFT LCD display interfaced via 6522 VIA  
**Key Components:** ILI9341 TFT controller, 6522 VIA for I/O  
**Display:** 320×240 resolution  
**Interface:** 6522 VIA parallel interface (slower than SPI but directly CPU-addressable)  
**Status:** ⚠️ **UNTESTED**  

An experimental board attempting to drive an LCD display directly from 6502 via VIA. Performance may be limited due to parallel interface overhead.

#### Main Board
**Purpose:** Core board containing 65(c)02 CPU, RAM, ROM, clock, and reset circuitry  
**Key Components:** 65C02 CPU, 32KB SRAM (62256), 32KB EEPROM (28C256), oscillator, reset IC  
**CPU:** W65C02S or equivalent  
**Clock Speed:** 1-8 MHz (selectable by swapping oscillator)   
**RAM:** 32KB (addresses $0000-$7FFF)  
**ROM:** 32KB (addresses $8000-$FFFF)  
**Bus Connection:** Ribbon cable header for expansion  
**Power:** 5V DC input  
**Status:** ✓ Tested  

The Main Board is a self-contained 6502 computer suitable for standalone use or as the foundation for expanded systems.

#### Output Board
**Purpose:** Combined VGA video output (via Pico9918) and audio output (via ARMSID)  
**Key Components:** Raspberry Pi Pico (Pico9918 firmware), ARMSID module  
**Video:** VGA 640×480 via [Pico9918](https://github.com/visrealm/pico9918)  
**Audio:** ARMSID (SID chip emulation)  
**Interface:** Memory-mapped I/O to 6502 bus  
**Status:** ✓ Tested  
**Used In:** VCS (primary), DEV (alternative), COB (alternative)

Combines video and audio output on a single board for compact gaming console configurations.

---

### Cards

Peripheral cards plug into backplane slots or directly onto the Main Board via single card slot.

#### Blinkenlights Card
**Purpose:** Visual status LEDs for address, data, and control bus monitoring  
**Key Components:** LEDs (×32+), current-limiting resistors  
**LEDs:** Show address bus (16), data bus (8), control signals (8)  
**Status:** ✓ Tested 

A classic "blinkenlights" panel that visualizes bus activity in real-time, invaluable for debugging and demonstrations.

#### CPU Card
**Purpose:** Hosts a 65C02 CPU in a card form factor for backplane systems  
**Key Components:** W65C02S CPU socket, clock input, bus buffers  
**CPU:** W65C02S (DIP-40 socket)  
**Clock:** Provided by Backplane Pro or external source  
**Status:** ✓ Tested  

Allows the CPU to be installed on a card rather than the main board, enabling CPU upgrades or replacements.

#### CPU Card Pro
**Purpose:** Hosts a 65816 16-bit CPU (backward-compatible with 65C02)  
**Key Components:** W65C816S CPU socket, additional address lines (A16-A23)  
**CPU:** W65C816S (DIP-40 socket)  
**Address Space:** 16MB (24-bit addressing)  
**Status:** ⚠️ **UNTESTED**  

Provides 16-bit CPU capability and extended address space. Requires compatible memory and I/O cards to utilize extended features.

#### GPIO Card
**Purpose:** General-purpose I/O using 6522 VIA chip  
**Key Components:** 6522 VIA (Versatile Interface Adapter)  
**I/O Pins:** 20 GPIO pins (2× 8-bit ports + 4 control lines)  
**Address Range:** IO 6 ($9400-$97FF), configurable via jumpers  
**Timers:** 2× 16-bit timers with interrupt capability  
**Shift Register:** Serial I/O support  
**Status:** ✓ Tested  

The GPIO Card is highly versatile, supporting keyboards, displays, custom I/O devices, and timing-critical applications.

#### LCD Card
**Purpose:** 16×2 character LCD display via 6522 VIA  
**Key Components:** 6522 VIA, 16×2 LCD module (HD44780-compatible)  
**Display:** 16 characters × 2 lines  
**Interface:** 6522 VIA parallel interface (4-bit or 8-bit mode)  
**Address Range:** IO 6 ($9400-$97FF), shared with GPIO Card  
**Status:** ✓ Tested  

Provides a simple text display for system messages, debugging output, or simple user interfaces.

#### Memory Card
**Purpose:** 32KB RAM + 32KB ROM with address decoding logic  
**Key Components:** 32KB SRAM (62256), 32KB EEPROM (28C256), address decoder logic  
**RAM:** 32KB at $0000-$7FFF  
**ROM:** 32KB at $8000-$FFFF  
**Address Decoding:** 74-series logic (74HC00)  
**Status:** ✓ Tested  

Essential card providing memory for COB systems where the CPU Card is used instead of the Main Board.

#### Prototype Card
**Purpose:** Blank prototyping area for custom circuits  
**Key Components:** Onboard small breadboard, bus access headers  
**Prototyping Area:** Integrated breadboard  
**Bus Access:** Full address, data, and control bus available  
**Status:** ✓ Tested (verified bus connectivity) 

Allows experimentation with custom circuits while maintaining access to the system bus.

#### RAM Card
**Purpose:** 512KB banked RAM expansion  
**Key Components:** SRAM chips (512KB total), banking logic, bank select latch  
**Capacity:** 512KB organized as two 256KB banks, each with 256× 1KB windows  
**Banking:** Single latch at $83FF or $87FF selects 1KB window (0-255) for both IO slots  
**Bank Windows:** IO 1 ($8000-$83FF) and IO 2 ($8400-$87FF)  
**Status:** ✓ Tested 

Dramatically expands memory capacity beyond the 65C02's 64KB address space. The single latch controls which 1KB window appears at both IO 1 (first 256KB) and IO 2 (second 256KB), allowing efficient access to the full 512KB.

#### RTC Card
**Purpose:** Real-time clock with battery backup  
**Key Components:** Dallas DS1511Y (RTC with integrated SRAM and battery)  
**Clock:** Year/month/day/hour/minute/second with Y2K support  
**Battery Backup:** Lithium coin cell (maintains time during power-off)  
**SRAM:** 256 bytes battery-backed SRAM  
**Alarm:** Programmable alarm interrupt  
**Address Range:** IO 3 ($8800-$8BFF)  
**Status:** ✓ Tested 

Provides persistent timekeeping for timestamps, scheduling, and time-based applications.

#### Serial Card
**Purpose:** RS-232 serial communication via 6551 ACIA  
**Key Components:** 6551 ACIA (Asynchronous Communications Interface Adapter), MAX232 level shifter  
**Interface:** RS-232 via DB9 connector  
**Baud Rate:** 50 - 19200 bps (configurable via software)  
**Address Range:** IO 5 ($9000-$93FF)  
**Status:** ✓ Tested  

Essential for terminal communication, program loading, and interfacing with modern PCs.

#### Serial Card Pro
**Purpose:** Enhanced serial card with full modem control signals  
**Key Components:** 6551 ACIA, MAX232 (×2) for RS-232 level conversion  
**Features:** Same as Serial Card plus DTR, DCD, and DSR modem control signals via second MAX232  
**Status:** ✓ Tested  

An improved version of the Serial Card with full modem control signal support for advanced serial communications.

#### Sound Card
**Purpose:** Audio output via ARMSID SID chip emulator  
**Key Components:** ARMSID module (ARM-based SID emulation)  
**Audio:** 3-voice synthesizer (SID chip emulation)  
**Output:** RCA jack (line level)  
**Interface:** Memory-mapped I/O (SID-compatible registers)  
**Address Range:** IO 7 ($9800-$9BFF)  
**Status:** ✓ Tested  

Provides classic SID chip audio capabilities for games and music applications.

#### Storage Card
**Purpose:** CompactFlash storage interface  
**Key Components:** CompactFlash socket, IDE interface logic  
**Capacity:** Up to 128GB (CF card dependent)  
**Interface:** IDE protocol (CF cards are IDE-compatible)  
**Address Range:** IO 4 ($8C00-$8FFF)  
**File System:** Raw sectors or FAT16/FAT32 with appropriate drivers  
**Status:** ✓ Tested 

Provides persistent storage for programs, data, and disk images. CompactFlash cards are rugged and widely available.

#### Storage Card Pro
**Purpose:** Modern storage via SD card, 16MB Flash, and SPI interface  
**Key Components:** SD card socket, SPI flash (16MB), SPI controller  
**SD Card:** Up to 32GB (SDHC)  
**Flash:** 16MB onboard SPI flash  
**Interface:** SPI protocol  
**Address Range:** IO 4 ($8C00-$8FFF) 
**Status:** ⚠️ **UNTESTED**  

Provides modern SD card storage plus onboard flash. More complex software required but offers higher capacities.

#### VGA Card
**Purpose:** VGA video output via Pico9918  
**Key Components:** Raspberry Pi Pico running [Pico9918](https://github.com/visrealm/pico9918) firmware  
**Video:** VGA 640×480 @ 60Hz  
**Graphics:**  
 TMS9918A-compatible modes (Pico9918 emulates classic video chip)  
**Colors:** 16-color palette (TMS9918A modes)  
**Interface:** Memory-mapped I/O (TMS9918A-compatible registers)  
**Address Range:** IO 8 ($9C00-$9FFF)  
**Status:** ✓ Tested  

Provides VGA output with classic TMS9918A graphics modes, ideal for retro gaming and graphics applications.

#### VGA Card Pro
**Purpose:** Custom programmable VGA output using Pi Pico with custom firmware  
**Key Components:** Raspberry Pi Pico, resistor DAC, VGA connector  
**Video:** VGA 640×480 (or custom resolutions)  
**Graphics:** Fully programmable (custom framebuffer modes possible)  
**Interface:** Memory-mapped I/O  
**Status:** ⚠️ **UNTESTED**  

An experimental card offering fully customizable VGA output modes. Requires custom firmware development.

#### Video Card
**Purpose:** Composite video output via TMS9918A chip  
**Key Components:** TMS9918A Video Display Processor (VDP), video RAM (16KB)  
**Video:** Composite video output (NTSC or PAL)  
**Resolution:** 256×192 pixels  
**Colors:** 16-color palette  
**Sprites:** 32 sprites (8×8 or 8×16 pixels)  
**Interface:** Memory-mapped I/O (2-byte register interface)  
**Address Range:** IO 8 ($9C00-$9FFF)  
**Status:** ✓ Tested  

Classic retro graphics using the authentic TMS9918A chip from 1970s/80s computers and consoles.

#### Video Card Pro
**Purpose:** Enhanced composite video via ATmega1284p microcontroller  
**Key Components:** ATmega1284p, video DAC, composite video output  
**Video:** Color composite video  
**Features:** Microcontroller-generated video (programmable graphics modes)  
**Status:** ⚠️ **UNTESTED**  

Attempts to generate composite video using a microcontroller rather than dedicated video chip. Flexibility vs. complexity tradeoff.

---

### Helpers

Helper boards provide specialized interfaces, connectors, or functionality that complements the main boards and cards.

#### Backplane Helper
**Purpose:** Adds 2 additional card slots to existing backplane  
**Key Components:** Card edge connectors (×2), ribbon cable headers  
**Slots:** +2 expansion slots  
**Status:** ✓ Tested  

Increases expansion capability for systems requiring more peripherals.

#### Breadboard Helper
**Purpose:** Interfaces 6502 bus to standard breadboard  
**Key Components:** Ribbon cable header, breadboard-compatible headers  
**Pinout:** Full address, data, and control bus broken out  
**Status:** ✓ Tested  

Essential for prototyping custom circuits and testing ideas before committing to PCB designs.

#### Clock Helper
**Purpose:** Manual clock control for debugging and single-stepping  
**Key Components:** Clock manipulation buttons  
**Modes:**  
  - Free-running (normal operation)  
  - Single cycle step (advance one clock cycle per button press)  
  - Single instruction step (advance one full instruction)  
**Status:** ✓ Tested  

Invaluable debugging tool allowing cycle-by-cycle or instruction-by-instruction execution.

#### DB25 Helper
**Purpose:** Interfaces GPIO Card to DB25 connector  
**Key Components:** DB25 connector, header pins  
**Pinout:** Maps VIA ports to standard DB25 pinout  
**Status:** ✓ Tested  

Provides DB25 connectivity for GPIO applications.

#### GPIO Helper
**Purpose:** 8 buttons + 8 LEDs for user I/O and testing  
**Key Components:** Tactile switches (×8), LEDs (×8), current-limiting resistors  
**Inputs:** 8 debounced buttons  
**Outputs:** 8 indicator LEDs  
**Interface:** Connects to GPIO Card (6522 VIA)  
**Status:** ✓ Tested  

Simple I/O for testing, debugging, or basic user interaction.

#### GPIO Breadboard Helper
**Purpose:** Interfaces GPIO Card to breadboard  
**Key Components:** Header pins  
**Pinout:** All GPIO pins broken out to breadboard-compatible headers  
**Status:** ✓ Tested  

Allows quick breadboard prototyping of GPIO-based circuits.

#### Joystick Helper
**Purpose:** Atari 2600-style joystick interface  
**Key Components:** DB9 connector, pull-up resistors  
**Compatibility:** Atari 2600, Commodore 64, Amiga joysticks  
**Inputs:** Up, down, left, right, fire button (plus extra buttons) 
**Interface:** Connects to GPIO Card  
**Status:** ✓ Tested  

Classic digital joystick interface for retro gaming.

#### Keyboard Helper
**Purpose:** 64-key matrix keyboard + dual joystick ports  
**Key Components:** Key switches (×64), diodes, DB9 connectors (×2)  
**Keyboard:** 8×8 matrix (64 keys total)  
**Joysticks:** 2× Atari-style joystick ports  
**Interface:** Connects to GPIO Card (6522 VIA)  
**Scanning:** Matrix scanning via VIA ports  
**Status:** ✓ Tested  

Full keyboard input plus joystick support for gaming and general computing.

#### Keyboard Encoder Helper
**Purpose:** Translates keyboard matrix and PS/2 keyboard to ASCII  
**Key Components:** ATmega1284p microcontroller, PS/2 connector  
**Inputs:** PS/2 keyboard OR 8×8 keyboard matrix (Keyboard Helper)  
**Output:** ASCII characters via 6522 VIA interface  
**Firmware:** [KEH Controller](./Firmware/KEH%20Controller/)  
**Features:**  
  - Dual input support (PS/2 + matrix simultaneously)  
  - ASCII conversion (0x00-0xFF including extended characters)  
  - Full modifier key support (Shift, Ctrl, Alt, Fn)  
  - Function keys F1-F12  
  - Circular buffer (prevents data loss)  
**Status:** ✓ Tested  

Provides intelligent keyboard handling, converting raw scancodes to ASCII for easier software development.

#### Keypad Helper
**Purpose:** 4×6 keypad
**Key Components:** Matrix keypad (4×6)  
**Keys:** 0-9, A-F (hex), plus 8 function keys  
**Interface:** Connects to GPIO Card (matrix scanning)  
**Status:** ✓ Tested  

Essential for KIM-1-style hexadecimal data entry and system control.

#### Keypad LCD Helper
**Purpose:** LCD display designed to pair with Keypad Helper  
**Key Components:** LCD module (HD44780-compatible)  
**Display:** 16×2 characters (typical) or 20×4  
**Interface:** Connects to GPIO Card via parallel interface  
**Status:** ✓ Tested  

Displays output for KIM-1-style system (addresses, data, messages).

#### Mega Helper
**Purpose:** Interfaces Arduino Mega 2560 R3 to 6502 bus  
**Key Components:** Headers for Arduino Mega  
**Microcontroller:** Arduino Mega 2560 R3 (user-supplied)  
**Use Cases:**  
  - 6502 bus monitoring
  - Custom peripherals programmed in Arduino IDE  
  - I/O expansion  
  - Sensor interfaces  
  - LCD controllers  
**Interface:** Connects to 6502 bus  
**Status:** ✓ Tested  

Leverages the Arduino ecosystem for easy peripheral development.

#### PS2 Helper
**Purpose:** PS/2 keyboard interface with ASCII conversion  
**Key Components:** ATmega328p microcontroller, PS/2 connector  
**Input:** PS/2 keyboard  
**Output:** ASCII characters via 6522 VIA or matrix emulation  
**Firmware:** [PS2 Keyboard Controller](./Firmware/PS2%20Keyboard%20Controller/)  
**Features:**  
  - PS/2 protocol decoding  
  - ASCII conversion  
  - Extended scancode support  
  - Function key emulation (F1-F10 via Fn + numbers)  
**Status:** ✓ Tested  

Simpler alternative to Keyboard Encoder Helper when only PS/2 input is needed.

#### VERA Helper
**Purpose:** Adapts the VERA module to 6502 bus  
**Key Components:** VERA module (from Commander X16 project), level shifters, bus interface  
**Video:** VERA capabilities (see [Commander X16 VERA](https://github.com/commanderx16/x16-docs/blob/master/VERA%20Programmer's%20Reference.md))  
**Interface:** Memory-mapped I/O (VERA register interface)  
**Status:** ✓ Tested

Adapter for the powerful VERA module from the Commander X16 project.

---

### Carts

#### ROM Cart
**Purpose:** Swappable ROM cartridge for VCS gaming system  
**Key Components:** EEPROM or Flash ROM (up to 16KB), edge connector  
**Capacity:** 16KB (addresses $C000-$FFFF when inserted)  
**Programming:** Requires TL866 or similar EEPROM programmer (for 28C256/27C256 chips), or in-system programming (if flash-based)  
**Status:** ✓ Tested  

Allows game cartridge-based operation similar to classic game consoles.

---

## Firmware

The [Firmware](./Firmware) folder contains source code for firmware running on various microcontrollers used in the project (Teensy 4.1, ATmega328p, ATmega1284p, ATTiny85). Firmware is written in C/C++ and is developed using **PlatformIO**.

See the [Firmware README](./Firmware/README.md) for detailed information on building and flashing firmware.

### Firmware Projects by System

| Firmware Project | Target Hardware | Microcontroller | System | Description |
|-----------------|-----------------|-----------------|--------|-------------|
| [DB Emulator](./Firmware/DB%20Emulator/) | Dev Board | Teensy 4.1 | DEV | 65C02 emulator with networking, USB I/O, and web control |
| [DOB Display](./Firmware/DOB%20%20Display/) | Dev Output Board | Teensy 4.0 | DEV | TMS9918A VDP + SID audio emulation on ILI9341 LCD |
| [IB Keyboard Controller](./Firmware/IB%20Keyboard%20Controller/) | Input Board Rev 0.0 | ATTiny85 | VCS | PS/2 keyboard to serial interface |
| [IB Mouse Controller](./Firmware/IB%20Mouse%20Controller/) | Input Board Rev 0.0 | ATTiny85 | VCS | PS/2 mouse interface |
| [KEH Controller](./Firmware/KEH%20Controller/) | Keyboard Encoder Helper, Input Board Rev 1.0 | ATmega1284p | COB/VCS | Dual keyboard (PS/2 + matrix) to ASCII converter |
| [PS2 Keyboard Controller](./Firmware/PS2%20Keyboard%20Controller/) | PS2 Helper | ATmega328p | COB | PS/2 keyboard to matrix interface |
| [STP Controller](./Firmware/STP%20Controller/) | Storage Card Pro | ATmega328p | COB/VCS | SD card and SPI flash storage controller |

### Key Firmware Features

**DB Emulator (Dev Board):**
- Cycle-accurate 65C02 emulation via [vrEmu6502](https://github.com/visrealm/vrEmu6502)
- Ethernet connectivity with mDNS (`6502.local`)
- Embedded web server with REST API and web application
- SD card support for ROM/program loading
- USB keyboard and joystick input (Xbox controllers)
- Variable CPU speed control
- Memory snapshot save
- Serial terminal interface (115200 baud)
- Real-time clock support

**DOB Display (Dev Output Board):**
- TMS9918A VDP emulation (Graphics I/II, Text, and Multicolor modes)
- SID 6581 3-voice audio synthesis (triangle, sawtooth, pulse, noise + full ADSR)
- 320×240 color LCD display (2.4" ILI9341) with 256×192 active area and TMS9918A color borders
- ~60 FPS rendering from VRAM state
- AV packet protocol at 6 Mbps via hardware UART from DB Emulator

**KEH Controller (Keyboard Encoder Helper):**
- Dual input support (PS/2 + keyboard matrix simultaneously)
- ASCII conversion with extended character set (0x00-0xFF)
- Full modifier key support (Shift, Ctrl, Alt, Fn)
- Function keys F1-F12
- Circular buffering to prevent key loss

**PS2 Keyboard Controller (PS2 Helper):**
- PS/2 protocol decoding with parity checking
- MT8808 crosspoint switch control for matrix emulation
- Extended scancode support
- Function key emulation (Fn + numbers)

### Development Environment

**Prerequisites:**
- [PlatformIO Core](https://platformio.org/install/cli) or [PlatformIO IDE](https://platformio.org/platformio-ide) (VS Code extension)
- Python 3.7+ (bundled with PlatformIO)
- USB cable for programming

**Building Firmware:**
```bash
cd Firmware/<project_name>
pio run                 # Build
pio run --target upload # Build and upload
```

**Using VS Code:**
1. Open folder in VS Code (with PlatformIO extension installed)
2. PlatformIO: Build (Ctrl/Cmd+Alt+B)
3. PlatformIO: Upload (Ctrl/Cmd+Alt+U)

**Serial Monitoring:**
```bash
pio device monitor    # PlatformIO serial monitor
```

See individual firmware project README files for specific build instructions and configuration options.

---

## Software & Related Projects

The 6502 project has several companion repositories containing software that runs on the 6502 systems, as well as development tools.

### Emulator

**[6502-EMULATOR](https://github.com/acwright/6502-EMULATOR)**
- Software emulator for the 6502 computer systems
- Allows running and testing 6502 programs without physical hardware
- Useful for software development and debugging

### Assembly Code & BIOS

The following repositories contain 6502 assembly code that runs on the hardware:

**[6502-ASM](https://github.com/acwright/6502-ASM)**
- General 6502 assembly code examples and utilities
- Sample programs demonstrating hardware features
- Library routines for common tasks

**[6502-BIOS](https://github.com/acwright/6502-COBBIOS)**
- BIOS (Basic Input/Output System) for all systems
- Low-level hardware initialization and I/O routines
- System ROM code providing BASIC and machine code monitor

**[6502-NOP](https://github.com/acwright/6502-NOP)**
- NOP (No Operation) test program
- Minimal ROM code for hardware testing
- Useful for verifying basic CPU and memory operation

**[6502-WOZMON](https://github.com/acwright/6502-WOZMON)**
- Woz Monitor (Apple I monitor program)
- Classic machine language monitor by Steve Wozniak
- Adapted for the 6502 project hardware using the Serial Card

### Using the Software

To use these programs on your 6502 hardware:
1. Clone the desired repository
2. Assemble the code using a 6502 assembler (ca65, vasm, or similar)
3. Program the resulting binary to EEPROM using TL866 or similar programmer
4. Install the EEPROM in your 6502 system
5. Power on and test

See individual repository README files for specific build and usage instructions.

---

## CAD

The [CAD](./CAD) folder contains mechanical designs for the project including 3D models and enclosure designs.

**Contents:**
- **Bases/**: 3D models of complete system assemblies (COB, DEV, VCS)
- **Card/**: 3D models of individual card designs
- **Enclosures/**: Custom enclosure designs for various systems
- **Tops/**: Top panel/cover designs

**File Formats:**
- STL files (for 3D printing)
- STEP files (for CAD editing)

**Use Cases:**
- 3D printing custom enclosures
- Visualizing system assembly
- Creating custom mounting solutions
- Designing front panels and bezels

See the [CAD README](./CAD/README.md) for details on using and modifying CAD files.

---

## Production

The [Production](./Production) folder contains JLCPCB-ready production files for manufacturing PCBs, including Gerber files, BOMs (Bill of Materials), and assembly instructions.

### Ordering from JLCPCB

1. **Navigate to desired board folder** in [Production/](./Production)
2. **Locate Gerber files** (usually in a ZIP archive named `<BoardName>_Gerber.zip`)
3. **Go to [JLCPCB.com](https://jlcpcb.com/)** and create account (if needed)
4. **Upload Gerber ZIP file** using "Add gerber file" button
5. **Configure PCB options:**
   - Layers: 2 or 4 (check board specifications)
   - Thickness: 1.6mm (typical)
   - Color: Your choice
   - Surface Finish: HASL (economical) or ENIG (better for edge connectors)
   - Remove Order Number: Optional
6. **Select SMD Assembly** (if desired):
   - Enable PCB Assembly option
   - Upload BOM and Pick-and-Place (CPL) files from Production folder
   - Review component availability (JLCPCB may not stock all parts)
   - Confirm component placement in preview
7. **Review and order:**
   - Verify quantities (5 or 10 boards minimum typically)
   - Add to cart and checkout
   - Manufacturing time: typically 2-7 days + shipping

### Production Files Included

For each board/card/helper:
- **Gerber files**: PCB manufacturing data (layers, drill files, solder mask, silkscreen)
- **BOM (Bill of Materials)**: CSV file with component list and JLCPCB part numbers
- **CPL (Component Placement List)**: Pick-and-place file for SMD assembly
- **Design rules**: KiCad design rule (.kicad_dru) file optimized for JLCPCB

See the [Production README](./Production/README.md) for detailed ordering guides and quality control checklists.

---

## Schematics

The [Schematics](./Schematics) folder contains schematic diagrams for all PCBs in the project. Schematics are created using **KiCad 7.0+** and provide detailed component information and electrical connections.

**Contents:**
- Complete schematics for every board, card, and helper
- PDF exports (for easy viewing without KiCad)

**Schematic Conventions:**
- Power nets: VCC/+5V (positive rail), GND (ground)
- Signal naming: descriptive labels (A0-A15 for address bus, D0-D7 for data bus)
- Component references: Standard prefixes (U=IC, R=resistor, C=capacitor, D=diode, J=connector)
- Component values: Marked on schematic (resistor ohms, capacitor farads)

See the [Schematics README](./Schematics/README.md) for details on navigating and understanding schematics.

---

## Libraries

The [Libraries](./Libraries) folder contains KiCad symbol libraries, footprint libraries, and 3D models used throughout the project.

**Contents:**
- **6502 Parts.kicad_sym**: Symbol library with 6502-specific components
- **6502 Parts.pretty/=**: Footprint library (PCB footprints)
- **6502 Logos.pretty/**: Logo footprints for PCB silkscreen
- **A.C. Wright Logo.pretty/**: A.C. Wright Design logo footprints
- **Models/**: 3D STEP models for components (see [Models README](./Libraries/Models/README.md))

**Using Libraries in KiCad Projects:**
1. Open KiCad project
2. Go to Preferences → Manage Symbol Libraries / Manage Footprint Libraries
3. Add library paths pointing to this folder
4. Libraries will appear in symbol/footprint choosers

**Adding New Components:**
1. Open appropriate library in KiCad Library Editor
2. Create new symbol/footprint following existing conventions
3. Save and commit changes (if contributing back to project)

See the [Libraries README](./Libraries/README.md) for detailed library usage instructions.

---

## Development Workflow

Contributing to or extending the 6502 project follows these general workflows.

### Modifying Existing Boards

1. **Clone/Fork Repository**:
   ```bash
   git clone https://github.com/acwright/6502.git
   cd 6502
   ```

2. **Open Hardware Design** in KiCad 7.0+:
   ```
   Hardware/<BoardName>/Rev X.X/<BoardName>.kicad_pro
   ```

3. **Make Schematic Changes**:
   - Edit schematic in KiCad Schematic Editor
   - Update component values, add/remove components, modify connections
   - Run Electrical Rules Check (ERC) to verify

4. **Update PCB Layout**:
   - Update PCB from schematic (Tools → Update PCB from Schematic)
   - Adjust component placement and routing as needed
   - Run Design Rules Check (DRC) to verify JLCPCB compatibility
   - Use design rules file from Production folder if available

5. **Generate Production Files**:
   - Production files are generated using [Fabrication-Toolkit](https://github.com/bennymeg/Fabrication-Toolkit)
   - This KiCad plugin automates generation of Gerbers, drill files, BOMs, and pick-and-place files for JLCPCB
   - Install the plugin and run it from KiCad to generate all required manufacturing files
   - Output files should be moved to approprite folder in Production folder

6. **Test Design**:
   - Order prototype from JLCPCB
   - Assemble and test thoroughly
   - Document findings

7. **Commit Changes**:
   ```bash
   git add Hardware/<BoardName>/
   git commit -m "Description of changes"
   git push
   ```

### Designing New Cards/Helpers

1. **Start with Template**:
   - Use existing card design as template (copy folder structure)
   - OR use blank template from Templates/ folder

2. **Define Requirements**:
   - What functionality does the card provide?
   - What I/O address range will it occupy?
   - What connectors are needed?
   - Power consumption estimate

3. **Schematic Design**:
   - Create multi-sheet schematic if complex
   - Use 6502 Parts library for common components
   - Include address decoding logic
   - Add bus buffers if driving bus signals
   - Follow existing card conventions for connector pinouts

4. **PCB Layout**:
   - Match card edge connector footprint to backplane
   - Place connectors for easy access
   - Route power planes (4-layer board) or wide power traces (2-layer)
   - Maintain signal integrity (short traces for high-speed signals)
   - Add mounting holes (if applicable)

5. **Verify Design Rules**:
   - Run DRC with JLCPCB design rules
   - Check trace widths (minimum 0.15mm for outer layers)
   - Check clearances (minimum 0.15mm)
   - Verify drill sizes (minimum 0.3mm)

6. **Documentation**:
   - Update README with card description
   - Add to Board Compatibility Matrix
   - Create schematic PDF
   - Document I/O addresses and register map

7. **Build and Test**:
   - Order prototype
   - Test all functionality
   - Update Testing Status when verified

### Firmware Development

1. **Set Up Environment**:
   ```bash
   cd Firmware/<ProjectName>
   pio init   # If creating new project
   ```

2. **Write Code**:
   - Follow existing firmware structure
   - Use libraries when available (ArduinoJson, Bounce2, etc.)
   - Comment code thoroughly
   - Use meaningful variable/function names

3. **Build and Test**:
   ```bash
   pio run                      # Build
   pio run --target upload      # Upload to microcontroller
   pio device monitor           # Monitor serial output
   ```

4. **Debug**:
   - Add debug print statements
   - Use serial monitor to observe behavior
   - Test edge cases (invalid inputs, boundary conditions)

5. **Document**:
   - Update firmware README with features
   - Document API/registers (for microcontrollers acting as peripherals)
   - Add comments explaining complex logic

### Contributing CAD Designs

1. **Create Enclosure**:
   - Measure assembled boards accurately
   - Design in Fusion 360, OpenSCAD, FreeCAD, or similar
   - Allow clearances for connectors, cables, ventilation
   - Include mounting holes for standoffs

2. **Export Models**:
   - STL for 3D printing
   - STEP for CAD interchange

3. **Test Print**:
   - Print prototype (PLA or PETG recommended)
   - Verify fit with actual boards
   - Iterate as needed

4. **Commit to Repository**:
   ```bash
   git add CAD/<EnclosureName>/
   git commit -m "Add enclosure for <SystemName>"
   git push
   ```

---

## Contributing

Contributions to the 6502 project are welcome! Whether you're fixing bugs, adding features, testing untested boards, or improving documentation, your help is appreciated.

### How to Contribute

1. **Fork the Repository** on GitHub
2. **Clone Your Fork**:
   ```bash
   git clone https://github.com/<your-username>/6502.git
   cd 6502
   ```
3. **Create a Feature Branch**:
   ```bash
   git checkout -b feature/<your-feature-name>
   ```
4. **Make Your Changes** (see [Development Workflow](#development-workflow))
5. **Test Thoroughly** (build and verify your changes work)
6. **Commit with Clear Messages**:
   ```bash
   git add .
   git commit -m "Add <feature>: <clear description>"
   ```
7. **Push to Your Fork**:
   ```bash
   git push origin feature/<your-feature-name>
   ```
8. **Submit a Pull Request** on GitHub:
   - Describe your changes
   - Reference any related issues
   - Include photos/screenshots if applicable
   - Note testing performed

### Types of Contributions

**Hardware:**
- New card/helper designs
- Improvements to existing boards
- Bug fixes (incorrect component values, routing errors)
- Design optimizations (cost reduction, improved performance)

**Firmware:**
- New features for existing firmware
- Bug fixes
- Performance improvements
- Support for additional peripherals

**Software:**
- 6502 assembly programs and examples
- Test programs
- BIOS improvements
- Monitor/debugger enhancements

**Documentation:**
- Improved README files
- Tutorials and how-to guides
- Assembly instructions with photos
- Troubleshooting additions

**Testing:**
- Testing UNTESTED boards
- Reporting bugs and issues
- Validating fixes

### Contribution Guidelines

**Code Style:**
- **C/C++**: Follow existing style

**Commit Messages:**
- Use present tense ("Add feature" not "Added feature")
- Be specific ("Fix Serial Card address decode logic" not "Fix bug")
- Reference issues if applicable (#123)

**Hardware Design:**
- Use KiCad 7.0+
- Include all library dependencies
- Run ERC (Electrical Rules Check) and DRC (Design Rules Check) before committing
- Follow existing board naming conventions
- Generate production files for new designs

**Firmware:**
- Test on actual hardware before submitting
- Include README or comments explaining usage
- Document any new PlatformIO dependencies in platformio.ini

**Documentation:**
- Use clear, concise language
- Assume intermediate skill level (explain advanced concepts, not basics)
- Include examples where helpful
- Test all commands/procedures before documenting

### Code of Conduct

- Be respectful and constructive
- Help newcomers with patience
- Give credit to others' work
- Focus on technical merit
- Keep discussions on-topic

### Reporting Issues

**Bug Reports:**
- Use GitHub Issues
- Include:
  - Board/firmware name and revision
  - Steps to reproduce
  - Expected vs. actual behavior
  - Photos/screenshots if applicable
  - Error messages (verbatim)

**Feature Requests:**
- Describe the feature and use case
- Explain why it's valuable
- Suggest possible implementation (optional)

### Getting Help

- **GitHub Issues**: Technical questions, bug reports
- **6502.org Forums**: General 6502 development discussion
- **Reddit r/beneater**: Retro computing community

Thank you for contributing to the 6502 project!

---

## Licensing

This project is licensed under the **CERN Open Hardware Licence Version 2 - Permissive** (CERN-OHL-P v2).

### What This Means

**You are free to:**
- **Use** the designs for any purpose (personal, educational, commercial)
- **Study** the designs and learn from them
- **Modify** the designs to suit your needs
- **Distribute** copies of the original or modified designs
- **Manufacture** products based on these designs
- **Sell** products based on these designs

**Under these conditions:**
- **Attribution**: Give credit to the original designers
- **Share-Alike (for documentation only)**: If you distribute modified designs, you must make the source files available under the same license
- **No Warranty**: Designs are provided "as-is" without warranty of any kind

### Firmware Licensing

Firmware and software components may be licensed differently (check individual firmware folders). Generally:
- Original firmware: CERN-OHL-P v2 or MIT License
- Third-party libraries: Licensed under their respective licenses (see platformio.ini and library documentation)

### Using These Designs

**For Personal Use:**
- Build as many as you want
- Modify however you like
- No attribution required (but appreciated!)

**For Commercial Use:**
- You may manufacture and sell products based on these designs
- Attribution required (include link to this repository or credit in documentation)
- Provide source files if you've modified the designs (share-alike for documentation)

**For Educational Use:**
- Excellent for teaching digital design, computer architecture, embedded systems
- Use in classrooms, workshops, tutorials freely
- Attribution appreciated

### Third-Party Components

This project uses several third-party hardware modules and software libraries:
- **Pico9918**: [GitHub](https://github.com/visrealm/pico9918) - Check repository for license
- **vrEmu6502**: [GitHub](https://github.com/visrealm/vrEmu6502) - Check repository for license
- **ARMSID**: Commercial product from [RetroComp.cz](https://retrocomp.cz)
- **Arduino Libraries**: Various (MIT, GPL, Apache - see platformio.ini)

Respect the licenses of third-party components when using or redistributing.

### Full License Text

See the [LICENSE](./LICENSE) file for the complete CERN-OHL-P v2 license text.

---

**Project Maintainer:** A.C. Wright  
**Repository:** [https://github.com/acwright/6502](https://github.com/acwright/6502)  

---
