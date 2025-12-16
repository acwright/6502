Wozmon
======

A basic monitor program for the Apple I computer, written in 6502 assembly language. 
Wozmon allows users to inspect and modify memory, execute programs, and perform other low-level tasks on the Apple I. 
This version is a modern reimplementation of the original Wozmon, designed to run on the 6502 project computer using the Serial Card in IO slot 5 (`$9000`) for input and output.

Install:

    brew install cc65

Build:

    make

View:

    make view

Burn EEPROM:

    make eeprom
