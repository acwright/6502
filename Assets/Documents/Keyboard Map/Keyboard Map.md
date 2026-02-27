# 6502 Keyboard Map

This document is a complete reference for the keyboard mapping system used in the 6502 project.

## Overview

The 6502 keyboard system uses an 8×8 matrix scanning architecture, utilizing two 8-bit ports (Port A and Port B) of the GPIO Card to detect key presses. 
The keyboard supports multiple modifier keys including Shift, Ctrl, Alt, and Fn, allowing for extended character sets and system control.

## Keyboard Encoder

The optional keyboard encoder which can be attached to the GPIO Card is responsible for scanning the matrix and generating the appropriate byte values based on key presses and modifier states. 
Key presses are encoded as hex values (0x00-0xFF) which correspond to ASCII characters, control codes, or special functions depending on the active modifiers and are presented on Port B of the GPIO Card.
PS/2 keyboards can be connected to the encoder for additional keys and functionality, which are also mapped into the same 0x00-0xFF range. The encoder presents PS/2 data on PortA of the GPIO Card.
Control of the keyboard encoder is done using CA1, CA2, CB1, and CB2 lines on the GPIO Card. CA1 and CA2 control the PS/2 keyboard interface, while CB1 and CB2 are used to control keyboard scanning.

## Keyboard Special Keys

- **BS** (Backspace): ASCII 0x08
- **TAB**: ASCII 0x09
- **ENTER**: ASCII 0x0D
- **ESC** (Escape): ASCII 0x1B
- **DEL** (Delete): ASCII 0x7F
- **INS** (Insert): ASCII 0x1A
- **CAPS** (Caps Lock): Software controlled toggle key on PA3/PB6
- **Arrow Keys**: UP (0x1E), DOWN (0x1F), LEFT (0x1C), RIGHT (0x1D)
- **MENU**: ASCII 0x80 (Equivalent to Windows or system key on PC keyboards)
- **Function Keys**: F1-F15 (0x81-0x8F accessible via Fn modifier or PS/2 keyboard)

## Keyboard Matrix

The physical keyboard matrix is organized as follows:

- **Rows**: PA0-PA7 (Port A, bits 0-7)
- **Columns**: PB0-PB7 (Port B, bits 0-7)

### Keyboard Matrix Layout

