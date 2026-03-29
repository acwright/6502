# C++ Firmware Emulator Refactor Plan

**Project**: `/Users/acwright/Developer/6502/Firmware/DB Emulator/`
**Goal**: Align the C++ firmware 6502 emulator with the TypeScript emulator for behavioral consistency and accuracy. Where differences exist, favor the implementation that is more faithful to the real hardware specifications.

---

## 1. ROM: Change Fill Value from 0xEA to 0x00

**File**: `lib/6502/ROM/ROM.cpp`
**Priority**: LOW — Cosmetic/consistency alignment.

**Current behavior**: ROM is initialized with `0xEA` (NOP opcode).

**Required behavior**: ROM should be initialized with `0x00` to match the TypeScript emulator. A blank ROM should not appear to contain valid instructions.

### Change:

```cpp
// Before:
for (uint16_t i = 0x0000; i < size; i++) {
    this->data[i] = 0xEA; // Load with NOPs
}

// After:
for (uint16_t i = 0x0000; i < size; i++) {
    this->data[i] = 0x00;
}
```

---

## 2. Cart: Change Fill Value from 0xEA to 0x00

**File**: `lib/6502/Cart/Cart.cpp`
**Priority**: LOW — Cosmetic/consistency alignment.

**Current behavior**: Cart is initialized with `0xEA` (NOP opcode).

**Required behavior**: Cart should be initialized with `0x00` to match ROM and the TypeScript emulator.

### Change:

```cpp
// Before:
for (uint16_t i = 0x0000; i < size; i++) {
    this->data[i] = 0xEA; // Load with NOPs
}

// After:
for (uint16_t i = 0x0000; i < size; i++) {
    this->data[i] = 0x00;
}
```

---

## 3. ROM: Make Read-Only (Remove Write Capability)

**File**: `lib/6502/ROM/ROM.cpp`, `lib/6502/ROM/ROM.h`
**Priority**: MEDIUM — Correctness. ROM should not be writable by the CPU.

**Current behavior**: `ROM::write()` stores values into the ROM data array. This means the CPU can write to ROM addresses, which is unrealistic.

**Required behavior**: `ROM::write()` should be a no-op. ROM data should only be loadable via an explicit programming/load method (for loading ROM images from SD card, etc.), not through the bus write path.

### Changes:

**ROM.cpp** — Make `write()` a no-op:
```cpp
// Before:
void ROM::write(uint16_t index, uint8_t value) {
    if (index <= (ROM_END - ROM_START)) {
        this->data[index] = value;
    }
}

// After:
void ROM::write(uint16_t index, uint8_t value) {
    // ROM is read-only from the CPU bus perspective
}
```

**ROM.h** — Add a `load()` method for programming ROM contents (separate from bus writes):
```cpp
uint8_t read(uint16_t index);
void write(uint16_t index, uint8_t value);  // No-op (ROM is read-only on bus)
void load(uint16_t index, uint8_t value);   // For loading ROM images
```

**ROM.cpp** — Add the load method:
```cpp
void ROM::load(uint16_t index, uint8_t value) {
    if (index <= (ROM_END - ROM_START)) {
        this->data[index] = value;
    }
}
```

**Note**: The Cart class already follows this pattern — `Cart::write()` is a no-op and `Cart::load()` exists for programming. ROM should match.

**main.cpp** — Update any code that writes to ROM via `rom.write()` to use `rom.load()` instead. Search for all call sites. Currently, ROM loading from SD card likely calls `rom.data[i] = value` directly (since `rom.data` is public), but verify and update if `rom.write()` is used anywhere for loading purposes.

---

## 4. GPIO: Add Second Joystick Attachment (Port A)

**File**: `src/main.cpp`
**Priority**: LOW — Feature alignment with TypeScript emulator.

**Current behavior**: Only one joystick attachment exists, attached to Port B.

```cpp
GPIOJoystickAttachment joystickAttachment = GPIOJoystickAttachment(false, 100); // Port B
gpioCard.attachToPortB(&joystickAttachment);
```

