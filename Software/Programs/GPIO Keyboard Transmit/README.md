GPIO Keyboard Transmit
======================

Transmits X-Y matrix data (in hex) from Keyboard Helper or PS2 Helper to Serial Card.

Notes: 

This is a very basic keyboard scan routine only useful for confirming Keyboard Helper or PS2 Helper is working correctly. 
A more robust scan routine would be needed for actual keyboard input.

Install:

    brew install cc65

Build:

    make

View:

    make view

Burn EEPROM:

    make eeprom
