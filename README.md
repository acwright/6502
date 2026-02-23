6502
====

## A 65(c)02 Based 8-Bit Retro Computer Project

![The 6502 Project](./Assets/Images/6502.png?raw=true)
*Shown above are some of the systems that can be assembled from the 6502 project*

## Overview

This project aims to create multiple retro-style 8-bit computers based on the 65(c)02 microprocessor. 
The project is centered around a number of custom-designed PCBs that can be combined in various configurations to create different computer systems based on a common [memory map](./Assets/Documents/Memory%20Map/Memory%20Map.pdf). 
The designs are inspired by classic 8-bit computers such as the KIM-1, Commodore 64, and Apple II, but with modern enhancements and flexibility.

The project is designed using KiCad for PCB design and is open-source, allowing enthusiasts to contribute and modify the designs. The boards are manufactured 
by JLCPCB and use a combination of surface-mount and through-hole components.

## Computer Systems

The project supports the creation of several different computer systems, including but not limited to:

**The "COB" (COMPUTER ON-A BACKPLANE)**: A compact 8-bit computer on a backplane:

![The COB](./CAD/Bases/The%20COB/The%20COB.png?raw=true)
![The COB](./Assets/Images/COB.png?raw=true)

  * 1x Backplane
  * 1x Backplane Pro
  * 1x Backplane Helper
  * 1x CPU Card (or 1x CPU Card Pro)
  * 1x Memory Card
  * 1x Video Card (or 1x Video Card Pro or 1x VGA Card)
  * 1x Sound Card
  * 1x Storage Card (or 1x Storage Card Pro)
  * 1x Serial Card (or 1x Serial Card Pro)
  * 1x RAM Card
  * 1x RTC Card
  * 1x GPIO Card
    * 1x Keyboard Encoder Helper (or 1x PS2 Helper)

---

**The "DEV" (DEVELOPMENT ENVIRONMENT VEHICLE)**: A development system featuring an emulated 65(c)02 CPU:

![The DEV](./CAD/Bases/The%20DEV/The%20DEV.png?raw=true)
![The DEV](./Assets/Images/DEV.png?raw=true)

  * 1x Dev Board
  * 1x Dev Output Board (or 1x LCD Board or 1x Output Board)

---

**The "KIM" (KEYPAD INPUT MONITOR)**: A modern take on the classic KIM-1 computer:

![The KIM](./CAD/Bases/The%20KIM/The%20KIM.png?raw=true)
![The KIM](./Assets/Images/KIM.png?raw=true)

  * 1x Main Board
  * 1x Backplane Helper
  * 1x GPIO Card
    * 1x Keypad Helper
    * 1x Keypad LCD Helper
  * 1x Serial Card (or 1x Serial Card Pro)

---

**The "VCS" (VIDEO COMPUTER SYSTEM)**: A retro-style gaming console:

![The VCS](./CAD/Bases/The%20VCS/The%20VCS.png?raw=true)
![The VCS](./Assets/Images/VCS.png?raw=true)

  * 1x Main Board
  * 1x Input Board
  * 1x Output Board
  * 1x ROM Cart

## CAD

The [CAD](./CAD) folder contains CAD files and mechanical designs for the project including 3D models and enclosure designs. 
See the [README](./CAD/README.md) file in the CAD folder for more details.

## Firmware

The [Firmware](./Firmware) folder contains the source code for the firmware running on the various microcontrollers used in the project, such as the Teensy 4.1, and the BIOS code for the various systems. 
The firmware is written in C/C++ or Assembly and is designed to interface with the 65(c)02 and other hardware components. 
See the [README](./Firmware/README.md) file in the Firmware folder for more details.

## Hardware

The [Hardware](./Hardware) folder contains KICAD files for the system's custom-designed PCBs. These are divided into the following categories:

- **Boards**: The main board, development board, backplane boards and other peripheral boards.
- **Cards**: Various peripheral cards for plugging into the backplane or card slot(s).
- **Helpers**: Additional boards that provide specific functionalities or interfaces.
- **Carts**: Cartridge boards for ROM images.

**Boards**:

