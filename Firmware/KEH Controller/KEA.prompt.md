# Keyboard Encoder Attachment (KEA) Behavior Specification

This document defines the complete behavior of the Keyboard Encoder used in the 6502 computer system. The encoder receives USB HID keycodes and outputs a single ASCII byte per keypress. Use this as the reference when implementing the encoder in any context (emulator, firmware, etc.).

## Overview

The keyboard encoder translates USB HID key events into ASCII bytes. It only produces output on key **press** events — key releases are ignored (except for tracking modifier state). Letters are **always uppercase**. The only recognized modifiers are **Shift** and **Ctrl**.

All other modifier keys (Alt, Caps Lock, GUI/Menu, Fn) are ignored and produce no output.

## Hardware Interface

The encoder connects to a 65C22 VIA. It can output on Port A, Port B, or both.

- **Port A** (PS/2-style encoder): enabled when **CA2 is LOW**, signals data ready via **CA1** (falling edge IRQ)
- **Port B** (matrix-style encoder): enabled when **CB2 is LOW**, signals data ready via **CB1** (falling edge IRQ)

When the VIA reads the port register (ORA or ORB), interrupts and data-ready flags are cleared.

When a port is disabled (Cx2 HIGH), the encoder still latches data internally but does not assert interrupts. If the port is re-enabled, the latched data becomes readable.

When no data is ready or the port is disabled, reads return `0xFF`.

## Modifier Keys

Only two modifiers are tracked:

| Modifier | USB HID Keycodes | Behavior |
|----------|-----------------|----------|
| Shift | `0xE1` (Left), `0xE5` (Right) | Track press/release state. Affects number and symbol keys only (not letters). |
| Ctrl | `0xE0` (Left), `0xE4` (Right) | Track press/release state. Produces control codes with letters and select special keys. |

Modifier keys **never** generate output by themselves.

### Ignored Keys

These keys are silently ignored (no state change, no output):

- `0xE2`, `0xE6` — Left/Right Alt
- `0xE3`, `0xE7` — Left/Right GUI (Menu)
- `0x39` — Caps Lock
- `0x3A`–`0x45` — F1–F12
- `0x68`–`0x6A` — F13–F15
- Any USB HID keycode not listed in the mapping tables below

## Key Mapping

### Base Mapping: USB HID Keycode → ASCII

Letters always produce uppercase ASCII regardless of Shift state:

| HID | ASCII | Char | | HID | ASCII | Char |
|------|-------|------|-|------|-------|------|
| `0x04` | `0x41` | A | | `0x05` | `0x42` | B |
| `0x06` | `0x43` | C | | `0x07` | `0x44` | D |
| `0x08` | `0x45` | E | | `0x09` | `0x46` | F |
| `0x0A` | `0x47` | G | | `0x0B` | `0x48` | H |
| `0x0C` | `0x49` | I | | `0x0D` | `0x4A` | J |
| `0x0E` | `0x4B` | K | | `0x0F` | `0x4C` | L |
| `0x10` | `0x4D` | M | | `0x11` | `0x4E` | N |
| `0x12` | `0x4F` | O | | `0x13` | `0x50` | P |
| `0x14` | `0x51` | Q | | `0x15` | `0x52` | R |
| `0x16` | `0x53` | S | | `0x17` | `0x54` | T |
| `0x18` | `0x55` | U | | `0x19` | `0x56` | V |
| `0x1A` | `0x57` | W | | `0x1B` | `0x58` | X |
| `0x1C` | `0x59` | Y | | `0x1D` | `0x5A` | Z |

Numbers:

| HID | ASCII | Char | | HID | ASCII | Char |
|------|-------|------|-|------|-------|------|
| `0x1E` | `0x31` | 1 | | `0x1F` | `0x32` | 2 |
| `0x20` | `0x33` | 3 | | `0x21` | `0x34` | 4 |
| `0x22` | `0x35` | 5 | | `0x23` | `0x36` | 6 |
| `0x24` | `0x37` | 7 | | `0x25` | `0x38` | 8 |
| `0x26` | `0x39` | 9 | | `0x27` | `0x30` | 0 |

Symbols:

