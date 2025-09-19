6502 Project
=============

**A 6502 Based 8-Bit Retro Computer Project**

![6502](./Documentation/public/6502.svg)
![Main Board](./Documentation/public/main-board.png)

## Overview

The project aims to create a retro-style 8-bit computer based on the 65(c)02 microprocessor. It includes custom-designed main board, development board, 
backplane boards, various peripheral cards, various helper boards, and a simple operating system.

The project is designed using KiCad for PCB design and is open-source, allowing enthusiasts to contribute and modify the designs. The boards are manufactured 
by JLCPCB and use a combination of surface-mount and through-hole components.

## Hardware

The [Hardware](./Hardware) folder contains several custom-designed PCBs. These are divided into the following categories:

- **Boards**: The main board, development board, backplane boards and other peripheral boards.
- **Cards**: Various peripheral cards for plugging into the backplane or main board card slot(s).
- **Helpers**: Additional boards that provide specific functionalities or interfaces.

**Boards**:

- **Main Board**: The main board that contains the 65(c)02 CPU, memory, memory decoding ICs, one peripheral card slot, and a bus connector.
- **Dev Board**: The development board is an alternative to the main board which can either act as a 65(c)02 emulator or monitor using a Teensy 4.1.
- **Backplane Board**: A backplane board that provides multiple card slots for peripheral cards and connects to the main board via a ribbon cable.
- **Backplane Board Pro**: An enhanced version of the backplane board with additional features.
- **Input Board**: A board that provides PS/2 keyboard and mouse interfaces along with SNES controller support.
- **Front Panel Board**: A board that provides a front panel interface with switches and LEDs for controlling and monitoring the system.

**Cards**:

- **Blinkenlights Card**: A card that provides a set of LEDs for visual output. 
- **CPU Card**: A card that can host a 65(c)02 CPU and associated components. 
- **GPIO Card**: A card that provides general-purpose input/output (GPIO) pins for interfacing with external devices.
- **LCD Card**: A card that provides output to an LCD display.
- **Memory Card**: A card that provides RAM or ROM memory. 
- **Prototype Card**: A card that provides a prototyping area for custom circuits. 
- **Serial Card**: A card that provides serial communication capabilities. 
- **Serial Card Pro**: An enhanced version of the Serial Card with additional features. 
- **Sound Card**: A card that provides audio output capabilities.
- **Storage Card**: A card that provides storage capabilities using a CompactFlash card.
- **Teensy Card**: A card that hosts a Teensy 4.1 microcontroller for various capabilities.
- **VGA Card**: A card that provides VGA output capabilities.
- **Video Card**: A card that provides composite video output capabilities using the TMS9918A.

**Helpers**:

- **Breadboard Helper**: A board that allows easy connection to a breadboard for prototyping.
- **GPIO Helper**: A board that provides eight buttons and eight LEDs for user input and output.
- **Keypad Helper**: A board that provides a 4x4 and 2x4 matrix keypad for user input.
- **Keypad LCD Helper**: A board that contains an LCD display and is designed to work with the Keypad Helper.
- **Joystick Helper**: A board that provides an Atari 2600 style joystick interface for user input.

**Carts**:

- **ROM Cart**: A cartridge that provides a ROM image for the system.

## Schematics

The [Schematics](./Schematics) folder contains the schematic diagrams for all the PCBs in the project. These schematics are created using KiCad and provide detailed information about the components and connections used in each board. See the README file in the Schematics folder for more details.

## Firmware

The [Firmware](./Firmware) folder contains the source code for the firmware running on the various microcontrollers used in the project, such as the Teensy 4.1 and Raspberry Pi Zero. The firmware is written in C/C++ and is designed to interface with the 65(c)02 and other hardware components. See the README file in the Firmware folder for more details.

## Software

The [Software](./Software) folder contains various example projects written in assembly language for the 65(c)02. These examples demonstrate how to interface with the various hardware components and peripherals. See the README file in the Software folder for more details.

## Production

The [Production](./Production) folder contains the JLCPCB ready production files for the various PCBs, including Gerber files, BOMs, and assembly instructions. See the README file in the Production folder for more details.

## Documentation

The main documentation for this project can be found at: [https://6502.acwrightdesign.com](https://6502.acwrightdesign.com)

**Running the Documentation Locally:**

The documentation for this project uses [VitePress](https://vitepress.dev/) and is deployed to Github Pages. To build the documentation locally, follow these steps:

1. Ensure you have Node.js and npm installed on your system.
2. Install the required dependencies by running:

```bash
npm install
```
3. Start the VitePress development server by running:

```bash
npm run docs:dev
```
4. Open your web browser and navigate to [http://localhost:5147](http://localhost:5147) to view the documentation.