# GPIO Card - 65C22 VIA Emulation

## Overview

The GPIO Card provides a complete emulation of the 65C22 VIA (Versatile Interface Adapter) IC, handling keyboard and joystick input for the 6502 emulator system.

## Features

### 1. Complete 65C22 VIA Register Set

All 16 VIA registers are fully implemented:

- **$0 (ORB/IRB)**: Output/Input Register B
- **$1 (ORA/IRA)**: Output/Input Register A
- **$2 (DDRB)**: Data Direction Register B
- **$3 (DDRA)**: Data Direction Register A
- **$4 (T1C-L)**: Timer 1 Counter Low
- **$5 (T1C-H)**: Timer 1 Counter High
- **$6 (T1L-L)**: Timer 1 Latch Low
- **$7 (T1L-H)**: Timer 1 Latch High
- **$8 (T2C-L)**: Timer 2 Counter Low
- **$9 (T2C-H)**: Timer 2 Counter High
- **$A (SR)**: Shift Register
- **$B (ACR)**: Auxiliary Control Register
- **$C (PCR)**: Peripheral Control Register
- **$D (IFR)**: Interrupt Flag Register
- **$E (IER)**: Interrupt Enable Register
- **$F (ORA/IRA)**: Output/Input Register A (no handshake)

### 2. Dual-Mode Keyboard Support

#### Automatic Keyboard Encoder Mode (Default)
- **CB2**: Controls keyboard encoder enable (LOW = enabled, HIGH = disabled)
- **CB1**: Interrupt signal when ASCII data is ready
- **Port B**: Presents ASCII data from keyboard encoder
- When enabled, the keyboard encoder automatically scans the matrix and presents ASCII codes

#### Manual Keyboard Matrix Scanning Mode
- **8×8 Matrix**: PA0-PA7 (rows) × PB0-PB7 (columns)
- Software can scan the keyboard directly by:
  1. Disabling the encoder via CB2 (set HIGH)
  2. Writing to Port B to select columns
  3. Reading Port A to detect key presses in selected columns

#### Keyboard Matrix Layout
```
        PB0    PB1    PB2    PB3    PB4    PB5    PB6    PB7
PA0:    `      1      2      3      4      5      6      7
PA1:    8      9      0      -      =      BS     ESC    TAB
PA2:    Q      W      E      R      T      Y      U      I
PA3:    O      P      [      ]      \      INS    CAPS   A
PA4:    S      D      F      G      H      J      K      L
PA5:    ;      '      ENTER  DEL    SHIFT  Z      X      C
PA6:    V      B      N      M      ,      .      /      UP
PA7:    CTRL   MENU   ALT    SPACE  FN     LEFT   DOWN   RIGHT
```

### 3. PS/2 Keyboard Support
- **CA2**: Controls PS/2 port enable (LOW = enabled, HIGH = disabled)
- **CA1**: Interrupt signal when PS/2 ASCII data is ready
- **Port A**: Presents ASCII data from PS/2 keyboard
- Provides additional keyboard input via PS/2 connector

### 4. Joystick Support
- **Digital Inputs**: UP, DOWN, LEFT, RIGHT, Y, X, B, A (FIRE)
- **8-bit Button State**: One bit per button
- Joystick data overlays on Port A and Port B based on configuration
- Active-low button inputs (pressed = 0, released = 1)

### 5. VIA Timers
- **Timer 1**: 16-bit timer with:
  - One-shot or free-running mode (controlled by ACR bit 6)
  - PB7 toggle output option (ACR bit 7)
  - Interrupt on timeout (IRQ_T1)
  
- **Timer 2**: 16-bit timer with:
  - One-shot mode
  - Interrupt on timeout (IRQ_T2)

### 6. Interrupt System
Seven interrupt sources with individual enable/flag bits:
- **CA1**: PS/2 keyboard data ready
- **CA2**: PS/2 control line
- **CB1**: Keyboard encoder data ready
- **CB2**: Keyboard encoder control line
- **Shift Register**: Shift register complete
- **Timer 1**: Timer 1 timeout
- **Timer 2**: Timer 2 timeout

## API Functions

### Core VIA Functions
```cpp
uint8_t read(uint16_t address);      // Read VIA register
void write(uint16_t address, uint8_t value);  // Write VIA register
uint8_t tick(uint32_t cpuFrequency); // Update timers and return IRQ status
void reset();                         // Reset all VIA registers
```

### Input Update Functions
```cpp
void updateKeyboard(uint8_t key);     // Update keyboard matrix with key press
void releaseKey(uint8_t key);         // Release key from matrix
void updatePS2Keyboard(uint8_t ascii); // Update PS/2 keyboard input
void updateJoystick(uint8_t buttons);  // Update joystick button state
```

## Usage Examples

### Example 1: Reading Keyboard in Encoder Mode
```assembly
; Setup
LDA #$00
STA VIA_DDRA    ; Port A as input
STA VIA_DDRB    ; Port B as input

