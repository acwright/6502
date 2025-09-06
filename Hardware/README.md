6502 Hardware
=============

This folder contains KiCad hardware designs for the 6502 project.

Changelog
--------- 

- **Rev 1.0**
  - Initial release
- **Rev 1.1**
  - Dev Board
    - Removed OE control signals from all TXS0108's (All level shifters on by default)
    - Added reset switch
    - Added pin header for switches
    - Added pin header for Teensy serial connection
    - Relocated connections on Teensy to allow for RX/TX serial header connection 
  - LCD Card
    - Change JP1 from bridged to open
  - Sound Card
    - Change JP1 from bridged to open
  - Video Card
    - Change JP7 from open to bridged