- **Backplane Board**: A backplane board that provides multiple card slots for peripheral cards and connects to the main board via a ribbon cable.
- **Backplane Board Pro**: An enhanced version of the backplane board with addition of clock, reset and power circuitry.
- **Dev Board**: The development board emulates the 65(c)02 using a Teensy 4.1 running custom firmware and using [vrEmu6502](https://github.com/visrealm/vrEmu6502).
- **Dev Output Board**: A compliment to the Dev Board that provides [VT-AC](https://github.com/acwright/VT-AC) compatible ASCII terminal display output using a 2.4in LCD.
- **Input Board**: A board that provides PS/2 keyboard and mouse input along with SNES controller support.
- **LCD Board**: A board that provides a 320x240 LCD display via a 6522 VIA interface. **UNTESTED**
- **Main Board**: The main board that contains the 65(c)02 CPU, 32K RAM, 32K ROM along with bus connection, clock and reset circuitry.
- **Output Board**: A board that provides VGA output using the [Pico9918](https://github.com/visrealm/pico9918) and sound output using the [ARMSID](https://retrocomp.cz/produkt?id=2). **UNTESTED**

**Cards**:

- **Blinkenlights Card**: A card that provides a set of LEDs for visual output. 
- **CPU Card**: A card that can host a 65(c)02 CPU. 
- **CPU Card Pro**: A card that can host a 65816 CPU. **UNTESTED**
- **GPIO Card**: A card that provides general-purpose input/output (GPIO) pins using a 6522 VIA for interfacing with external devices.
- **LCD Card**: A card that provides output to a 16x2 LCD display using a 6522 VIA.
- **Memory Card**: A card that provides 32K RAM and 32K ROM memory along with address decoding. 
- **Prototype Card**: A card that provides a prototyping area for custom circuits. 
- **RAM Card**: A card that provides 512k of extra banked RAM.
- **RTC Card**: A card that provides real-time clock functionality using the Dallas DS1511Y. 
- **Serial Card**: A card that provides serial communication capabilities using the 6551 ACIA. 
- **Serial Card Pro**: An enhanced version of the Serial Card with additional features. 
- **Sound Card**: A card that provides audio output capabilities using the [ARMSID](https://retrocomp.cz/produkt?id=2).
- **Storage Card**: A card that provides storage capabilities using a CompactFlash card.
- **Storage Card Pro**: A card that provides storage capabilities with an SD Card, 16MB Flash and SPI. **UNTESTED**
- **VGA Card**: A card that provides VGA output capabilities using the [Pico9918](https://github.com/visrealm/pico9918).
- **VGA Card Pro**: A custom programmable VGA Card using the Pi Pico. **UNTESTED**
- **Video Card**: A card that provides composite video output capabilities using the TMS9918A.
- **Video Card Pro**: A card that provides color composite video output capabilities using the Atmega1284p. **UNTESTED**

**Helpers**:

- **Backplane Helper**: A backplane board that adds two additional slots for cards.
- **Breadboard Helper**: A board that allows easy bus connection to a breadboard for prototyping.
- **Clock Helper**: A board that allows for running and stopping the clock along with single cycle or instruction stepping.
- **DB25 Helper**: A board that allows easy connection of GPIO ports to a DB25 connector.
- **GPIO Helper**: A board that provides eight buttons and eight LEDs for user input and output.
- **GPIO Breadboard Helper**: A board that allows easy connection of GPIO ports to a breadboard for prototyping.
- **Joystick Helper**: A board that provides an Atari 2600 style joystick interface for user input.
- **Keyboard Helper**: A board that provides a 64 key matrix keyboard interface and dual joystick ports for user input.
- **Keyboard Encoder Helper**: A board that translates keyboard scan data and PS/2 keyboard data to ASCII using the Atmega1284p.
- **Keypad Helper**: A board that provides a 4x4 and 2x4 matrix keypad for user input. **UNTESTED**
- **Keypad LCD Helper**: A board that contains an LCD display and is designed to work with the Keypad Helper. **UNTESTED**
- **Mega Helper**: A board that interfaces the Arduino Mega 2560 R3 with 6502 bus.
- **PS2 Helper**: A board that allows connection of a PS/2 keyboard using the Atmega328p.

**Carts**:

- **ROM Cart**: A cartridge that provides a ROM image for the system.

## Production

The [Production](./Production) folder contains the JLCPCB ready production files for the various PCBs, including Gerber files, BOMs, and assembly instructions. 
See the [README](./Production/README.md) file in the Production folder for more details.

## Schematics

The [Schematics](./Schematics) folder contains the schematic diagrams for all the PCBs in the project. 
These schematics are created using KiCad and provide detailed information about the components and connections used in each board. 
See the [README](./Schematics/README.md) file in the Schematics folder for more details.

## Licensing

This project is licensed under the CERN Open Hardware Licence Version 2 - Permissive License. See the [LICENSE](./LICENSE) file for more details.