; Enable CB1 interrupt for keyboard data ready
LDA #$90        ; Set bit 7 (enable) and bit 4 (CB1)
STA VIA_IER

; Wait for keyboard interrupt
WAIT:
  LDA VIA_IFR
  AND #$10      ; Check CB1 flag
  BEQ WAIT

; Read ASCII key from Port B
LDA VIA_ORB     ; Reading ORB clears CB1 interrupt
```

### Example 2: Manual Keyboard Matrix Scanning
```assembly
; Disable keyboard encoder via CB2
LDA VIA_PCR
ORA #$E0        ; Set CB2 to manual output HIGH
STA VIA_PCR

; Setup ports
LDA #$FF
STA VIA_DDRB    ; Port B as output (column select)
LDA #$00
STA VIA_DDRA    ; Port A as input (row read)

; Scan column 0
LDA #$01
STA VIA_ORB     ; Select column 0
LDA VIA_ORA     ; Read rows
; Check for key presses in column 0

; Scan column 1
LDA #$02
STA VIA_ORB
LDA VIA_ORA
; And so on...
```

### Example 3: Reading Joystick
```cpp
// Update joystick state
uint8_t buttons = 0;
if (digitalRead(JOY_UP_PIN) == LOW) buttons |= JOY_UP;
if (digitalRead(JOY_DOWN_PIN) == LOW) buttons |= JOY_DOWN;
if (digitalRead(JOY_LEFT_PIN) == LOW) buttons |= JOY_LEFT;
if (digitalRead(JOY_RIGHT_PIN) == LOW) buttons |= JOY_RIGHT;
if (digitalRead(JOY_A_PIN) == LOW) buttons |= JOY_A;

gpioCard.updateJoystick(buttons);

// Read in 6502 code
; LDA VIA_ORA  ; Joystick state in accumulator
```

### Example 4: Using Timer 1
```assembly
; Setup Timer 1 for 1ms timeout at 1MHz
LDA #$E8        ; Low byte ($03E8 = 1000 cycles)
STA VIA_T1CL
LDA #$03        ; High byte
STA VIA_T1CH    ; Starts timer

; Enable Timer 1 interrupt
LDA #$C0        ; Set bit 7 (enable) and bit 6 (T1)
STA VIA_IER

; Wait for timeout
WAIT:
  LDA VIA_IFR
  AND #$40      ; Check T1 flag
  BEQ WAIT
```

## Joystick Button Mapping
```
| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|-------|-------|-------|-------|-------|-------|-------|-------|
|   Y   |   X   |   B   |   A   |   R   |   L   |   D   |   U   |

Bit 0: UP
Bit 1: DOWN
Bit 2: LEFT
Bit 3: RIGHT
Bit 4: A button (FIRE)
Bit 5: B button
Bit 6: X button
Bit 7: Y button
```

## Control Line Behavior

### CA2 (PS/2 Control)
- **Mode 110b** (PCR bits 1-3): Manual LOW = PS/2 enabled
- **Mode 111b**: Manual HIGH = PS/2 disabled

### CB2 (Keyboard Encoder Control)
- **Mode 110b** (PCR bits 5-7): Manual LOW = Encoder enabled
- **Mode 111b**: Manual HIGH = Encoder disabled (manual scan mode)

## Implementation Notes

1. **Timer Resolution**: Timers decrement on each `tick()` call. The tick rate should match the CPU frequency.

2. **Interrupt Flags**: Reading data registers (ORA, ORB) automatically clears associated interrupt flags (CA1, CA2, CB1, CB2).

3. **Port Direction**: When a DDR bit is 0 (input), reading the port returns external data. When 1 (output), reading returns the output register value.

4. **Keyboard Matrix State**: The matrix maintains pressed key state until `releaseKey()` is called, allowing multiple simultaneous key presses.

5. **ASCII Conversion**: The keyboard encoder automatically converts matrix positions to ASCII codes based on the predefined mapping.

## Future Enhancements

- Shift register operations
- Analog joystick support via ADC
- Additional keyboard layouts
- Programmable key macros
- USB keyboard/mouse support

## License

Part of the DB Emulator project.
