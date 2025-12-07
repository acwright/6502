Hardware
========

The hardware consists of several custom-designed PCBs. These are divided into the following categories:

- **Boards**: The main board, development board, backplane boards and other peripheral boards.
- **Cards**: Various peripheral cards for plugging into the backplane or main board card slot(s).
- **Helpers**: Additional boards that provide specific functionalities or interfaces.

**Boards**:

- **Backplane Board**: A backplane board that provides multiple card slots for peripheral cards and connects to the main board via a ribbon cable.
- **Backplane Board Pro**: An enhanced version of the backplane board with additional features.
- **Dev Board**: The development board is an alternative to the main board which acts as a 65(c)02 emulator using a Teensy 4.1.
- **Dev Output Board**: A compliment to the Dev Board that provides output using an Adafruit I2S Stereo Decoder and a Waveshare 2.4in LCD.
- **Front Panel Board**: A board that provides a front panel interface with switches and LEDs for controlling and monitoring the system.
- **Input Board**: A board that provides PS/2 keyboard and mouse interfaces along with SNES controller support.
- **Main Board**: The main board that contains the 65(c)02 CPU, memory, memory decoding ICs, one peripheral card slot, and a bus connector.

**Cards**:

- **Blinkenlights Card**: A card that provides a set of LEDs for visual output. 
- **CPU Card**: A card that can host a 65(c)02 CPU and associated components. 
- **CPU Card Pro**: A card that can host a 65816 CPU and associated components. 
- **GPIO Card**: A card that provides general-purpose input/output (GPIO) pins for interfacing with external devices.
- **LCD Card**: A card that provides output to an LCD display.
- **Memory Card**: A card that provides RAM or ROM memory. 
- **Prototype Card**: A card that provides a prototyping area for custom circuits. 
- **RAM Card**: A card that provides 512k of extra RAM. 
- **RTC Card**: A card that provides real-time clock functionality. 
- **Serial Card**: A card that provides serial communication capabilities. 
- **Serial Card Pro**: An enhanced version of the Serial Card with additional features. 
- **Sound Card**: A card that provides audio output capabilities.
- **Storage Card**: A card that provides storage capabilities using a CompactFlash card.
- **VGA Card**: A card that provides VGA output capabilities.
- **VGA Card Pro**: An enhanced version of the VGA Card with additional features.
- **Video Card**: A card that provides composite video output capabilities using the TMS9918A.
- **Video Card Pro**: A card that provides composite video output capabilities using the Atmega1284p.

**Helpers**:

- **Backplane Helper**: A backplane board that adds two additional slots for cards.
- **Breadboard Helper**: A board that allows easy connection to a breadboard for prototyping.
- **Clock Helper**: A board that allows for running and stopping the clock along with single cycle or instruction stepping.
- **DB25 Helper**: A board that allows easy connection of GPIO ports to a DB25 connector.
- **GPIO Helper**: A board that provides eight buttons and eight LEDs for user input and output.
- **GPIO Breadboard Helper**: A board that allows easy connection of GPIO ports to a breadboard for prototyping.
- **Joystick Helper**: A board that provides an Atari 2600 style joystick interface for user input.
- **Keyboard Helper**: A board that provides a 64 key matrix keyboard interface and dual joystick ports for user input.
- **Keyboard Encoder Helper**: A board that translates keyboard scan data and PS/2 keyboard data to ASCII.
- **Keypad Helper**: A board that provides a 4x4 and 2x4 matrix keypad for user input.
- **Keypad LCD Helper**: A board that contains an LCD display and is designed to work with the Keypad Helper.
- **Output Helper**: A recreation of Grant Searle's Video interface for outputting video signals.
- **PS2 Helper**: A board that allows connection of a PS/2 keyboard.
- **VERA Helper**: A board that hosts a VERA module with graphics, sound and SD card.

**Carts**:

- **ROM Cart**: A cartridge that provides a ROM image for the system.