| Row | PB0 | PB1 | PB2 | PB3 | PB4 | PB5 | PB6 | PB7 |
|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| **PA0** | ` | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
| **PA1** | 8 | 9 | 0 | - | = | BS | ESC | TAB |
| **PA2** | Q | W | E | R | T | Y | U | I |
| **PA3** | O | P | [ | ] | \ | INS | CAPS | A |
| **PA4** | S | D | F | G | H | J | K | L |
| **PA5** | ; | ' | ENTER | DEL | SHIFT | Z | X | C |
| **PA6** | V | B | N | M | , | . | / | UP |
| **PA7** | CTRL | MENU | ALT | SPACE | FN | LEFT | DOWN | RIGHT |

## Keyboard Matrix Key Mapping Table

The following table maps all 256 possible values (0x00-0xFF) which can be produced by the keyboard encoder based on key presses and modifier states. Each row shows:
- **Hex**: The byte value
- **ASCII**: The standard ASCII character or name for that value
- **Key Name**: The physical key that directly produces this value (without modifiers)
- **Shift**: Which key combined with Shift produces this value
- **Ctrl**: Which key combined with Ctrl produces this value
- **Alt**: Which key combined with Alt produces this value
- **Fn**: Which key combined with Fn produces this value
- **Alt+Shift**: Which key combined with Alt+Shift produces this value
- **Alt+Fn**: Which key combined with Alt+Fn produces this value

Notes: 
* Some keys are only available with the PS/2 keyboard. These are indicated by parentheses "()" in the table.

For example, hex value 0x00 (NUL) is produced by pressing Ctrl+2, so the Ctrl column shows "2".

| Hex | ASCII | Key Name | Shift | Ctrl | Alt | Fn  | Alt+Shift | Alt+Fn | Matrix Position |
|-----|-------|----------|-------|------|-----|-----|-----------|--------|-----------------|
| 0x00 | NUL | | | 2 | | | | | |
| 0x01 | SOH | | | A | | | | | |
| 0x02 | STX | | | B | | | | | |
| 0x03 | ETX | | | C | | | | | |
| 0x04 | EOT | | | D | | | | | |
| 0x05 | ENQ | | | E | | | | | |
| 0x06 | ACK | | | F | | | | | |
| 0x07 | BEL | | | G | | | | | |
| 0x08 | BS | BS | | H | | | | | PA1/PB5 |
| 0x09 | TAB | TAB | | I | | | | | PA1/PB7 |
| 0x0A | LF | | | J | | | | | |
| 0x0B | VT | | | K | | | | | |
| 0x0C | FF | | | L | | | | | |
| 0x0D | CR | ENTER | | M | | | | | PA5/PB2 |
| 0x0E | SO | | | N | | | | | |
| 0x0F | SI | | | O | | | | | |
| 0x10 | DLE | | | P | | | | | |
| 0x11 | DC1 | | | Q | | | | | |
| 0x12 | DC2 | | | R | | | | | |
| 0x13 | DC3 | | | S | | | | | |
| 0x14 | DC4 | | | T | | | | | |
| 0x15 | NAK | | | U | | | | | |
| 0x16 | SYN | | | V | | | | | |
| 0x17 | ETB | | | W | | | | | |
| 0x18 | CAN | | | X | | | | | |
| 0x19 | EM | | | Y | | | | | |
| 0x1A | SUB | INS | | Z | | | | | PA3/PB5 |
| 0x1B | ESC | ESC | | [ | | | | | PA1/PB6 |
| 0x1C | FS | LEFT | | \ | | | | | PA7/PB5 |
| 0x1D | GS | RIGHT | | ] | | | | | PA7/PB7 |
| 0x1E | RS | UP | | 6 | | | | | PA6/PB7 |
| 0x1F | US | DOWN | | - | | | | | PA7/PB6 |
| 0x20 | SP | SPACE | | | | | | | PA7/PB3 |
| 0x21 | ! | | 1 | | | | | | |
| 0x22 | " | | ' | | | | | | |
| 0x23 | # | | 3 | | | | | | |
| 0x24 | $ | | 4 | | | | | | |
| 0x25 | % | | 5 | | | | | | |
| 0x26 | & | | 7 | | | | | | |
| 0x27 | ' | ' | | | | | | | PA5/PB1 |
| 0x28 | ( | | 9 | | | | | | |
| 0x29 | ) | | 0 | | | | | | |
| 0x2A | * | | 8 | | | | | | |
| 0x2B | + | | = | | | | | | |
| 0x2C | , | , | | | | | | | PA6/PB4 |
| 0x2D | - | - | | | | | | | PA1/PB3 |
| 0x2E | . | . | | | | | | | PA6/PB5 |
| 0x2F | / | / | | | | | | | PA6/PB6 |
| 0x30 | 0 | 0 | | | | | | | PA1/PB2 |
| 0x31 | 1 | 1 | | | | | | | PA0/PB1 |
| 0x32 | 2 | 2 | | | | | | | PA0/PB2 |
| 0x33 | 3 | 3 | | | | | | | PA0/PB3 |
| 0x34 | 4 | 4 | | | | | | | PA0/PB4 |
| 0x35 | 5 | 5 | | | | | | | PA0/PB5 |
| 0x36 | 6 | 6 | | | | | | | PA0/PB6 |
| 0x37 | 7 | 7 | | | | | | | PA0/PB7 |
| 0x38 | 8 | 8 | | | | | | | PA1/PB0 |
| 0x39 | 9 | 9 | | | | | | | PA1/PB1 |
| 0x3A | : | | ; | | | | | | |
| 0x3B | ; | ; | | | | | | | PA5/PB0 |
| 0x3C | < | | , | | | | | | |
| 0x3D | = | = | | | | | | | PA1/PB4 |
| 0x3E | > | | . | | | | | | |
| 0x3F | ? | | / | | | | | | |
| 0x40 | @ | | 2 | | | | | | |
| 0x41 | A | | A | | | | | | |
| 0x42 | B | | B | | | | | | |
| 0x43 | C | | C | | | | | | |
| 0x44 | D | | D | | | | | | |
| 0x45 | E | | E | | | | | | |
| 0x46 | F | | F | | | | | | |
| 0x47 | G | | G | | | | | | |
| 0x48 | H | | H | | | | | | |
| 0x49 | I | | I | | | | | | |
| 0x4A | J | | J | | | | | | |
| 0x4B | K | | K | | | | | | |
| 0x4C | L | | L | | | | | | |
| 0x4D | M | | M | | | | | | |
| 0x4E | N | | N | | | | | | |
| 0x4F | O | | O | | | | | | |
| 0x50 | P | | P | | | | | | |
| 0x51 | Q | | Q | | | | | | |
| 0x52 | R | | R | | | | | | |
| 0x53 | S | | S | | | | | | |
| 0x54 | T | | T | | | | | | |
| 0x55 | U | | U | | | | | | |
| 0x56 | V | | V | | | | | | |
| 0x57 | W | | W | | | | | | |
| 0x58 | X | | X | | | | | | |
| 0x59 | Y | | Y | | | | | | |
| 0x5A | Z | | Z | | | | | | |
| 0x5B | [ | [ | | | | | | | PA3/PB2 |
| 0x5C | \ | \ | | | | | | | PA3/PB4 |
| 0x5D | ] | ] | | | | | | | PA3/PB3 |
| 0x5E | ^ | | 6 | | | | | | |
| 0x5F | _ | | - | | | | | | |
| 0x60 | \` | \` | | | | | | | PA0/PB0 |
| 0x61 | a | A | | | | | | | PA3/PB7 |
| 0x62 | b | B | | | | | | | PA6/PB1 |
| 0x63 | c | C | | | | | | | PA5/PB7 |
| 0x64 | d | D | | | | | | | PA4/PB1 |
| 0x65 | e | E | | | | | | | PA2/PB2 |
| 0x66 | f | F | | | | | | | PA4/PB2 |
| 0x67 | g | G | | | | | | | PA4/PB3 |
| 0x68 | h | H | | | | | | | PA4/PB4 |
| 0x69 | i | I | | | | | | | PA2/PB7 |
| 0x6A | j | J | | | | | | | PA4/PB5 |
| 0x6B | k | K | | | | | | | PA4/PB6 |
| 0x6C | l | L | | | | | | | PA4/PB7 |
| 0x6D | m | M | | | | | | | PA6/PB3 |
| 0x6E | n | N | | | | | | | PA6/PB2 |
| 0x6F | o | O | | | | | | | PA3/PB0 |
| 0x70 | p | P | | | | | | | PA3/PB1 |
| 0x71 | q | Q | | | | | | | PA2/PB0 |
| 0x72 | r | R | | | | | | | PA2/PB3 |
| 0x73 | s | S | | | | | | | PA4/PB0 |
| 0x74 | t | T | | | | | | | PA2/PB4 |
| 0x75 | u | U | | | | | | | PA2/PB6 |
| 0x76 | v | V | | | | | | | PA6/PB0 |
| 0x77 | w | W | | | | | | | PA2/PB1 |
| 0x78 | x | X | | | | | | | PA5/PB6 |
| 0x79 | y | Y | | | | | | | PA2/PB5 |
| 0x7A | z | Z | | | | | | | PA5/PB5 |
| 0x7B | { | | [ | | | | | | |
| 0x7C | \ | | \ | | | | | | |
| 0x7D | } | | ] | | | | | | |
| 0x7E | ~ | | \` | | | | | | |
| 0x7F | DEL | DEL | | | | | | | PA5/PB3 |
| 0x80 | | MENU | | | | | | | PA7/PB1 |
| 0x81 | | (F1) | | | | 1 | | | |
| 0x82 | | (F2) | | | | 2 | | | |
| 0x83 | | (F3) | | | | 3 | | | |
| 0x84 | | (F4) | | | | 4 | | | |
| 0x85 | | (F5) | | | | 5 | | | |
| 0x86 | | (F6) | | | | 6 | | | |
| 0x87 | | (F7) | | | | 7 | | | |
| 0x88 | | (F8) | | | | 8 | | | |
| 0x89 | | (F9) | | | | 9 | | | |
| 0x8A | | (F10) | | | | 0 | | | |
| 0x8B | | (F11) | | | | | | k | |
| 0x8C | | (F12) | | | | | | l | |
| 0x8D | | (F13) | | | | | | m | |
| 0x8E | | (F14) | | | | | | n | |
| 0x8F | | (F15) | | | | | | o | |
| 0x90 | | | | | MENU | | | | | |
| 0x91 | | | | | (F1) | | | | 1 | |
| 0x92 | | | | | (F2) | | | | 2 | |
| 0x93 | | | | | (F3) | | | | 3 | |
| 0x94 | | | | | (F4) | | | | 4 | |
| 0x95 | | | | | (F5) | | | | 5 | |
| 0x96 | | | | | (F6) | | | | 6 | |
| 0x97 | | | | | (F7) | | | | 7 | |
| 0x98 | | | | | (F8) | | | | 8 | |
| 0x99 | | | | | (F9) | | | | 9 | |
| 0x9A | | | | | (F10) | | | | 0 | |
| 0x9B | | | | | (F11) | ESC | | | | |
| 0x9C | | | | | (F12) | LEFT | | | | |
| 0x9D | | | | | (F13) | RIGHT | | | | |
| 0x9E | | | | | (F14) | UP | | | | |
| 0x9F | | | | | (F15) | DOWN | | | | |
| 0xA0 | | | | | SPACE | | | | |
| 0xA1 | | | | | | | 1 | | |
| 0xA2 | | | | | | | ' | | |
| 0xA3 | | | | | | | 3 | | |
| 0xA4 | | | | | | | 4 | | |
| 0xA5 | | | | | | | 5 | | |
| 0xA6 | | | | | | | 7 | | |
| 0xA7 | | | | | ' | | | | |
| 0xA8 | | | | | | | 9 | | |
| 0xA9 | | | | | | | 0 | | |
| 0xAA | | | | | | | 8 | | |
| 0xAB | | | | | | | = | | |
| 0xAC | | | | | , | | | | |
| 0xAD | | | | | - | | | | |
| 0xAE | | | | | . | | | | |
| 0xAF | | | | | / | | | | |
| 0xB0 | | | | | 0 | | | | |
| 0xB1 | | | | | 1 | | | | |
| 0xB2 | | | | | 2 | | | | |
| 0xB3 | | | | | 3 | | | | |
| 0xB4 | | | | | 4 | | | | |
| 0xB5 | | | | | 5 | | | | |
| 0xB6 | | | | | 6 | | | | |
| 0xB7 | | | | | 7 | | | | |
| 0xB8 | | | | | 8 | | | | |
| 0xB9 | | | | | 9 | | | | |
| 0xBA | | | | | | | ; | | |
| 0xBB | | | | | ; | | | | |
| 0xBC | | | | | | | , | | |
| 0xBD | | | | | = | | | | |
| 0xBE | | | | | | | . | | |
| 0xBF | | | | | | | / | | |
| 0xC0 | | | | | | | 2 | | |
| 0xC1 | | | | | | | A | | |
| 0xC2 | | | | | | | B | | |
| 0xC3 | | | | | | | C | | |
| 0xC4 | | | | | | | D | | |
| 0xC5 | | | | | | | E | | |
| 0xC6 | | | | | | | F | | |
| 0xC7 | | | | | | | G | | |
| 0xC8 | | | | | | | H | | |
| 0xC9 | | | | | | | I | | |
| 0xCA | | | | | | | J | | |
| 0xCB | | | | | | | K | | |
| 0xCC | | | | | | | L | | |
| 0xCD | | | | | | | M | | |
| 0xCE | | | | | | | N | | |
| 0xCF | | | | | | | O | | |
| 0xD0 | | | | | | | P | | |
| 0xD1 | | | | | | | Q | | |
| 0xD2 | | | | | | | R | | |
| 0xD3 | | | | | | | S | | |
| 0xD4 | | | | | | | T | | |
| 0xD5 | | | | | | | U | | |
| 0xD6 | | | | | | | V | | |
| 0xD7 | | | | | | | W | | |
| 0xD8 | | | | | | | X | | |
| 0xD9 | | | | | | | Y | | |
| 0xDA | | | | | | | Z | | |
| 0xDB | | | | | [ | | | | |
| 0xDC | | | | | \ | | | | |
| 0xDD | | | | | ] | | | | |
| 0xDE | | | | | | | 6 | | |
| 0xDF | | | | | | | - | | |
| 0xE0 | | | | | \` | | | | |
| 0xE1 | | | | | A | | | | |
| 0xE2 | | | | | B | | | | |
| 0xE3 | | | | | C | | | | |
| 0xE4 | | | | | D | | | | |
| 0xE5 | | | | | E | | | | |
| 0xE6 | | | | | F | | | | |
| 0xE7 | | | | | G | | | | |
| 0xE8 | | | | | H | | | | |
| 0xE9 | | | | | I | | | | |
| 0xEA | | | | | J | | | | |
| 0xEB | | | | | K | | | | |
| 0xEC | | | | | L | | | | |
| 0xED | | | | | M | | | | |
| 0xEE | | | | | N | | | | |
| 0xEF | | | | | O | | | | |
| 0xF0 | | | | | P | | | | |
| 0xF1 | | | | | Q | | | | |
| 0xF2 | | | | | R | | | | |
| 0xF3 | | | | | S | | | | |
| 0xF4 | | | | | T | | | | |
| 0xF5 | | | | | U | | | | |
| 0xF6 | | | | | V | | | | |
| 0xF7 | | | | | W | | | | |
| 0xF8 | | | | | X | | | | |
| 0xF9 | | | | | Y | | | | |
| 0xFA | | | | | Z | | | | |
| 0xFB | | | | | | | [ | | |
| 0xFC | | | | | | | \ | | |
| 0xFD | | | | | | | ] | | |
| 0xFE | | | | | | | \` | | |
| 0xFF | | | | | DEL | | | | |


