6502-COB
========

![6502-COB.png](./Images/6502-COB.png)

An **AC6502** retro-style 8-bit computer based on the **65C02** microprocessor.

> 📖 **Guide:** [AC6502 Documentation](https://acwright.github.io/6502-DOCS/) — the user's and programmer's guide for the whole family.
> See [COB's page in the family appendix](https://acwright.github.io/6502-DOCS/family/cob).

---

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [Systems](#systems)
- [Software](#software)
- [Hardware](#hardware)
  - [Backplanes](#backplanes)
  - [Cards](#cards)
  - [Helpers](#helpers)
- [Firmware](#firmware)
  - [KEH Controller](#keh-controller)
  - [PS2 Keyboard Controller](#ps2-keyboard-controller)
  - [STP Controller](#stp-controller)
- [CAD](#cad)
- [Production](#production)
- [Schematics](#schematics)
- [Checking the schematics](#checking-the-schematics)
- [Libraries](#libraries)
- [Bill of Materials](#bill-of-materials)
  - [Backplane](#backplane)
  - [Backplane Helper](#backplane-helper)
  - [Backplane Pro](#backplane-pro)
  - [Blinkenlights Card](#blinkenlights-card)
  - [Breadboard Helper](#breadboard-helper)
  - [Clock Helper](#clock-helper)
  - [CPU Card](#cpu-card)
  - [CPU Card Pro](#cpu-card-pro)
  - [DB25 Helper](#db25-helper)
  - [GPIO Breadboard Helper](#gpio-breadboard-helper)
  - [GPIO Card](#gpio-card)
  - [GPIO Helper](#gpio-helper)
  - [Joystick Helper](#joystick-helper)
  - [Keyboard Encoder Helper](#keyboard-encoder-helper)
  - [Keyboard Helper](#keyboard-helper)
  - [LCD Board](#lcd-board)
  - [LCD Card](#lcd-card)
  - [Mega Helper](#mega-helper)
  - [Memory Card](#memory-card)
  - [Prototype Card](#prototype-card)
  - [PS2 Helper](#ps2-helper)
  - [RAM Card](#ram-card)
  - [RTC Card](#rtc-card)
  - [Serial Card](#serial-card)
  - [Serial Card Pro](#serial-card-pro)
  - [Sound Card](#sound-card)
  - [Storage Card](#storage-card)
  - [Storage Card Pro](#storage-card-pro)
  - [VERA Helper](#vera-helper)
  - [VGA Card](#vga-card)
  - [VGA Card Pro](#vga-card-pro)
  - [Video Card](#video-card)
  - [Video Card Pro](#video-card-pro)
- [Purchase](#purchase)
- [License](#license)

---

## Overview

The AC6502 ecosystem is a family of open-source, 65C02-based computers sharing a common architecture, memory map, and [BIOS](https://github.com/acwright/6502-BIOS). Each computer in the family is purpose-built for a different use case but runs the same software and firmware.

The **COB** (Computer on a Backplane) is a full-featured modular desktop computer. It features a real 65C02 CPU, a backplane architecture with expandable card slots, up to 544KB RAM, composite or VGA video output, CompactFlash or SD storage, a real-time clock, and support for PS/2 keyboards, matrix keyboards, and Atari 2600-compatible joysticks.

## Architecture

All AC6502 computers share:

- **CPU**: 65C02 (or accurate emulation)
- **Memory**: 32KB RAM + 32KB ROM, with optional banked RAM expansion
- **Memory Map**: Standardized across the ecosystem — zero page, stack, I/O space ($8000–$9FFF), system ROM, and interrupt vectors at $FFFA–$FFFF
- **Bus**: 16-bit address bus, 8-bit bidirectional data bus, standard 65C02 control signals (RW, PHI2, RESET, IRQ, NMI, RDY, SYNC)
- **BIOS**: A common [BIOS](https://github.com/acwright/6502-BIOS) provides the kernel, monitor, and BASIC interpreter across all systems

## Systems

| Project | Description |
|---------|-------------|
| [6502-ACE](https://github.com/acwright/6502-ACE) | All-in-one Computer Experience — A single board computer |
| [6502-COB](https://github.com/acwright/6502-COB) | Computer On a Backplane — Modular desktop computer with expandable card slots (YOU ARE HERE) |
| [6502-DEV](https://github.com/acwright/6502-DEV) | Development Environment Vehicle — Emulation-based dev system |
| [6502-KIM](https://github.com/acwright/6502-KIM) | Keypad Input Monitor - KIM-1 inspired minimal computer |
| [6502-VCS](https://github.com/acwright/6502-VCS) | Video Computer System — Cartridge-based retro gaming console |

## Software

| Project | Description |
|---------|-------------|
| [6502-BIOS](https://github.com/acwright/6502-BIOS) | The shared BIOS (kernel, monitor, BASIC) for all AC6502 computers |
| [6502-EMULATOR](https://github.com/acwright/6502-EMULATOR) | Emulator for the whole family — desktop app, browser build, and a command line for scripted runs |
| [6502-PRG](https://github.com/acwright/6502-PRG) | Template project for writing assembly language programs |
| [6502-CRT](https://github.com/acwright/6502-CRT) | Template project for writing assembly language cartridges |
| [6502-ASM](https://github.com/acwright/6502-ASM) | Assembly language example programs and demos |
| [6502-BAS](https://github.com/acwright/6502-BAS) | BASIC program listings |
| [6502-WOZMON](https://github.com/acwright/6502-WOZMON) | Wozmon as a standalone ROM |
| [6502-NOP](https://github.com/acwright/6502-NOP) | An all-NOP ROM, for probing a board during bring-up |
| [6502-DOCS](https://github.com/acwright/6502-DOCS) | The documentation site — the guide, the printable reference cards, branding and label artwork |
| [cffs](https://github.com/acwright/cffs) | Builds CompactFlash disk images for the BIOS filesystem |
| [bastok](https://github.com/acwright/bastok) | Tokenizes BASIC listings into `.prg` images, and back |
| [bin2woz](https://github.com/acwright/bin2woz) | Converts a binary into a Wozmon serial upload |

## Hardware

This repository contains KiCad 7.0+ PCB designs for the backplanes, cards, and helpers that make up the COB system.

### Backplanes

**`Hardware/Backplane/`** — Passive backplane providing 5 card slots with full bus interconnect across all slots.

**`Hardware/Backplane Pro/`** — Enhanced backplane with integrated clock generation, reset circuitry, and power distribution plus 5 card slots. Rev 1.1 moves to entirely through-hole components and adds an onboard power switch and DC barrel jack input.

**`Hardware/Backplane Helper/`** — Adds additional card slots to expand the total backplane configuration up to 12 slots.

### Cards

**`Hardware/CPU Card/`** — Hosts the 65C02 CPU in a card form factor for backplane-based COB systems.

**`Hardware/CPU Card Pro/`** — Hosts a 65816 16-bit CPU (backward-compatible with 65C02). *(Untested)*

**`Hardware/Memory Card/`** — Provides 32KB SRAM and 32KB EEPROM with address decoding logic for COB systems using the CPU Card.

**`Hardware/RAM Card/`** — 512KB banked RAM expansion organized as two 256KB banks with 256 × 1KB windows each.

**`Hardware/RTC Card/`** — Real-time clock via Dallas DS1511Y with battery backup and 256 bytes of battery-backed SRAM.

**`Hardware/GPIO Card/`** — General-purpose I/O via 65C22 VIA supporting keyboards, joysticks, timers, and custom devices.

**`Hardware/Serial Card/`** — RS-232 serial communication via 65C51 ACIA and MAX232 level shifter.
Female DB-9 wired as **DCE**, so a straight-through cable reaches a PC. `DCD` and `DSR` are tied to
ground; `DTR` is not connected. The `CTS EN` jumper (J4) picks whether the ACIA's `CTS` comes from
ground or from the cable — **ground is the default**, which leaves the transmitter always free to
send.

**`Hardware/Serial Card Pro/`** — Enhanced serial card with full modem control signals (DTR, DCD, DSR).
Male DB-9 wired as **DTE**, like a PC, so reaching a laptop needs a **null-modem** cable — unlike the
plain Serial Card. `CTS`, `DSR` and `DTR` always reach the cable; the `DCD EN` jumper picks ground or
the cable for `DCD` — **ground is the default**.

**The handshake lines are not decoration.** On a real R6551, measured on a KIM in 2026: `CTS` high
stops the transmitter dead, and `DCD` high stops the receiver and the arriving byte is lost. A card
jumpered to take either from the cable, plugged into something that does not assert them, looks like
a dead board — no banner, no echo — until the line comes back. Ground is the default for exactly
that reason.

**`Hardware/VGA Card/`** — VGA 640×480 video output via Raspberry Pi Pico running [Pico9918](https://github.com/visrealm/pico9918) with TMS9918A-compatible graphics modes.

**`Hardware/VGA Card Pro/`** — Fully programmable VGA output using Pi Pico with custom firmware. *(Untested)*

**`Hardware/Video Card/`** — Composite video output via TMS9918A Video Display Processor with 16KB video RAM.

**`Hardware/Video Card Pro/`** — Enhanced composite video card. *(Untested)*

**`Hardware/Sound Card/`** — 3-voice SID audio output via ARMSID module with RCA line-level output.

**`Hardware/Storage Card/`** — CompactFlash storage interface supporting up to 128GB via IDE-compatible protocol.

**`Hardware/Storage Card Pro/`** — SD card (up to 32GB) and 16MB onboard SPI flash storage interface. *(Untested, experimental — the stock BIOS has no driver for it; see [STP Controller](#stp-controller))*

**`Hardware/LCD Card/`** — 16×2 character LCD display via 65C22 VIA in 4-bit or 8-bit mode.

**`Hardware/Blinkenlights Card/`** — Visual bus monitoring with LEDs showing address bus (16), data bus (8), and control signals (8) in real time.

**`Hardware/Prototype Card/`** — Blank prototyping area with integrated breadboard and full bus access headers.

### Helpers

**`Hardware/Keyboard Encoder Helper/`** — ATmega1284p-based dual keyboard encoder supporting PS/2 keyboard and 8×8 matrix keyboard simultaneously via 65C22 VIA.

**`Hardware/PS2 Helper/`** — ATmega328p-based PS/2 keyboard bridge that drives an MT8808 analog crosspoint switch to emulate a matrix keyboard. Its output is a raw 8×8 matrix and feeds the matrix inputs of a Keyboard Encoder Helper, which is what talks to the 65C22 VIA — not the VIA directly.

**`Hardware/Keyboard Helper/`** — 64-key matrix keyboard with Atari 2600-compatible joystick support.

**`Hardware/GPIO Helper/`** — 8 buttons and 8 LEDs for testing and debugging GPIO Card connections.

**`Hardware/GPIO Breadboard Helper/`** — Breadboard interface adapter for the GPIO Card.

**`Hardware/Joystick Helper/`** — Atari 2600-style dual joystick connector and interface board.

**`Hardware/Clock Helper/`** — Manual clock control providing single-step, free-run, and halt modes for debugging real CPU systems.

**`Hardware/DB25 Helper/`** — GPIO to DB25 connector adapter for external device interfacing.

**`Hardware/Breadboard Helper/`** — Exposes the full AC6502 bus to a breadboard for prototyping and experimentation.

**`Hardware/Mega Helper/`** — Arduino Mega 2560 interface adapter for the AC6502 bus.

**`Hardware/LCD Board/`** — 320×240 TFT LCD display (ILI9341) interfaced via 65C22 VIA. *(Untested)*

**`Hardware/VERA Helper/`** — VERA FPGA video module adapter for the AC6502 bus.

## Firmware

This repository contains [PlatformIO](https://platformio.org/)-based firmware for the COB system's microcontroller helpers.

### KEH Controller
`Firmware/KEH Controller/`

Firmware for the ATmega1284p on the Keyboard Encoder Helper. Provides:

- Simultaneous PS/2 keyboard and 8×8 matrix keyboard scanning
- ASCII conversion with modifier key support (Shift, Ctrl)
- Buffered, debounced input handling
- Parallel I/O to the AC6502 bus via 65C22 VIA

See [Firmware/KEH Controller/README.md](./Firmware/KEH%20Controller/README.md) for setup and usage instructions.

### PS2 Keyboard Controller
`Firmware/PS2 Keyboard Controller/`

Firmware for the ATmega328p on the PS2 Helper. Provides:

- Interrupt-driven PS/2 keyboard scan code reading
- MT8808 analog crosspoint switch matrix control
- Make/break detection and extended scan code support
- Function key emulation via FN key combinations

The MT8808 closes row/column contacts in place of physical key switches, so this
card connects to the **matrix inputs of a Keyboard Encoder Helper**, not to a
65C22 VIA. The encoder does the scanning and ASCII conversion; wired straight to
a VIA the card produces nothing, as the BIOS has no matrix-scanning code.

See [Firmware/PS2 Keyboard Controller/README.md](./Firmware/PS2%20Keyboard%20Controller/README.md) for setup and usage instructions.

### STP Controller
`Firmware/STP Controller/`

Firmware for the ATmega328p on the Storage Card Pro. Provides:

- Memory-mapped SPI controller for 6502 bus access
- Support for SD card, 16MB SPI flash, and external SPI devices
- Dual-speed SPI (4 MHz normal operation, 400 kHz initialization)
- PHI2-synchronized interface for reliable 6502 communication

⚠️ **Experimental.** The Storage Card Pro is not part of the default I/O set the
BIOS probes for, and no COB machine is built around it today. The BIOS storage
slot expects eight True IDE CompactFlash registers at `$8C00–$8C07`; this card
presents two SPI registers instead, so a stock BIOS reports `NO DEVICE` for every
storage path. Using it needs a modified BIOS or software that drives the two
registers directly.

See [Firmware/STP Controller/README.md](./Firmware/STP%20Controller/README.md) for setup and usage instructions.

## CAD
`CAD/`

3D-printable enclosure parts and laser-cut top panels for the COB system.

## Production
`Production/`

JLCPCB-ready Gerber files and BOM/CPL for PCB fabrication and assembly.

## Schematics
`Schematics/`

PDF schematics for each board.

## Checking the schematics
`Tests/check-schematics.mjs`

The RAM Card's bank latch decides which kilobyte of its 512 KB each window shows, and a latch that loads on the wrong address corrupts a bank without any sign. The ACE's equivalent latch was wrong twice while its gates looked right on paper, so this script checks the logic instead of reading it. It asks KiCad for the card's netlist, builds the glue logic from the gates it finds there, fits the card's jumpers, and drives every address, as a read and as a write, with PHI2 high and low. That is 262,144 bus states. For each one it compares the RAM's chip select, read and write strobes and A18, and the latch enable, against the memory map, and checks that the card drives the data bus only on a read of its own window. A net that nothing drives counts as a failure, so a missing wire fails too.

It checks the RAM Card Rev 1.0 with both windows jumpered, IO 1 (`$8000`) and IO 2 (`$8400`), which is the only setting Rev 1.0 supports: R1 and R2 pull the window selects to ground, so a window left without a jumper is selected at every address.

Run it after any change to a checked schematic:

```sh
node Tests/check-schematics.mjs
```

It needs Node.js and KiCad's `kicad-cli`, which it finds on the `PATH`, at the macOS default location, or through a `KICAD_CLI` environment variable. It takes a few seconds, prints `ok` or `FAIL` for each check with example bus states for any failure, and exits non-zero on a failure.

## Libraries
`Libraries/`

Shared KiCad symbol and footprint libraries used across all AC6502 hardware projects.

## Bill of Materials

### Backplane

| Reference | Qty | Value | Description | DigiKey | Mouser |
|-----------|-----|-------|-------------|---------|--------|
| J1–J5 | 5 | 6502 Card Connector | Card Edge 2×20 | [A31723-ND](https://www.digikey.com/en/products/filter?keywords=A31723-ND) | [571-5-5530843-4](https://www.mouser.com/ProductDetail/571-5-5530843-4) |
| J6, J7 | 2 | 6502 Bus | Bus Connector 2×20 | [732-5401-ND](https://www.digikey.com/en/products/filter?keywords=732-5401-ND) | |

### Backplane Helper

| Reference | Qty | Value | Description | DigiKey | Mouser |
|-----------|-----|-------|-------------|---------|--------|
| J1, J2 | 2 | 6502 Card Connector | Card Edge 2×20 | [A31723-ND](https://www.digikey.com/en/products/filter?keywords=A31723-ND) | [571-5-5530843-4](https://www.mouser.com/ProductDetail/571-5-5530843-4) |
| J3 | 1 | 6502 Bus | Bus Connector 2×20 | [732-5401-ND](https://www.digikey.com/en/products/filter?keywords=732-5401-ND) | |

### Backplane Pro

#### Rev 1.1

| Reference | Qty | Value | Description | Mouser |
|-----------|-----|-------|-------------|--------|
| C1, C3 | 2 | 10uF | Polarized capacitor | |
| C2 | 1 | 100nF | Unpolarized capacitor | |
| D1 | 1 | LED | Power LED 3mm | |
| J1 | 1 | 5V DC | DC Barrel Jack | [640-DCJ200-10-A-K1-K](https://www.mouser.com/ProductDetail/640-DCJ200-10-A-K1-K) |
| J2–J4, J7, J8 | 5 | 6502 Card Connector | Card Edge 2×20 | [571-5-5530843-4](https://www.mouser.com/ProductDetail/571-5-5530843-4) |
| J5 | 1 | CLOCK ENBL SW | Pin Header 1×2 | |
| J6 | 1 | RESET SW | Pin Header 1×2 | |
| J9 | 1 | 6502 Bus | Bus Connector 2×20 | |
| JP1 | 1 | Solder Jumper | Solder Jumper 2-pole | |
| Q1 | 1 | 2N3904 | NPN Transistor TO-92 | |
| R1 | 1 | 330 | Resistor | |
| R2 | 1 | 1M | Resistor | |
| R3 | 1 | 47k | Resistor | |
| R4 | 1 | 10k | Resistor | |
| R5 | 1 | 1k | Resistor | |
| SW1 | 1 | POWER | SPDT Power Switch | [612-400MSP1R6BLKM6QE](https://www.mouser.com/ProductDetail/612-400MSP1R6BLKM6QE) |
| SW2 | 1 | RESET | Tact Push Button 5mm | |
| U1 | 1 | LM555 | Timer SOIC-8 | |
| X1 | 1 | OCXO-14 | Crystal Oscillator DIP-14 | |

#### Rev 1.0

| Reference | Qty | Value | Description | LCSC | DigiKey | Mouser |
|-----------|-----|-------|-------------|------|---------|--------|
| C1, C3–C14 | 13 | 100nF | Unpolarized capacitor | [C49678](https://www.lcsc.com/search?q=C49678) | | |
| C2 | 1 | 10uF | Polarized capacitor | [C7171](https://www.lcsc.com/search?q=C7171) | | |
| D1 | 1 | 1N5819 | Schottky Diode SOD-323F | [C191023](https://www.lcsc.com/search?q=C191023) | | |
| D2 | 1 | LED | LED 0805 | [C2297](https://www.lcsc.com/search?q=C2297) | | |
| J1 | 1 | PWR_SW | JST XH 1×2 | | [455-2247-ND](https://www.digikey.com/en/products/filter?keywords=455-2247-ND) | |
| J2 | 1 | USB-C | USB 2.0 Type-C Receptacle | [C2988369](https://www.lcsc.com/search?q=C2988369) | | |
| J3 | 1 | RESET_SW | JST XH 1×2 | | [455-2247-ND](https://www.digikey.com/en/products/filter?keywords=455-2247-ND) | |
| J4 | 1 | CLOCK_ENBL_SW | JST XH 1×2 | | [455-2247-ND](https://www.digikey.com/en/products/filter?keywords=455-2247-ND) | |
| J5–J9 | 5 | 6502 Card Connector | Card Edge 2×20 | | [A31723-ND](https://www.digikey.com/en/products/filter?keywords=A31723-ND) | [571-5-5530843-4](https://www.mouser.com/ProductDetail/571-5-5530843-4) |
| J10 | 1 | 6502 Bus | Bus Connector 2×20 | | | |
| Q1 | 1 | SS8050 | NPN Transistor SOT-23 | [C2150](https://www.lcsc.com/search?q=C2150) | | |
| R1 | 1 | 1M | Resistor | [C17514](https://www.lcsc.com/search?q=C17514) | | |
| R2 | 1 | 47k | Resistor | [C17713](https://www.lcsc.com/search?q=C17713) | | |
| R3 | 1 | 1k | Resistor | [C17513](https://www.lcsc.com/search?q=C17513) | | |
| R4 | 1 | 10k | Resistor | [C17414](https://www.lcsc.com/search?q=C17414) | | |
| R5, R6 | 2 | 5.1k | Resistor | [C27834](https://www.lcsc.com/search?q=C27834) | | |
| R7 | 1 | 330 | Resistor | [C17630](https://www.lcsc.com/search?q=C17630) | | |
| SW1 | 1 | Reset | Push Button | [C318884](https://www.lcsc.com/search?q=C318884) | | |
| U1 | 1 | LM555xM | Timer SOIC-8 | [C7593](https://www.lcsc.com/search?q=C7593) | | |
| X1 | 1 | OCXO-14 | Crystal Oscillator DIP-14 | | | |

### Blinkenlights Card

| Reference | Qty | Value | Description | LCSC |
|-----------|-----|-------|-------------|------|
| C1–C4 | 4 | 100nF | Unpolarized capacitor | [C49678](https://www.lcsc.com/search?q=C49678) |
| D1–D25 | 25 | LED | LED 0805 | [C2297](https://www.lcsc.com/search?q=C2297) |
| D26, D27 | 2 | LED | LED 0805 | [C2295](https://www.lcsc.com/search?q=C2295) |
| D28–D30 | 3 | LED | LED 0805 | [C2296](https://www.lcsc.com/search?q=C2296) |
| D31, D32 | 2 | LED | LED 0805 | [C2293](https://www.lcsc.com/search?q=C2293) |
| R1–R32 | 32 | 330 | Resistor | [C17630](https://www.lcsc.com/search?q=C17630) |
| U1, U2, U4 | 3 | 74HC573 | 8-bit Latch SOIC-20 | [C5608](https://www.lcsc.com/search?q=C5608) |
| U3 | 1 | 74HC04 | Hex Inverter SOIC-14 | [C5590](https://www.lcsc.com/search?q=C5590) |

### Breadboard Helper

| Reference | Qty | Value | Description | DigiKey |
|-----------|-----|-------|-------------|---------|
| J1 | 1 | 6502 Bus | Bus Connector 2×20 | [732-5401-ND](https://www.digikey.com/en/products/filter?keywords=732-5401-ND) |
| J2, J3 | 2 | Pin Header | Pin Header 1×20 | |

### Clock Helper

| Reference | Qty | Value | Description |
|-----------|-----|-------|-------------|
| C1 | 1 | 100nF | Capacitor |
| D1, D2 | 2 | LED | LED 3mm |
| J1 | 1 | IO | Pin Header 1×5 |
| R1–R5 | 5 | 1k | Resistor |
| R6–R8 | 3 | 330 | Resistor |
| SW1, SW2 | 2 | XKB5858-Z | DPDT Toggle Switch |
| SW3 | 1 | XKB5858-W | DPDT Toggle Switch |
| U1 | 1 | 74HC74 | Dual D Flip-flop DIP-14 |

### CPU Card

| Reference | Qty | Value | Description | LCSC |
|-----------|-----|-------|-------------|------|
| C1 | 1 | 100nF | Unpolarized capacitor | [C49678](https://www.lcsc.com/search?q=C49678) |
| J2 | 1 | Pin Header | Pin Header 1×2 | |
| J3 | 1 | EXP | Pin Header 2×5 | |
| R1–R6 | 6 | 10k | Resistor | [C2930231](https://www.lcsc.com/search?q=C2930231) |
| U1 | 1 | 65C02 | CPU DIP-40 | |

#### Revision History

**Rev 1.1**

- Pull-up resistors changed from 1kΩ to 10kΩ.

**Rev 1.0**

- Initial release.

### CPU Card Pro

| Reference | Qty | Value | Description | LCSC |
|-----------|-----|-------|-------------|------|
| C1–C4 | 4 | 100nF | Unpolarized capacitor | [C49678](https://www.lcsc.com/search?q=C49678) |
| J2 | 1 | BANK ADDR | Pin Header 2×5 | |
| J3 | 1 | EXP | Pin Header 2×4 | |
| R1–R6 | 6 | 1k | Resistor | [C17513](https://www.lcsc.com/search?q=C17513) |
| U1 | 1 | 65C816 | CPU DIP-40 | |
| U2 | 1 | 74HC00 | Quad NAND SOIC-14 | [C5586](https://www.lcsc.com/search?q=C5586) |
| U3 | 1 | 74HC245 | Octal Bus Transceiver TSSOP-20 | [C5626](https://www.lcsc.com/search?q=C5626) |
| U4 | 1 | 74HC573 | 8-bit Latch SOIC-20 | [C5608](https://www.lcsc.com/search?q=C5608) |

### DB25 Helper

| Reference | Qty | Value | Description | DigiKey |
|-----------|-----|-------|-------------|---------|
| J1 | 1 | PORT A | GPIO Connector 2×6 | |
| J2 | 1 | PORT B | GPIO Connector 2×6 | |
| J3 | 1 | KEYBOARD | DB-25 Connector (Male) | [609-1505-ND](https://www.digikey.com/en/products/filter?keywords=609-1505-ND) |

### GPIO Breadboard Helper

| Reference | Qty | Value | Description |
|-----------|-----|-------|-------------|
| J1 | 1 | PORT A | GPIO Connector 2×6 |
| J2 | 1 | PORT A | Pin Header 1×12 |
| J3 | 1 | PORT B | Pin Header 1×12 |
| J4 | 1 | PORT B | GPIO Connector 2×6 |

### GPIO Card

| Reference | Qty | Value | Description | LCSC |
|-----------|-----|-------|-------------|------|
| C1, C2 | 2 | 100nF | Unpolarized capacitor | [C49678](https://www.lcsc.com/search?q=C49678) |
| D1 | 1 | Schottky Diode | Schottky Diode DO-35 | |
| J2 | 1 | IO SELECT | Pin Header 2×8 | |
| J3 | 1 | PORT A | GPIO Connector 2×6 | |
| J4 | 1 | PORT B | GPIO Connector 2×6 | |
| U1 | 1 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) |
| U2 | 1 | 65C22 | VIA DIP-40 | |

### GPIO Helper

| Reference | Qty | Value | Description | DigiKey |
|-----------|-----|-------|-------------|---------|
| D1–D8 | 8 | LED | LED 3mm | |
| J1 | 1 | PORT A | GPIO Connector 2×6 | |
| J2 | 1 | PORT B | GPIO Connector 2×6 | |
| R1–R8 | 8 | 1k | Resistor | |
| R9–R16 | 8 | 330 | Resistor | |
| SW1–SW8 | 8 | Push Button | Push Button 6mm | [2223-TS02-66-50-BK-160-LCR-D-ND](https://www.digikey.com/en/products/filter?keywords=2223-TS02-66-50-BK-160-LCR-D-ND) |

### Joystick Helper

| Reference | Qty | Value | Description | DigiKey |
|-----------|-----|-------|-------------|---------|
| J1 | 1 | PORT | GPIO Connector 2×6 | |
| J2 | 1 | DB-9 Male | DB-9 Connector (Male) | [609-1481-ND](https://www.digikey.com/en/products/filter?keywords=609-1481-ND) |
| R1–R8 | 8 | 1k | Resistor | |

### Keyboard Encoder Helper

| Reference | Qty | Value | Description | DigiKey |
|-----------|-----|-------|-------------|---------|
| C1, C4 | 2 | 100nF | Capacitor | |
| C2, C3 | 2 | 22pF | Capacitor | |
| J1 | 1 | PORT B | GPIO Connector 2×6 | |
| J2 | 1 | PORT A | GPIO Connector 2×6 | |
| J3 | 1 | PS/2 KEYBOARD | 6-pin Mini-DIN | |
| J4 | 1 | KEYBOARD | DB-25 Connector (Male) | [609-1505-ND](https://www.digikey.com/en/products/filter?keywords=609-1505-ND) |
| R1 | 1 | 1k | Resistor | |
| U1 | 1 | ATmega1284-P | MCU DIP-40 | |
| Y1 | 1 | 16 MHz | Crystal HC49-U | [3155-16M20P2/49US-ND](https://www.digikey.com/en/products/filter?keywords=3155-16M20P2/49US-ND) |

### Keyboard Helper

| Reference | Qty | Value | Description | DigiKey |
|-----------|-----|-------|-------------|---------|
| J1, J2 | 2 | JOYSTICK | DB-9 Connector (Male) | [609-1481-ND](https://www.digikey.com/en/products/filter?keywords=609-1481-ND) |
| J3, J4 | 2 | JOYSTICK | GPIO Connector 2×6 | |
| J5, J6 | 2 | PORT | GPIO Connector 2×6 | |
| J7 | 1 | KEYBOARD | DB-25 Connector (Male) | [609-1505-ND](https://www.digikey.com/en/products/filter?keywords=609-1505-ND) |
| R1–R16 | 16 | 1k | Resistor | |
| SW1–SW67 | 67 | Cherry MX | Key Switch | |

### LCD Board

| Reference | Qty | Value | Description |
|-----------|-----|-------|-------------|
| C1–C4 | 4 | 100nF | Capacitor |
| D1, D2 | 2 | Schottky Diode | Schottky Diode DO-35 |
| J1 | 1 | 6502 Bus | Bus Connector 2×20 |
| J2 | 1 | IO SELECT | Pin Header 2×8 |
| J3 | 1 | HVSYNC | Pin Header 1×1 |
| R1 | 1 | 10k | Resistor |
| U1 | 1 | 65C22 | VIA DIP-40 |
| U2 | 1 | Adafruit 2.8" TFT | TFT LCD Module |
| U3 | 1 | 74HC138 | Decoder DIP-16 |

### LCD Card

| Reference | Qty | Value | Description | LCSC |
|-----------|-----|-------|-------------|------|
| C1, C2 | 2 | 100nF | Unpolarized capacitor | [C49678](https://www.lcsc.com/search?q=C49678) |
| D1 | 1 | Schottky Diode | Schottky Diode DO-35 | |
| J2 | 1 | GPIO | Pin Header 1×7 | |
| J3 | 1 | IO SELECT | Pin Header 2×8 | |
| R1 | 1 | R | Resistor | |
| U1 | 1 | 65C22 | VIA DIP-40 | |
| U2 | 1 | LCD-16X2 | 16×2 Character LCD | |
| U3 | 1 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) |
| VR1 | 1 | 10k | Potentiometer | |

### Mega Helper

| Reference | Qty | Value | Description |
|-----------|-----|-------|-------------|
| A1 | 1 | Arduino Mega 2560 | Arduino Mega 2560 R3 Shield |
| J1 | 1 | 6502 Bus | Bus Connector 2×20 |

### Memory Card

| Reference | Qty | Value | Description | LCSC |
|-----------|-----|-------|-------------|------|
| C1–C5 | 5 | 100nF | Unpolarized capacitor | [C49678](https://www.lcsc.com/search?q=C49678) |
| J2 | 1 | Pin Header | Pin Header 2×2 | |
| J3 | 1 | Pin Header | Pin Header 1×2 | |
| R1, R2 | 2 | 1k | Resistor | [C17513](https://www.lcsc.com/search?q=C17513) |
| U1, U2, U5 | 3 | 74HC00 | Quad NAND SOIC-14 | [C5586](https://www.lcsc.com/search?q=C5586) |
| U3 | 1 | AS6C62256 | SRAM DIP-28 | |
| U4 | 1 | AT28C256 | EEPROM DIP-28 | |

### Prototype Card

| Reference | Qty | Value | Description | LCSC |
|-----------|-----|-------|-------------|------|
| C1–C3 | 3 | 100nF | Unpolarized capacitor | [C49678](https://www.lcsc.com/search?q=C49678) |
| J2 | 1 | Bus Header | Pin Socket 2×20 | |
| J3 | 1 | EXP | Pin Header 2×4 | |
| U1 | 1 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) |

### PS2 Helper

| Reference | Qty | Value | Description |
|-----------|-----|-------|-------------|
| C1–C3 | 3 | 100nF | Disc Capacitor |
| C4, C5 | 2 | 22pF | Disc Capacitor |
| J1 | 1 | PORT A | GPIO Connector 2×6 |
| J2 | 1 | PORT B | GPIO Connector 2×6 |
| J3 | 1 | PS/2 | Mini-DIN 6-pin |
| J4 | 1 | JOYSTICK | GPIO Connector 2×6 |
| R1–R9 | 9 | 1k | Resistor |
| U1 | 1 | MT8808 | Analog Matrix Switch DIP-28 |
| U2 | 1 | ATmega328 | MCU DIP-28 |
| Y1 | 1 | 16 MHz | Crystal |

### RAM Card

| Reference | Qty | Value | Description | LCSC |
|-----------|-----|-------|-------------|------|
| C1–C7 | 7 | 100nF | Unpolarized Capacitor | [C49678](https://www.lcsc.com/search?q=C49678) |
| J2 | 1 | LATCH EN | Pin Header 1×2 | |
| J3 | 1 | Conn_02x08 | Pin Header 2×8 | |
| J4 | 1 | BANK ADDR | Pin Header 2×5 | |
| R1, R2 | 2 | 10k | Resistor | [C17414](https://www.lcsc.com/search?q=C17414) |
| U1 | 1 | AS6C4008 | SRAM DIP-32 | |
| U2 | 1 | 74HC573 | Octal Latch SOIC-20 | [C5608](https://www.lcsc.com/search?q=C5608) |
| U3, U6 | 2 | 74HC00 | Quad NAND SOIC-14 | [C5586](https://www.lcsc.com/search?q=C5586) |
| U4 | 1 | 74HC21 | Dual 4-input AND SOIC-14 | [C75398](https://www.lcsc.com/search?q=C75398) |
| U5 | 1 | 74HC30 | 8-input NAND SOIC-14 | [C19818870](https://www.lcsc.com/search?q=C19818870) |
| U7 | 1 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) |

### RTC Card

| Reference | Qty | Value | Description | LCSC | DigiKey |
|-----------|-----|-------|-------------|------|---------|
| BT1 | 1 | BAT-HLD-001-THM | Coin Cell Battery Holder | | |
| C1–C3 | 3 | 100nF | Unpolarized Capacitor | [C49678](https://www.lcsc.com/search?q=C49678) | |
| D1 | 1 | Schottky Diode | Schottky Diode DO-35 | | |
| J2 | 1 | AUX | Pin Header 1×6 | | |
| J3 | 1 | INT Select | Pin Header 1×3 | | |
| J4 | 1 | Conn_02x08 | Pin Header 2×8 | | |
| R1 | 1 | 1k | Resistor | [C17513](https://www.lcsc.com/search?q=C17513) | |
| U1 | 1 | DS1511Y | RTC DIP-28 | | [DS1511Y+-ND](https://www.digikey.com/en/products/filter?keywords=DS1511Y+-ND) |
| U2, U3 | 2 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) | |

### Serial Card

| Reference | Qty | Value | Description | LCSC | DigiKey |
|-----------|-----|-------|-------------|------|---------|
| C1, C6, C8 | 3 | 100nF | Unpolarized Capacitor | [C49678](https://www.lcsc.com/search?q=C49678) | |
| C2–C5 | 4 | 1uF | Unpolarized Capacitor | [C28323](https://www.lcsc.com/search?q=C28323) | |
| C7 | 1 | 22pF | Unpolarized Capacitor | [C1804](https://www.lcsc.com/search?q=C1804) | |
| D1 | 1 | Schottky Diode | Schottky Diode DO-35 | | |
| J2 | 1 | DB9 FEMALE (DCE) | DB-9 Connector (Female) | | [609-1487-ND](https://www.digikey.com/en/products/filter?keywords=609-1487-ND) |
| J3 | 1 | Conn_02x08 | Pin Header 2×8 | | |
| J4 | 1 | CTS EN | Pin Header 1×3 | | |
| R1 | 1 | 1M | Resistor | [C103906](https://www.lcsc.com/search?q=C103906) | |
| U1 | 1 | MAX232 | RS-232 Driver/Receiver | [C59824](https://www.lcsc.com/search?q=C59824) | |
| U2 | 1 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) | |
| U3 | 1 | 6551 | ACIA DIP-28 | | |
| Y1 | 1 | 1.8432 MHz | Crystal | | |

### Serial Card Pro

| Reference | Qty | Value | Description | LCSC | DigiKey |
|-----------|-----|-------|-------------|------|---------|
| C1, C2, C11, C13 | 4 | 100nF | Unpolarized Capacitor | [C49678](https://www.lcsc.com/search?q=C49678) | |
| C3–C10 | 8 | 1uF | Unpolarized Capacitor | [C28323](https://www.lcsc.com/search?q=C28323) | |
| C12 | 1 | 22pF | Unpolarized Capacitor | [C1804](https://www.lcsc.com/search?q=C1804) | |
| D1 | 1 | Schottky Diode | Schottky Diode DO-35 | | |
| J2 | 1 | DB9 MALE (DTE) | DB-9 Connector (Male) | | [609-1481-ND](https://www.digikey.com/en/products/filter?keywords=609-1481-ND) |
| J3 | 1 | Conn_02x08 | Pin Header 2×8 | | |
| J4 | 1 | DCD Select | Pin Header 1×3 | | |
| R1 | 1 | 1M | Resistor | [C103906](https://www.lcsc.com/search?q=C103906) | |
| U1, U2 | 2 | MAX232 | RS-232 Driver/Receiver | [C59824](https://www.lcsc.com/search?q=C59824) | |
| U3 | 1 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) | |
| U4 | 1 | 6551 | ACIA DIP-28 | | |
| Y1 | 1 | 1.8432 MHz | Crystal | | |

### Sound Card

| Reference | Qty | Value | Description | LCSC | DigiKey |
|-----------|-----|-------|-------------|------|---------|
| C1, C2 | 2 | 2.2nF | Unpolarized Capacitor | [C28260](https://www.lcsc.com/search?q=C28260) | |
| C3, C5 | 2 | 100nF | Unpolarized Capacitor | [C49678](https://www.lcsc.com/search?q=C49678) | |
| C4 | 1 | 1uF | Unpolarized Capacitor | [C28323](https://www.lcsc.com/search?q=C28323) | |
| J2 | 1 | VDD | JST XH 1×2 | | |
| J3 | 1 | AUDIO | RCA Connector | | [PJRAN1X1U02X](https://www.digikey.com/en/products/detail/switchcraft-inc/PJRAN1X1U02X/1832412) |
| J5 | 1 | Conn_02x08 | Pin Header 2×8 | | |
| R1 | 1 | 1k | Resistor | [C17513](https://www.lcsc.com/search?q=C17513) | |
| U1 | 1 | 6581 | SID Chip DIP-28 | | |
| U2 | 1 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) | |

### Storage Card

| Reference | Qty | Value | Description | LCSC |
|-----------|-----|-------|-------------|------|
| C1, C2 | 2 | 100nF | Unpolarized Capacitor | [C49678](https://www.lcsc.com/search?q=C49678) |
| D1 | 1 | LED | Activity LED 0805 | [C2297](https://www.lcsc.com/search?q=C2297) |
| J2 | 1 | Conn_02x08 | Pin Header 2×8 | |
| J3 | 1 | Compact Flash | CompactFlash Connector | [C444350](https://www.lcsc.com/search?q=C444350) |
| J4 | 1 | ACT LED | JST XH 1×2 | |
| R1–R4 | 4 | 1k | Resistor | [C17513](https://www.lcsc.com/search?q=C17513) |
| R5, R6 | 2 | 330 | Resistor | [C17630](https://www.lcsc.com/search?q=C17630) |
| U1 | 1 | 74HC00 | Quad NAND SOIC-14 | [C5586](https://www.lcsc.com/search?q=C5586) |
| U2 | 1 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) |

### Storage Card Pro

| Reference | Qty | Value | Description | LCSC | DigiKey |
|-----------|-----|-------|-------------|------|---------|
| C1–C4, C7, C8 | 6 | 100nF | Unpolarized Capacitor | [C49678](https://www.lcsc.com/search?q=C49678) | |
| C5, C6 | 2 | 22pF | Disc Capacitor | [C107114](https://www.lcsc.com/search?q=C107114) | |
| C9, C10 | 2 | 1uF | Unpolarized Capacitor | [C28323](https://www.lcsc.com/search?q=C28323) | |
| D1 | 1 | LED | Activity LED 0805 | [C2297](https://www.lcsc.com/search?q=C2297) | |
| J1 | 1 | ACT LED | JST XH 1×2 | | |
| J3 | 1 | SPI | Pin Header 1×6 | | |
| J4 | 1 | SD CARD | SD Card Connector | [C569097](https://www.lcsc.com/search?q=C569097) | |
| J5 | 1 | Conn_02x08 | Pin Header 2×8 | | |
| R1, R2 | 2 | 330 | Resistor | [C17630](https://www.lcsc.com/search?q=C17630) | |
| R3, R4 | 2 | 1k | Resistor | [C17513](https://www.lcsc.com/search?q=C17513) | |
| U1 | 1 | TXS0108EPW | Level Shifter TSSOP-20 | [C17206](https://www.lcsc.com/search?q=C17206) | |
| U2 | 1 | ATmega328 | MCU DIP-28 | | |
| U3 | 1 | W25Q128JVS | Flash Memory SOIC-8 | [C97521](https://www.lcsc.com/search?q=C97521) | |
| U4 | 1 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) | |
| U5 | 1 | LP2985-3.3 | LDO Regulator 3.3V SOT-23-5 | [C95414](https://www.lcsc.com/search?q=C95414) | |
| Y1 | 1 | 16 MHz | Crystal | | [3155-16M20P2/49US-ND](https://www.digikey.com/en/products/filter?keywords=3155-16M20P2/49US-ND) |

### VERA Helper

| Reference | Qty | Value | Description | DigiKey |
|-----------|-----|-------|-------------|---------|
| C1, C2 | 2 | 100nF | Disc Capacitor | |
| D1 | 1 | Schottky Diode | Schottky Diode DO-35 | |
| J1 | 1 | 6502 Bus | Bus Connector 2×20 | |
| J2 | 1 | VERA Module | Pin Header 2×12 | |
| J3 | 1 | Audio L | RCA Connector | [PJRAN1X1U02X](https://www.digikey.com/en/products/detail/switchcraft-inc/PJRAN1X1U02X/1832412) |
| J4 | 1 | Audio R | RCA Connector | [PJRAN1X1U03X](https://www.digikey.com/en/products/detail/switchcraft-inc/PJRAN1X1U03X/1832413) |
| J5 | 1 | INT Select | Pin Header 1×3 | |
| J6 | 1 | Conn_02x08 | Pin Header 2×8 | |
| U1, U2 | 2 | 74HC138 | Decoder DIP-16 | |

### VGA Card

| Reference | Qty | Value | Description | LCSC |
|-----------|-----|-------|-------------|------|
| C1, C2 | 2 | 100nF | Unpolarized Capacitor | [C49678](https://www.lcsc.com/search?q=C49678) |
| D1 | 1 | Schottky Diode | Schottky Diode DO-35 | |
| J2 | 1 | INT Select | Pin Header 1×3 | |
| J3 | 1 | Conn_02x08 | Pin Header 2×8 | |
| U1 | 1 | Pico9918A | VDP Module DIP-40 | |
| U2, U3 | 2 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) |

### VGA Card Pro

| Reference | Qty | Value | Description | LCSC |
|-----------|-----|-------|-------------|------|
| C1–C6 | 6 | 100nF | Unpolarized Capacitor | [C49678](https://www.lcsc.com/search?q=C49678) |
| D1 | 1 | Schottky Diode | Schottky Diode DO-35 | |
| J2 | 1 | VGA | DB-15 VGA Connector (Female) | |
| J3 | 1 | INT Select | Pin Header 1×3 | |
| J4 | 1 | Conn_02x08 | Pin Header 2×8 | |
| Q1 | 1 | BSS138 | N-Channel MOSFET SOT-23 | [C78284](https://www.lcsc.com/search?q=C78284) |
| R1, R2, R4, R7 | 4 | 1k | Resistor | [C17513](https://www.lcsc.com/search?q=C17513) |
| R3, R6 | 2 | 2.2k | Resistor | [C17520](https://www.lcsc.com/search?q=C17520) |
| R5, R8 | 2 | 470 | Resistor | [C17710](https://www.lcsc.com/search?q=C17710) |
| R9 | 1 | 820 | Resistor | [C17837](https://www.lcsc.com/search?q=C17837) |
| R10 | 1 | 390 | Resistor | [C17655](https://www.lcsc.com/search?q=C17655) |
| R11, R12 | 2 | 22 | Resistor | [C17561](https://www.lcsc.com/search?q=C17561) |
| U1 | 1 | SN74LVC4245APWR | Octal Bus Transceiver TSSOP-24 | [C7859](https://www.lcsc.com/search?q=C7859) |
| U2 | 1 | 74LVC245 | Octal Bus Transceiver TSSOP-20 | [C6082](https://www.lcsc.com/search?q=C6082) |
| U3 | 1 | Raspberry Pi Pico | MCU Module | |
| U4 | 1 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) |
| U5 | 1 | 74LVC138 | Decoder SOIC-16 | [C6061](https://www.lcsc.com/search?q=C6061) |

### Video Card

| Reference | Qty | Value | Description | LCSC | Mouser | DigiKey |
|-----------|-----|-------|-------------|------|--------|---------|
| C1, C2 | 2 | 32pF | Disc Capacitor | [C107114](https://www.lcsc.com/search?q=C107114) | | |
| C3–C10 | 8 | 100nF | Unpolarized Capacitor | [C49678](https://www.lcsc.com/search?q=C49678) | | |
| D1 | 1 | Schottky Diode | Schottky Diode DO-35 | | | |
| FB1 | 1 | BLM41PG600SN1L | Ferrite Bead 1806 | [C85844](https://www.lcsc.com/search?q=C85844) | | |
| J1 | 1 | INT Select | Pin Header 1×3 | | | |
| J3 | 1 | Conn_02x08 | Pin Header 2×8 | | | |
| J4 | 1 | VIDEO | RCA Connector | | | [PJRAN1X1U04X](https://www.digikey.com/en/products/detail/switchcraft-inc/PJRAN1X1U04X/969899) |
| Q1 | 1 | 2N4401 | NPN BJT TO-92 | | | |
| R1 | 1 | 470 | Resistor | [C17710](https://www.lcsc.com/search?q=C17710) | | |
| R2 | 1 | 75 | Resistor | [C17820](https://www.lcsc.com/search?q=C17820) | | |
| U1 | 1 | TMS9918A | VDP DIP-40 | | | |
| U2, U5, U6 | 3 | 74HCT574 | 8-bit Register SOIC-20 | [C6001](https://www.lcsc.com/search?q=C6001) | | |
| U3, U4 | 2 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) | | |
| U7 | 1 | AS6C62256 | SRAM DIP-28 | | | |
| U8 | 1 | 74HCT04 | Hex Inverter SO-14 | [C672096](https://www.lcsc.com/search?q=C672096) | | |
| Y1 | 1 | 10.738635 MHz | Crystal | | [774-MP107-E](https://www.mouser.com/ProductDetail/774-MP107-E) | |

### Video Card Pro

| Reference | Qty | Value | Description | LCSC | Mouser | DigiKey |
|-----------|-----|-------|-------------|------|--------|---------|
| C1, C2 | 2 | 20pF | Disc Capacitor | [C105173](https://www.lcsc.com/search?q=C105173) | | |
| C3–C10 | 8 | 100nF | Unpolarized Capacitor | [C49678](https://www.lcsc.com/search?q=C49678) | | |
| D1 | 1 | Schottky Diode | Schottky Diode DO-35 | | | |
| FB1 | 1 | BLM41PG600SN1L | Ferrite Bead 1806 | [C85844](https://www.lcsc.com/search?q=C85844) | | |
| J1 | 1 | VIDEO | RCA Connector | | | [PJRAN1X1U04X](https://www.digikey.com/en/products/detail/switchcraft-inc/PJRAN1X1U04X/969899) |
| J2 | 1 | SERIAL | Pin Header 1×4 | | | |
| J3 | 1 | INT Select | Pin Header 1×3 | | | |
| J5 | 1 | Conn_02x08 | Pin Header 2×8 | | | |
| Q1 | 1 | 2N4401 | NPN BJT TO-92 | | | |
| R1, R2 | 2 | 4.7k | Resistor | | | |
| R3 | 1 | 470 | Resistor | [C17710](https://www.lcsc.com/search?q=C17710) | | |
| R4 | 1 | 75 | Resistor | [C17820](https://www.lcsc.com/search?q=C17820) | | |
| R5 | 1 | 1k | Resistor | | | |
| U1, U4 | 2 | 74HC157 | Quad 2-to-1 Mux SOIC-16 | [C6823](https://www.lcsc.com/search?q=C6823) | | |
| U2, U5 | 2 | 74HC166 | Shift Register SOIC-16 | [C473363](https://www.lcsc.com/search?q=C473363) | | |
| U3 | 1 | ATmega1284-P | MCU DIP-40 | | | |
| U6 | 1 | 74HC138 | Decoder SOIC-16 | [C5602](https://www.lcsc.com/search?q=C5602) | | |
| VR1, VR2 | 2 | 10k | Potentiometer | | | |
| Y1 | 1 | 14.31818 MHz | Crystal | | [774-MP107-E](https://www.mouser.com/ProductDetail/774-MP107-E) | |

## Purchase

I have a few PCBs available on Tindie for those interested in building their own AC6502 system without ordering from a fab directly.

<a href="https://www.tindie.com/stores/acwrightdesign/?ref=offsite_badges&utm_source=sellers_acwrightdesign&utm_medium=badges&utm_campaign=badge_medium"><img src="https://static.tindie.com/badges/tindie-mediums.png" alt="I sell on Tindie" width="150" height="78"></a>

## License

Hardware designs are released under the [CERN Open Hardware Licence Version 2 – Permissive](https://ohwr.org/cern_ohl_p_v2.txt).  
Firmware is released under the MIT License — [KEH Controller](./Firmware/KEH%20Controller/LICENSE), [PS2 Keyboard Controller](./Firmware/PS2%20Keyboard%20Controller/LICENSE), [STP Controller](./Firmware/STP%20Controller/LICENSE).