| HID | ASCII | Char |
|------|-------|------|
| `0x2D` | `0x2D` | - |
| `0x2E` | `0x3D` | = |
| `0x2F` | `0x5B` | [ |
| `0x30` | `0x5D` | ] |
| `0x31` | `0x5C` | \ |
| `0x33` | `0x3B` | ; |
| `0x34` | `0x27` | ' |
| `0x35` | `0x60` | ` |
| `0x36` | `0x2C` | , |
| `0x37` | `0x2E` | . |
| `0x38` | `0x2F` | / |

Control characters and navigation:

| HID | ASCII | Function |
|------|-------|----------|
| `0x28` | `0x0D` | Enter (CR) |
| `0x29` | `0x1B` | Escape (ESC) |
| `0x2A` | `0x08` | Backspace (BS) |
| `0x2B` | `0x09` | Tab (HT) |
| `0x2C` | `0x20` | Space (SP) |
| `0x4C` | `0x7F` | Delete (DEL) |
| `0x49` | `0x1A` | Insert (SUB) |
| `0x4F` | `0x1D` | Right Arrow |
| `0x50` | `0x1C` | Left Arrow |
| `0x51` | `0x1F` | Down Arrow |
| `0x52` | `0x1E` | Up Arrow |

### Shift + Key (symbols only)

Shift has **no effect on letters** (they are already uppercase). Shift only modifies numbers and symbol keys. Ctrl takes priority over Shift (if Ctrl is held, Shift is ignored).

| Base | Shifted | | Base | Shifted |
|------|---------|-|------|---------|
| `1` → `!` (`0x21`) | `2` → `@` (`0x40`) | | `3` → `#` (`0x23`) | `4` → `$` (`0x24`) |
| `5` → `%` (`0x25`) | `6` → `^` (`0x5E`) | | `7` → `&` (`0x26`) | `8` → `*` (`0x2A`) |
| `9` → `(` (`0x28`) | `0` → `)` (`0x29`) | | `-` → `_` (`0x5F`) | `=` → `+` (`0x2B`) |
| `[` → `{` (`0x7B`) | `]` → `}` (`0x7D`) | | `\` → `\|` (`0x7C`) | `;` → `:` (`0x3A`) |
| `'` → `"` (`0x22`) | `,` → `<` (`0x3C`) | | `.` → `>` (`0x3E`) | `/` → `?` (`0x3F`) |
| `` ` `` → `~` (`0x7E`) | | | | |

### Ctrl + Key

Ctrl takes priority over all other modifiers. Only letters and a few special keys produce Ctrl codes.

**Ctrl + Letter → Control codes `0x01`–`0x1A`:**

| Combo | Output | | Combo | Output |
|-------|--------|-|-------|--------|
| Ctrl+A | `0x01` (SOH) | | Ctrl+B | `0x02` (STX) |
| Ctrl+C | `0x03` (ETX) ★ | | Ctrl+D | `0x04` (EOT) |
| Ctrl+E | `0x05` (ENQ) | | Ctrl+F | `0x06` (ACK) |
| Ctrl+G | `0x07` (BEL) | | Ctrl+H | `0x08` (BS) |
| Ctrl+I | `0x09` (HT) | | Ctrl+J | `0x0A` (LF) |
| Ctrl+K | `0x0B` (VT) | | Ctrl+L | `0x0C` (FF) |
| Ctrl+M | `0x0D` (CR) | | Ctrl+N | `0x0E` (SO) |
| Ctrl+O | `0x0F` (SI) | | Ctrl+P | `0x10` (DLE) |
| Ctrl+Q | `0x11` (DC1) | | Ctrl+R | `0x12` (DC2) |
| Ctrl+S | `0x13` (DC3) | | Ctrl+T | `0x14` (DC4) |
| Ctrl+U | `0x15` (NAK) | | Ctrl+V | `0x16` (SYN) |
| Ctrl+W | `0x17` (ETB) | | Ctrl+X | `0x18` (CAN) |
| Ctrl+Y | `0x19` (EM) | | Ctrl+Z | `0x1A` (SUB) |

★ Ctrl+C (`0x03`) is used by BASIC as the break/interrupt signal.

**Ctrl + Special keys:**

| Combo | Output | Control Code |
|-------|--------|--------------|
| Ctrl+2 | `0x00` | NUL |
| Ctrl+6 | `0x1E` | RS |
| Ctrl+- | `0x1F` | US |
| Ctrl+[ | `0x1B` | ESC |
| Ctrl+\ | `0x1C` | FS |
| Ctrl+] | `0x1D` | GS |

## Processing Priority

When mapping a keypress, apply modifiers in this order:

1. **Ctrl** — If Ctrl is held, produce the Ctrl code. Shift is ignored.
2. **Shift** — If Shift is held (and Ctrl is not), produce shifted symbol. Letters are unaffected.
3. **Base** — No modifier: produce the base ASCII character from the lookup table.

## Keypress-Only Output

- Only **key press** events produce output. Key **release** events are used solely to track Shift/Ctrl modifier state.
- Each keypress overwrites the previous data on the output port(s). There is no input buffer — only the most recent keypress is available.
- A `0x00` output is valid only when Ctrl+2 is pressed. All other unmapped keys produce no output (no data ready, no interrupt).

## Reset Behavior

On reset, all internal state is cleared:
- ASCII data registers set to `0x00`
- Data ready flags cleared
- Interrupt pending flags cleared
- Port enable flags cleared
- Shift and Ctrl modifier states cleared

## Keyboard Layout Reference

```
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬───────────┐ ┌─────┐
│~ `  │! 1  │@ 2  │# 3  │$ 4  │% 5  │^ 6  │& 7  │* 8  │( 9  │) 0  │_ -  │+ =  │ Backspace │ │ Esc │
├─────┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬────────┤ ├─────┤
│ Tab    │  Q  │  W  │  E  │  R  │  T  │  Y  │  U  │  I  │  O  │  P  │{ [  │} ]  │|  \    │ │ Ins │
├────────┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴────────┤ ├─────┤
│ Caps Lock │  A  │  S  │  D  │  F  │  G  │  H  │  J  │  K  │  L  │: ;  │" '  │   Enter   │ │ Del │
├───────────┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴──┬──┴───────────┤ ├─────┤
│    Shift     │  Z  │  X  │  C  │  V  │  B  │  N  │  M  │< ,  │> .  │? /  │    Shift     │ │  ↑  │
├──────┬───────┼─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┼─────┼─────┼──┬──┴──┬──┬──┤
│ Ctrl │       │                  Space                         │     │Ctrl │ │  ←  │↓ │→ │
└──────┘       └────────────────────────────────────────────────┘     └─────┘ └─────┴──┴──┘
```

**Active keys:** All labeled keys above produce output except Caps Lock (ignored).
**Unlabeled keys** in the bottom-left (Menu, Alt) and bottom-right (Alt, Fn) positions are ignored by the encoder.