**Required behavior**: Two joystick attachments — one for Port A and one for Port B — matching the TypeScript emulator.

### Changes:

**main.cpp** — Declare a second joystick attachment:
```cpp
// Before:
GPIOJoystickAttachment joystickAttachment = GPIOJoystickAttachment(false, 100); // Port B, Priority 100

// After:
GPIOJoystickAttachment joystickAttachmentA = GPIOJoystickAttachment(true, 100);   // Port A, Priority 100
GPIOJoystickAttachment joystickAttachmentB = GPIOJoystickAttachment(false, 100);  // Port B, Priority 100
```

**main.cpp** — Attach both joysticks in `setup()`:
```cpp
// Before:
gpioCard.attachToPortB(&joystickAttachment);

// After:
gpioCard.attachToPortA(&joystickAttachmentA);
gpioCard.attachToPortB(&joystickAttachmentB);
```

**main.cpp** — Update USB gamepad handling to support two joysticks. The existing gamepad code updates `joystickAttachment.updateJoystick(buttons)` — this needs to be updated to route Gamepad 1 → `joystickAttachmentB` (Port B, primary) and Gamepad 2 → `joystickAttachmentA` (Port A, secondary). Search for all references to `joystickAttachment` and update accordingly.

**API routes** — If there are any REST API endpoints that reference the joystick (for the web interface), update them to handle two joysticks. Search for `joystick` references in main.cpp API route handlers.

---

## 5. Verify Sound/Video Register Behavior from 6502 Perspective

**Files**: `lib/6502/IO/SoundCard.cpp`, `lib/6502/IO/VideoCard.cpp`
**Priority**: LOW — Verification only, no changes expected.

The C++ SoundCard and VideoCard forward register writes to the web application via `avSend()` and store register values locally. The TypeScript equivalents include full synthesis/rendering. This is fine — the important thing is that register read/write behavior is consistent from the 6502's perspective.

### SoundCard (SID)
- **Write**: C++ stores `registers[address & 0x1F] = value` and calls `avSend()`. This matches the TS register store. No change needed.
- **Read**: C++ returns `registers[reg]` for $19–$1C and `0x00` for all others. The TS version returns live oscillator/envelope data for $1B (OSC3) and $1C (ENV3) instead of the stored register value. The TS behavior is more accurate to the real SID but since the C++ offloads synthesis, returning stored values is acceptable. **No change needed.**

### VideoCard (TMS9918A)
- Both implementations handle the data port read-ahead buffer, two-stage control port write, register writes, VRAM auto-increment, and vblank interrupt generation identically.
- **No change needed.**

---

## Summary / Execution Order

| # | Task | Priority | File(s) |
|---|------|----------|---------|
| 1 | ROM fill value → 0x00 | LOW | ROM.cpp |
| 2 | Cart fill value → 0x00 | LOW | Cart.cpp |
| 3 | ROM read-only (no-op write, add load method) | MEDIUM | ROM.cpp, ROM.h, main.cpp |
| 4 | Add second joystick (Port A) | LOW | main.cpp |
| 5 | Verify Sound/Video register behavior | LOW | SoundCard.cpp, VideoCard.cpp |

Recommended order: 1 → 2 → 3 → 4 → 5. Items 1–2 are trivial single-line changes. Item 3 requires adding a `load()` method and updating call sites. Item 4 involves wiring a second attachment and updating gamepad routing. Item 5 is verification only.

**Note**: The C++ emulator's CPU (vrEmu6502) already handles BCD mode, BIT #imm, branch cycle counting, and level-triggered IRQ correctly. No CPU changes are needed on the C++ side. The SerialCard (R6551) implementation is already the correct target model — single-byte TX/RX, no buffers, no baud timing. The RAMBank already has the correct $3FF write-through behavior. The keyboard encoder already defaults to Port B for routing. These are all items being fixed in the TypeScript project instead.
