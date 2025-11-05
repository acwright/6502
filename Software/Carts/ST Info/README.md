ST Info
=======

Reads "Identify Drive" info from Compact Flash card and stores to RAM at address `0x0400` then reads sector 0 and stores
data at RAM location `0x0500`.

Use snapshot tool on Dev Board to save RAM to SD card and verify data.

References:

- [https://www.waveguide.se/?article=8-bit-compact-flash-interface](https://www.waveguide.se/?article=8-bit-compact-flash-interface)
- [https://blog.retroleum.co.uk/electronics-articles/an-8-bit-ide-interface/](https://blog.retroleum.co.uk/electronics-articles/an-8-bit-ide-interface/)
- [https://planck6502.com/news/2023/03/27/compactflash-6502/](https://planck6502.com/news/2023/03/27/compactflash-6502/)

Install:

    brew install cc65

Build:

    make

View:

    make view

Burn EEPROM:

    make eeprom
