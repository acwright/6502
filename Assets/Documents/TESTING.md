6502 Testing Procedures
=======================

Comprehensive testing procedures for validating hardware, firmware, and system integration. Use these tests to verify new builds, diagnose issues, and validate modifications.

---

## Table of Contents

- [Overview](#overview)
- [Hardware Testing](#hardware-testing)
- [Firmware Testing](#firmware-testing)
- [System Integration Testing](#system-integration-testing)
- [Peripheral Testing](#peripheral-testing)
- [Performance Testing](#performance-testing)
- [Automated Testing](#automated-testing)
- [Test Status](#test-status)

---

## Overview

### Testing Philosophy

**Objectives:**
1. **Verify functionality**: Does it work as designed?
2. **Ensure reliability**: Does it work consistently?
3. **Validate specifications**: Does it meet performance targets?
4. **Document limitations**: What doesn't work or has issues?

### Test Levels

**Level 1: Component Testing**
- Individual components in isolation
- Verify basic operation (power, signals, communication)

**Level 2: Board Testing**
- Complete board/card as unit
- All features functional
- Meets specifications

**Level 3: Integration Testing**
- Multiple boards/cards working together
- System-level functionality
- No conflicts or interference

**Level 4: Application Testing**
- Real-world usage scenarios
- Run actual programs and workflows
- Verify end-user experience

---

## Hardware Testing

### Pre-Power-On Tests

**Test 1: Visual Inspection**

**Purpose:** Catch assembly errors before power-on

**Procedure:**
1. [ ] Inspect under magnification (5-10×)
2. [ ] Check for solder bridges (especially between IC pins)
3. [ ] Verify all components soldered
4. [ ] Check component orientation:
   - [ ] ICs (pin 1 matches socket notch)
   - [ ] Diodes (cathode stripe matches silkscreen)
   - [ ] Electrolytic capacitors (+ lead to + pad)
   - [ ] LEDs (long lead to + side)
5. [ ] Look for cold solder joints (dull, grainy appearance)
6. [ ] Check for lifted pads or damaged traces

**Expected result:** No issues found

**If issues found:** Repair before proceeding

---

**Test 2: Resistance Check**

**Purpose:** Detect short circuits before applying power

**Equipment:**
- Digital multimeter (resistance mode)

**Procedure:**
```
1. Set multimeter to resistance (Ω) mode
2. Disconnect all power sources
3. Remove all ICs from sockets (if socketed)
4. Measure between 5V rail and GND:
   Expected: >10kΩ (typically 20-100kΩ)
   Measured: _______kΩ
5. If <1kΩ: SHORT CIRCUIT! Do not power on.
   - Inspect for solder bridges
   - Check for reversed electrolytic capacitors
   - Fix and re-test
```

**Pass criteria:** Resistance >10kΩ

---

**Test 3: Continuity Check**

**Purpose:** Verify critical connections intact

**Equipment:**
- Digital multimeter (continuity mode)

**Procedure:**
Test key connections (examples, adjust for specific board):
```
1. Power connections:
   [ ] 5V test point to IC VCC pins: continuity
   [ ] GND to all IC GND pins: continuity
   [ ] 5V to GND: NO continuity (otherwise short!)

2. Clock distribution:
   [ ] Clock source to CPU PHI2: continuity
   [ ] Clock to peripherals (if applicable): continuity

3. Critical signals (examples):
   [ ] CPU D0 to RAM D0: continuity
   [ ] CPU A0 to ROM A0: continuity
   [ ] Reset circuit to CPU RES pin: continuity
```

**Pass criteria:** All expected connections present, no unexpected connections

---

### Power-On Tests

**Test 4: Initial Power-On (No ICs)**

**Purpose:** Verify power supply correct before inserting expensive ICs

**Equipment:**
- 5V power supply
- Digital multimeter (DC voltage mode)

**Procedure:**
```
1. [ ] Remove all ICs from sockets (leave board empty)
2. [ ] Connect power supply (DO NOT TURN ON YET)
3. [ ] Set multimeter to DC voltage (20V range)
4. [ ] Turn on power supply
5. [ ] Measure voltage across 5V and GND:
   Expected: 4.75V - 5.25V
   Measured: _______V
6. [ ] Check voltage at each IC socket VCC pin:
   VCC pin of U1: _______V
   VCC pin of U2: _______V
   (etc. for all ICs)
7. [ ] All should be ~5V (±0.5V)
8. [ ] Turn off power
```

**Pass criteria:**
- Voltage in range 4.75V - 5.25V at all test points
- No smoke, no hot components

**If out of range:**
- Check power supply output
- Check polarity (center positive for barrel jack)
- Check voltage regulator (if present)

---

**Test 5: Insert and Test ICs Sequentially**

**Purpose:** Isolate problem IC if issues arise

**Procedure:**
```
1. Insert first IC (typically non-critical support IC)
2. Power on
3. Verify:
   [ ] No smoke
   [ ] IC warm but not hot (touch test after 30 seconds)
   [ ] Voltage still correct at test points
4. Power off
5. Insert next IC
6. Repeat steps 2-5 for all ICs
7. If any IC causes issue:
   - Remove that IC
   - Check orientation (may be backward!)
   - Try different IC (may be defective)
```

**Pass criteria:** All ICs inserted, power normal, no hot components

---

**Test 6: Clock Signal Verification**

**Purpose:** Verify CPU receives clock signal

**Equipment:**
- Oscilloscope OR logic probe

**Procedure (oscilloscope):**
```
1. Power on system
2. Connect scope probe to CPU PHI2 pin:
   - W65C02S: Pin 37 (40-pin DIP)
3. Set scope:
   - Timebase: 500ns/div (for 1 MHz clock)
   - Voltage: 2V/div
   - Trigger: Rising edge, ~2.5V
4. Observe waveform:
   Expected: Square wave at design frequency
   Frequency measured: _______Hz (expected: ~1 MHz)
   Amplitude: 0V to ~5V
   Duty cycle: 40-60%
```

**Procedure (logic probe):**
```
1. Power on system
2. Touch logic probe to PHI2 pin
3. Observe LED:
   Expected: PULSE indicator (blinking/both LEDs)
   Observed: [ ] PULSE  [ ] HI  [ ] LO
```

**Pass criteria:**
- Square wave present at expected frequency (±10%)
- Clean transitions (no excessive ringing)

**If no clock:**
- Check crystal/oscillator power (5V to VCC pin)
- Verify crystal/oscillator part number (correct frequency?)
- Test oscillator with scope directly at output pin
- Replace if defective

---

**Test 7: Reset Signal Verification**

**Purpose:** Verify reset circuit functional

**Equipment:**
- Digital multimeter OR oscilloscope

**Procedure:**
```
1. Power on system
2. Measure RES pin on CPU:
   - W65C02S: Pin 40
   Expected: HIGH (~5V) during normal operation
   Measured: _______V

3. Press reset button (if present)
   Expected: Voltage drops to LOW (<0.8V)
   Measured during reset: _______V

4. Release reset button
   Expected: Voltage returns to HIGH (~5V)
   Measured after release: _______V
```

**Pass criteria:**
- RES normally HIGH (>3.5V)
- RES goes LOW when reset asserted (<0.8V)
- RES returns HIGH when reset released

---

### Functional Tests

**Test 8: Address Bus Activity**

**Purpose:** Verify CPU executing instructions (fetching from memory)

**Equipment:**
- Logic analyzer OR oscilloscope

**Procedure (oscilloscope):**
```
1. Power on, observe system boot
2. Probe any address line (A0-A15):
   Expected: Toggling signal (changing addresses as CPU runs)
   Observed: [ ] Toggling  [ ] Stuck HIGH  [ ] Stuck LOW

3. If toggling: CPU is running! Proceed to next test
4. If stuck: Check ROM programming, reset circuit, clock
```

**Procedure (logic analyzer):**
```
1. Connect analyzer to address lines A0-A15
2. Trigger on any activity
3. Capture ~100 cycles
4. Examine capture:
   Expected: Changing addresses (incrementing PC, reading instructions)
   Observed: [ ] Activity seen  [ ] No activity  [ ] Repeating pattern
```

**Pass criteria:** Address lines actively changing

---

**Test 9: Memory Read/Write Test**

**Purpose:** Verify RAM functional

**Equipment:**
- Serial terminal (for monitor) OR logic analyzer

**Procedure (using monitor):**
```
1. Connect serial terminal (9600 baud, 8-N-1)
2. Power on, monitor should display prompt
3. Write test value to RAM:
   Command: W 0200 42
   (Write $42 to address $0200)

4. Read back:
   Command: R 0200
   Expected: Returns $42
   Observed: $________

5. Repeat with different addresses and values:
   W 0300 AA
   R 0300 → Expected: $AA, Observed: $________
   
   W 0400 55
   R 0400 → Expected: $55, Observed: $________
```

**Procedure (logic analyzer - advanced):**
```
1. Capture write cycle to RAM address
2. Verify:
   - Address lines = target address
   - Data lines = written value
   - RW = LOW (write)
   - RAM CS = active (LOW typically)
3. Capture read cycle from same address
4. Verify:
   - Address lines = target address
   - Data lines = previously written value
   - RW = HIGH (read)
   - RAM CS = active
```

**Pass criteria:** All writes and reads return correct data

---

**Test 10: ROM Verification**

**Purpose:** Verify ROM programmed correctly and readable

**Procedure:**
```
1. Using monitor or ROM test program, read reset vector:
   Address $FFFC-$FFFD contains reset address
   Expected: (varies by firmware, e.g., $E000 for typical ROM)
   Read $FFFC: $________
   Read $FFFD: $________

2. Read first few bytes of ROM code:
   Read $E000: $________
   Read $E001: $________
   Read $E002: $________
   
3. Compare to expected values (from ROM .bin file)
   [ ] Match → ROM correct
   [ ] Differ → ROM not programmed or wrong contents
```

**Alternative:** Calculate checksum (sum all ROM bytes, compare to expected)

**Pass criteria:** ROM contents match expected firmware

---

## Firmware Testing

### Compilation Tests

**Test 11: Build Firmware**

**Purpose:** Verify code compiles without errors

**Procedure:**
```bash
cd Firmware/<ProjectName>/
pio run
```

**Expected output:**
```
...
Building .pio/build/.../firmware.hex
Linking .pio/build/.../firmware.elf
RAM:   [====      ]  40.0% (used 1024 bytes from 2560 bytes)
Flash: [======    ]  60.0% (used 15360 bytes from 25600 bytes)
======================== [SUCCESS] Took 5.23 seconds ========================
```

**Pass criteria:**
- [ ] Build succeeds with status [SUCCESS]
- [ ] No errors
- [ ] Warnings acceptable (but should be investigated/fixed)
- [ ] RAM usage <90%
- [ ] Flash usage <95%

---

**Test 12: Upload Firmware**

**Purpose:** Verify firmware uploads to target successfully

**Procedure:**
```bash
pio run -t upload
```

**Expected output:**
```
...
Uploading .pio/build/.../firmware.hex
avrdude: ... bytes of flash written
avrdude: ... bytes of flash verified
avrdude done. Thank you.
======================== [SUCCESS] Took 8.45 seconds ========================
```

**Pass criteria:**
- [ ] Upload succeeds
- [ ] Verification passes (flash contents match uploaded file)

---

### Functional Tests

**Test 13: Serial Output Test**

**Purpose:** Verify firmware can output to serial terminal

**Procedure:**
```bash
# Upload firmware with serial output test code
pio device monitor

# Firmware should print test message on boot
```

**Expected output:**
```
6502 Test Firmware v1.0
Type '?' for help
>
```

**Pass criteria:**
- [ ] Serial output appears
- [ ] Text legible (not garbage)
- [ ] Correct baud rate detected

**If garbage characters:**
- Try different baud rates (115200, 57600, 19200, 9600)
- Check firmware UART configuration matches terminal

---

**Test 14: GPIO Test**

**Purpose:** Verify firmware can control GPIO pins

**Procedure:**
```c
// Test code (example for blinking LED)
void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}
```

**Verification:**
```
1. Upload firmware
2. Observe LED:
   Expected: Blinks at ~1 Hz (on 500ms, off 500ms)
   Observed: [ ] Correct  [ ] Different rate  [ ] No blink
3. Measure with multimeter (DC voltage mode):
   LED_PIN when HIGH: ~5V
   LED_PIN when LOW: ~0V
```

**Pass criteria:** LED blinks as expected, voltages correct

---

**Test 15: Peripheral Communication Test**

**Purpose:** Verify firmware can communicate with peripheral (VIA, ACIA, etc.)

**Example: VIA Register Read/Write**
```c
#define VIA_BASE 0xC000
#define VIA_DDRA (VIA_BASE + 0x03)

void testVIA() {
  // Write to data direction register
  *((volatile uint8_t*)VIA_DDRA) = 0xFF;
  
  // Read back
  uint8_t value = *((volatile uint8_t*)VIA_DDRA);
  
  // Should read back 0xFF
  if (value == 0xFF) {
    Serial.println("VIA test PASS");
  } else {
    Serial.print("VIA test FAIL: expected 0xFF, got 0x");
    Serial.println(value, HEX);
  }
}
```

**Pass criteria:** Read back value matches written value

---

## System Integration Testing

### End-to-End Tests

**Test 16: System Boot Test**

**Purpose:** Verify complete system boots and runs

**Procedure:**
```
1. Power on system
2. Observe startup:
   Expected sequence:
   - Power LED illuminates (if present)
   - Clock starts (verify with scope/probe)
   - CPU fetches reset vector from ROM
   - System jumps to monitor/BIOS
   - Monitor prints prompt to serial terminal
   
3. Monitor output:
   Connected to serial terminal? [ ] Yes [ ] No
   Prompt appears? [ ] Yes [ ] No
   If no: Check baud rate, cable, ACIA config
```

**Pass criteria:**
- System boots without intervention
- Monitor/BIOS prompt appears
- No errors during boot

---

**Test 17: Simple Program Execution**

**Purpose:** Verify CPU can execute user programs

**Test program (simple loop):**
```assembly
; Count from 0 to 255, output each value to GPIO card
      LDX #$00      ; Start at 0
Loop:
      STX $9401     ; Write to GPIO Port A (IO 6 at $9400)
      INX           ; Increment
      BNE Loop      ; Repeat until X wraps to 0
      BRK           ; Stop
```

**Procedure:**
```
1. Assemble test program
2. Load into RAM (via monitor or programmer)
3. Execute:
   Command: G 0800  (assuming program loaded at $0800)
4. Verify results:
   - Monitor address $9401 (or use logic analyzer)
   - Should see values 00, 01, 02, ..., FF
5. Check execution time:
   Expected: Depends on clock speed and program
   Measured: _______ms (or cycles)
```

**Pass criteria:** Program executes correctly, expected results

---

**Test 18: Interrupt Test**

**Purpose:** Verify interrupt handling works

**Test program:**
```assembly
; Set up IRQ handler
      LDA #<IRQHandler
      STA $FFFE       ; IRQ vector low byte
      LDA #>IRQHandler
      STA $FFFF       ; IRQ vector high byte
      CLI             ; Enable interrupts
      
MainLoop:
      JMP MainLoop    ; Wait for interrupt

IRQHandler:
      ; Increment counter at $0200
      INC $0200
      RTI             ; Return from interrupt
```

**Procedure:**
```
1. Load program
2. Configure VIA for timer interrupt (example):
   - Set timer to fire every 10ms
   - Enable interrupt
3. Run program
4. Monitor address $0200:
   Expected: Increments ~100 times per second
   Observed: Increments? [ ] Yes [ ] No
   Rate: ~_______Hz
```

**Pass criteria:** Counter increments at expected rate

---

## Peripheral Testing

### VIA (6522) Tests

**Test 19: VIA Port I/O**

**Purpose:** Verify VIA can read inputs and write outputs

**Equipment:**
- Jumper wires
- Multimeter OR LEDs + resistors

**Procedure:**
```assembly
; Configure Port A as outputs
LDA #$FF
STA VIA_DDRA    ; $9403

; Write test pattern
LDA #$AA        ; 10101010
STA VIA_ORA     ; $9401

; Measure with multimeter:
; PA0, PA2, PA4, PA6 should be ~5V (HIGH)
; PA1, PA3, PA5, PA7 should be ~0V (LOW)
```

**Pass criteria:** Output pins match written pattern

**Input test:**
```assembly
; Configure Port B as inputs
LDA #$00
STA VIA_DDRB    ; $9402

; Connect PB0 to GND (via jumper)
; Connect PB1 to +5V (via jumper)

; Read port
LDA VIA_IRB     ; $9400
; Bit 0 should be 0 (LOW)
; Bit 1 should be 1 (HIGH)
```

**Pass criteria:** Read values match external signals

---

**Test 20: VIA Timer**

**Purpose:** Verify VIA timer generates interrupts

**Procedure:**
```assembly
; Set Timer 1 for 10ms intervals (assuming 1 MHz clock)
; 1 MHz / 100 Hz = 10,000 cycles
LDA #<10000
STA VIA_T1CL    ; Timer 1 latch low
LDA #>10000
STA VIA_T1CH    ; Timer 1 latch high (starts timer)

; Enable Timer 1 interrupt
LDA #$C0        ; Enable T1 interrupt
STA VIA_IER

; Wait for interrupt (flag will be set in IFR)
Wait:
  LDA VIA_IFR
  AND #$40      ; Check T1 flag
  BEQ Wait      ; Loop until set
  
; Timer fired!
```

**Pass criteria:** Timer interrupt fires at expected interval (measure with scope or counter)

---

### ACIA (Serial) Tests

**Test 21: ACIA Loopback**

**Purpose:** Verify ACIA TX and RX work

**Procedure:**
```
1. Hardware loopback:
   - Connect ACIA TX pin to RX pin (short circuit)
   
2. Software test:
   a. Initialize ACIA (set baud rate, format)
   b. Transmit character 'A'
   c. Wait for RX buffer full flag
   d. Read received character
   e. Compare: should be 'A'
   
3. Expected result: TX character = RX character
   Transmitted: 'A'
   Received: $________
```

**Pass criteria:** Loopback receives same character sent

---

**Test 22: ACIA Baud Rate Accuracy**

**Purpose:** Verify ACIA transmits at correct baud rate

**Equipment:**
- Oscilloscope

**Procedure:**
```
1. Configure ACIA for known baud rate (e.g., 9600)
2. Transmit character repeatedly
3. Measure bit time on scope:
   9600 baud = 9600 bits/second
   Bit time = 1/9600 = 104.17 µs
4. Measure on scope:
   Expected: ~104 µs per bit
   Measured: _______µs
5. Calculate actual baud rate:
   Actual = 1,000,000 / measured_time
   Actual: _______baud
```

**Pass criteria:** Baud rate within ±2% of target

---

### Display Tests

**Test 23: LCD Character Display**

**Purpose:** Verify LCD can display characters

**Procedure:**
```c
// Initialize LCD (HD44780 compatible)
lcdInit();

// Clear display
lcdClear();

// Write test string
lcdPrint("Hello, 6502!");

// Verify display shows: Hello, 6502!
```

**Visual verification:**
- [ ] Text visible (not blank)
- [ ] Correct characters (not garbage)
- [ ] Proper contrast (adjust pot if needed)

**Pass criteria:** "Hello, 6502!" appears on LCD

---

**Test 24: VGA Video Output**

**Purpose:** Verify VGA generates valid signal

**Equipment:**
- VGA monitor
- Oscilloscope (optional, for signal verification)

**Procedure:**
```
1. Connect VGA cable to monitor
2. Power on system
3. Observe monitor:
   - Does it sync? (no "No Signal" message)
   - Is there an image?
   - Is image stable (not flickering/rolling)?
   
4. Test pattern (checkerboard or color bars):
   - Run test pattern firmware
   - Verify pattern displayed correctly
   
5. Scope measurements (optional):
   - HSYNC: ~31.5 kHz (for 640×480@60Hz)
   - VSYNC: ~60 Hz
   - RGB: 0-0.7V analog signals
```

**Pass criteria:**
- Monitor syncs and displays image
- No artifacts or distortion

---

## Performance Testing

**Test 25: Clock Speed Verification**

**Purpose:** Verify system runs at expected clock speed

**Equipment:**
- Frequency counter OR oscilloscope

**Procedure:**
```
1. Measure PHI2 clock frequency:
   Expected: 1.000 MHz (or design frequency)
   Measured: _______MHz
   
2. Verify within tolerance:
   Tolerance: ±1% (±10 kHz for 1 MHz)
   Pass if: 0.990 MHz < measured < 1.010 MHz
```

**Pass criteria:** Frequency within ±1% of target

---

**Test 26: Instruction Timing**

**Purpose:** Verify CPU executes instructions at expected speed

**Test program:**
```assembly
; Simple NOP loop (2 cycles per NOP)
Loop:
  NOP    ; 2 cycles
  JMP Loop  ; 3 cycles
; Total: 5 cycles per iteration
```

**Procedure:**
```
1. Run test program
2. Measure loop frequency on scope (probe any toggling signal)
3. Calculate:
   Loop frequency = Clock / 5
   Expected @ 1 MHz: 200 kHz
   Measured: _______kHz
4. Verify matches expected
```

**Pass criteria:** Loop frequency = clock / cycles (±1%)

---

**Test 27: Memory Access Speed**

**Purpose:** Verify memory meets timing requirements

**Equipment:**
- Oscilloscope
- Logic analyzer (preferred)

**Procedure:**
```
1. Capture memory read cycle:
   - Trigger on address stable
   - Capture RW, address, data, PHI2
2. Measure timing:
   - Address setup time: _______ns
   - Data valid time: _______ns
   - Access time (address to data): _______ns
3. Compare to RAM specifications:
   RAM spec (e.g., AS6C62256): 55ns typical
   Measured: _______ns
```

**Pass criteria:** Memory access time < clock period

---

## Automated Testing

### Continuous Integration Tests

**For firmware:**
Create `.github/workflows/build.yml`:
```yaml
name: Build Firmware

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - name: Set up Python
      uses: actions/setup-python@v2
      with:
        python-version: '3.x'
    - name: Install PlatformIO
      run: pip install platformio
    - name: Build firmware
      run: |
        cd Firmware/DB\ Emulator
        pio run
```

**For hardware:**
- KiCad ERC/DRC can be run via command line
- Add to CI workflow for automated checks

---

## Test Status

### Test Coverage Goals

**Hardware:**
- [ ] All boards power-on tested
- [ ] All boards functionally tested
- [ ] All boards integration tested
- [ ] Performance tested (clock speed, timing margins)

**Firmware:**
- [ ] All firmware projects compile
- [ ] All firmware projects upload successfully
- [ ] Core functionality tested for each project
- [ ] Edge cases tested

**System:**
- [ ] All system configurations boot
- [ ] All peripheral combinations tested
- [ ] Stress testing completed (long-term operation)

---

**For more information, see:**
- [Troubleshooting Guide](TROUBLESHOOTING.md) - Diagnostic procedures for failures
- [Assembly Guide](ASSEMBLY_GUIDE.md) - Assembly procedures
- [Hardware README](../../Hardware/README.md) - Hardware specifications

---
