6502 Troubleshooting Guide
==========================

Comprehensive troubleshooting procedures for diagnosing and fixing issues with 6502 systems. Organized by symptom with step-by-step diagnostic procedures.

---

## Table of Contents

- [General Troubleshooting Approach](#general-troubleshooting-approach)
- [Power Issues](#power-issues)
- [Boot Failures](#boot-failures)
- [Clock and Reset Problems](#clock-and-reset-problems)
- [Memory Issues](#memory-issues)
- [Bus Problems](#bus-problems)
- [Peripheral Issues](#peripheral-issues)
- [Display Problems](#display-problems)
- [Serial Communication Issues](#serial-communication-issues)
- [Intermittent Faults](#intermittent-faults)
- [Testing Procedures](#testing-procedures)
- [Component Testing](#component-testing)

---

## General Troubleshooting Approach

### Systematic Diagnosis

**Step 1: Define the problem**
- What is the expected behavior?
- What is observed instead?
- When did it start? (new build, after modification, suddenly failed)

**Step 2: Visual inspection**
- Look for obvious issues (smoke, burned components, loose connections)
- Check IC orientations (especially CPU!)
- Look for solder bridges (magnification helpful)
- Verify power LED (if present)

**Step 3: Isolate the problem**
- Does it affect one subsystem or entire system?
- Is it consistent or intermittent?
- Does it occur immediately or after warm-up?

**Step 4: Form hypothesis**
- Based on symptoms, what could cause this?
- List possible causes (most likely first)

**Step 5: Test hypothesis**
- Perform measurements or swaps to verify
- If confirmed: fix
- If not: next hypothesis

**Step 6: Verify fix**
- After repair, retest thoroughly
- Ensure no new issues introduced

### Essential Tools

**Minimum:**
- Digital multimeter (continuity, DC voltage, resistance)
- Visual magnification (5-10×)
- Good lighting

**Recommended:**
- Logic probe (detect high/low/pulse signals quickly)
- Oscilloscope (view waveforms, measure frequency/timing)
- Logic analyzer (capture bus transactions)
- EEPROM programmer (TL866 or similar, for verifying/re-programming ROMs)

**Advanced:
- ROM emulator (test without programming ROM)
- In-circuit emulator (advanced debugging)

---

## Power Issues

### Symptom: No Power LED / Dead System

**Possible causes:**
- Power supply failure/wrong voltage
- Incorrect polarity
- Short circuit on board
- Power switch defective
- LED installed backward

**Diagnostic procedure:**

**Step 1: Check power supply**
```
Test: Measure power supply OUTPUT with multimeter (DC voltage)
Expected: 4.75V - 5.25V
Measured: _______
```
- If 0V: Power supply dead or not turned on → replace supply
- If wrong voltage: Adjust supply or replace
- If correct voltage at supply, proceed to Step 2

**Step 2: Check power connection**
```
Test: Verify polarity (center positive for barrel jack)
Expected: Center pin = +5V, outer = GND
Measured: Center pin = _______V, Outer = _______V
```
- If reversed: Disconnect immediately! Check for damaged components → fix polarity
- If correct, proceed to Step 3

**Step 3: Check for short circuit**
```
Test: Disconnect power. Measure resistance between 5V and GND
Expected: >10kΩ (typically 20-100kΩ)
Measured: _______Ω
```
- If <100Ω: **Short circuit present!**
  - [ ] Remove all ICs from sockets
  - [ ] Re-measure (if now high resistance → bad IC, if still low → board short)
  - [ ] Inspect for solder bridges (especially IC pins, power traces)
  - [ ] Check for reversed electrolytic capacitors (will short!)
  - [ ] Fix bridges, replace shorted components
- If >10kΩ, proceed to Step 4

**Step 4: Check power LED**
```
Test: Verify LED orientation
Expected: Long lead (anode) to + side (typically square pad)
Check: LED leads match silkscreen? [ ] Yes [ ] No
```
- If backward: Desolder, reverse, re-solder
- If correct orientation:
  - Check LED current-limiting resistor (typically 330Ω-1kΩ)
  - Test LED with separate power source (3V battery + resistor)
  - Replace if defective

**Step 5: Check voltage regulator (if present)**
```
Test: Measure input voltage to regulator, output voltage
Regulator type: _______ (e.g., 7805, LM2940)
Input voltage: _______V (should be >7V for 7805)
Output voltage: _______V (should be ~5V)
```
- If input OK but output low/zero: Regulator failed → replace
- If input low: Check input power connection

### Symptom: System Resets Randomly / Unstable

**Possible causes:**
- Insufficient power supply current
- Voltage droop under load
- Noisy power supply
- Inadequate decoupling capacitors
- Loose connections

**Diagnostic procedure:**

**Step 1: Measure voltage under load**
```
Test: Measure 5V rail voltage while system running
Idle voltage: _______V
Under load voltage: _______V
Expected: Should remain 4.75V - 5.25V
```
- If drops below 4.75V: Power supply insufficient → upgrade to higher current rating (2A minimum)

**Step 2: Check decoupling capacitors**
- Visual inspection: Each IC should have 100nF ceramic cap nearby
- Missing caps: Add 100nF capacitor across VCC and GND pins of each IC (close to IC)

**Step 3: Check ripple voltage**
```
Test: Oscilloscope on AC coupling, measure 5V rail
Expected: <100mV peak-to-peak ripple
Observed: _______mV p-p
```
- If excessive ripple (>100mV): Add bulk capacitance (100-1000µF electrolytic near power input)

**Step 4: Check connections**
- Reseat all ICs in sockets (remove and re-insert)
- Check for cold solder joints (especially power connector)
- Verify screws tight on terminal blocks

---

## Boot Failures

### Symptom: System Powers On, No Activity

**Possible causes:**
- CPU not clocked
- CPU in reset
- ROM not programmed or wrong address
- Wrong CPU orientation
- Defective CPU or support chips

**Diagnostic procedure:**

**Step 1: Check CPU orientation**
```
Visual check: Pin 1 of CPU matches socket/silkscreen?
[ ] Yes → proceed to Step 2
[ ] No → Power off IMMEDIATELY! Remove CPU, correct orientation, reinsert
```

**Step 2: Check clock signal**
```
Test: Oscilloscope or logic probe on CPU PHI2 pin (pin 37 for 40-pin W65C02S)
Expected: Square wave at design frequency (e.g., 1 MHz)
Observed: [ ] Square wave _______ Hz
          [ ] No signal
          [ ] Constant high or low
```
- If no signal or constant:
  - Check crystal/oscillator (should oscillate, measure with scope)
  - Verify oscillator power (5V to pins)
  - Replace crystal/oscillator if defective
- If wrong frequency: Check crystal value, capacitor values
- If correct frequency, proceed to Step 3

**Step 3: Check reset signal**
```
Test: Measure RES pin (pin 40 on W65C02S) while powered
Expected: Should be HIGH (~5V) during normal operation
          Briefly LOW when reset button pressed
Measured: _______V
```
- If stuck LOW (0V):
  - Check reset circuit (should pull-up to 5V)
  - Check for short to ground
  - Disconnect reset button, re-measure (button may be shorted)
- If constantly HIGH but system doesn't run, proceed to Step 4

**Step 4: Check ROM**
```
Test: Verify ROM programmed with correct firmware
Remove ROM from socket
Program with known-good firmware (see Firmware/ folder)
Verify programming with ROM programmer
Reinsert ROM
```
- Ensure ROM chip enable (CE) signal correct
- Measure voltage at CE pin (should go LOW when address in ROM range)

**Step 5: Check address bus activity**
```
Test: Oscilloscope or logic analyzer on address lines A0-A15
Expected: Changing signals (CPU fetching instructions)
Observed: [ ] Activity on address lines
          [ ] All stuck HIGH or LOW
          [ ] Some active, some stuck
```
- If no activity: CPU not running or halted
  - Verify RDY pin (pin 2) HIGH (if LOW, CPU is waiting)
  - Verify BE pin (pin 36, 65C02S) HIGH (if LOW, CPU is tri-stated)
  - Check for stuck data bus lines (pull-down/pull-up)
- If activity seen, proceed to Step 6

**Step 6: Check data bus**
```
Test: Logic analyzer capture reset sequence
Expected: CPU reads reset vector from $FFFC-$FFFD, jumps to address
Observed: Reset vector = $________ (read from ROM)
          CPU jumped to: $________
```
- If vector wrong: ROM not programmed correctly or wrong ROM address mapping
- If vector correct but execution fails: Check RAM, check ROM contents match expected

**Step 7: Swap components**
- Try known-good CPU
- Try known-good ROM (pre-programmed)
- Try known-good RAM
- Isolate defective component

### Symptom: System Partially Boots (Some Output, Then Hangs)

**Possible causes:**
- RAM failure
- ROM corruption
- Address decoding error
- Specific instruction triggering hardware fault

**Diagnostic procedure:**

**Step 1: Determine where execution stops**
```
Test: Monitor serial output or use logic analyzer
Last output seen: _________________________________
Approximate address: $________
```

**Step 2: Check RAM**
```
Test: RAM test program (write, read, compare all addresses)
Use monitor command or upload RAM test program
Results: [ ] Pass
         [ ] Fail at address: $________
```
- If RAM fails: Replace RAM chip
- If RAM passes, proceed to Step 3

**Step 3: Check ROM contents**
```
Test: Read ROM with programmer, compare to original firmware file
Results: [ ] Match
         [ ] Differ at address: $________
```
- If differ: Re-program ROM

**Step 4: Check address decoding**
- Logic analyzer: Capture reads/writes around failure point
- Verify correct peripheral selected for each address
- Check for bus contention (multiple devices driving bus simultaneously)

---

## Clock and Reset Problems

### Symptom: Clock Not Running / Wrong Frequency

**Diagnostic procedure:**

**Step 1: Verify oscillator power**
```
Test: Measure voltage on oscillator VCC pin
Expected: 4.75V - 5.25V
Measured: _______V
```
- If low/zero: Check power connection, trace from oscillator VCC to 5V rail

**Step 2: Check oscillator output**
```
Test: Oscilloscope on oscillator output pin
Expected: Square wave at crystal frequency
Observed: [ ] Square wave _______ Hz
          [ ] Sine wave (may work but unusual)
          [ ] No signal
```
- If no signal:
  - Power off, carefully remove oscillator
  - Measure oscillator in isolation (with separate power)
  - Replace if defective
- If wrong frequency: Wrong crystal installed → check crystal markings

**Step 3: Check loading capacitors (for crystal oscillators)**
```
Schematic shows: _______pF capacitors to ground
Installed: _______pF
```
- Wrong capacitance affects frequency
- Replace with correct values (typically 18-22pF for microprocessor crystals)

**Step 4: Check clock distribution**
- Measure PHI2 at CPU (should match oscillator frequency)
- Check for breaks in clock trace
- Verify clock signal reaches all peripherals (if synchronous bus)

### Symptom: Reset Not Working / Stuck in Reset

**Diagnostic procedure:**

**Step 1: Check reset circuit**
```
Test: Measure RES pin voltage
Expected: HIGH (~5V) during normal operation
          LOW (<0.8V) when reset asserted
Measured: Normal = _______V
          Reset button pressed = _______V
```

**Step 2: If stuck LOW:**
- Check for short to ground
- Disconnect reset button, re-measure
  - If now HIGH: Button or switch shorted → replace
  - If still LOW: Trace issue or component pulling down

**Step 3: If stuck HIGH (won't reset):**
- Press reset button, measure voltage (should drop to ~0V)
- If doesn't drop: Button not connected or defective
- Check continuity through button

**Step 4: Check reset timing**
```
Test: Oscilloscope on RESET pin
Press reset button, observe waveform
Expected: Clean transition LOW → HIGH
          ≥2 clock cycles LOW duration
Observed: [ ] Clean transition
          [ ] Slow transition (RC too large)
          [ ] Bouncing (switch bounce)
```
- If slow: Reduce capacitor value or increase resistor value
- If bouncing: Add debounce capacitor (0.1µF typical)

---

## Memory Issues

### Symptom: RAM Test Fails / Data Corruption

**Diagnostic procedure:**

**Step 1: Simple RAM test**
```assembly
LDA #$55      ; Pattern 1
STA $0200     ; Write to RAM
LDA $0200     ; Read back
CMP #$55      ; Compare
BNE Fail      ; Branch if not equal

LDA #$AA      ; Pattern 2 (inverse bits)
STA $0200
LDA $0200
CMP #$AA
BNE Fail
```
- If fails: Proceed to Step 2

**Step 2: Determine failure pattern**
- Test multiple addresses
- Test different data patterns ($ 00, $FF, $55, $AA)
- Note: Which addresses fail? Which bit(s) wrong?

**Pattern analysis:**
```
Same bit always wrong (e.g., bit 3) → Data line D3 issue
Same address range fails → Address decoding issue
Random failures → Multiple issues or power problem
All locations fail → RAM chip defective or not selected
```

**Step 3: Check RAM chip enable (CE/CS)**
```
Test: Logic probe or oscilloscope on RAM CE pin
Expected: LOW when CPU accesses RAM address ($0000-$7FFF)
          HIGH when accessing other addresses
Observed: [ ] Toggles correctly
          [ ] Stuck HIGH (RAM never selected!)
          [ ] Stuck LOW (RAM always selected, bus conflict!)
```
- If stuck HIGH: Address decoding error → fix decoder logic
- If stuck LOW: Short circuit or decoder fault

**Step 4: Check specific data line (if one bit always fails)**
```
Example: Bit 3 always reads 0
Test: Trace D3 from CPU to RAM
      Check for breaks, shorts
      Verify continuity with multimeter (power off!)
From CPU D3 (pin 38) to RAM D3 (pin 13): _______ Ω (should be <1Ω)
```
- If open circuit (infinite resistance): Broken trace or solder joint → repair
- If short to GND or +5V: Fix short

**Step 5: Swap RAM chip**
- Replace with known-good SRAM
- If problem persists: Not RAM chip (check bus, decoder)
- If problem fixed: RAM chip was defective

### Symptom: ROM Not Reading Correctly

**Diagnostic procedure:**

**Step 1: Verify ROM chip enable**
```
Test: Logic probe on ROM CE pin
Expected: LOW when CPU accesses ROM address ($8000-$FFFF)
Observed: [ ] Correct
          [ ] Stuck HIGH (ROM never enabled!)
          [ ] Stuck LOW (may conflict with RAM)
```

**Step 2: Verify ROM contents**
- Remove ROM from socket
- Read with ROM programmer
- Compare checksum to original firmware file
- If mismatch: Re-program ROM
  - Note: System has two ROM banks (LO ROM at $A000, HI ROM at $C000)
  - Verify you're checking the correct ROM chip

**Step 3: Check address lines**
- LO ROM ($A000-$BFFF): A0-A12 (8KB = 13 address lines)
- HI ROM ($C000-$FFFF): A0-A13 (16KB = 14 address lines)
- Check for swapped address lines (symptom: ROM works but code scrambled)

---

## Bus Problems

### Symptom: Bus Contention / Data Bus Issues

**Symptom:** Erratic behavior, random crashes, hot chips

**Diagnostic procedure:**

**Step 1: Check for multiple drivers**
- Only ONE device should drive data bus at a time
- Verify chip enable  signals mutually exclusive (logic analyzer)
- Example: When RAM CE LOW, ROM CE must be HIGH

**Step 2: Thermal check**
```
Test: Power on, wait 1 minute, touch ICs (carefully!)
Expected: Warm to touch (30-40°C typical)
Hot to touch (>60°C, painful): Problem! → power off
Hot chip: ___________________
```
- Hot chip indicates excessive current (possible short or bus contention)

**Step 3: Logic analyzer capture**
- Capture bus transactions
- Look for:
  - Multiple chip enables active simultaneously
  - Address glitches (brief incorrect addresses)
  - Data bus conflicts (driver changes during read cycle)

**Step 4: Isolate problem device**
- Remove ICs one at a time
- Test after each removal
- When problem disappears, last removed IC is culprit (or its address decoder)

---

## Peripheral Issues

### Symptom: VIA (6522) Not Responding

**Diagnostic procedure:**

**Step 1: Verify addressing**
```
Test: Write to VIA register, read back
Example (assuming GPIO Card at IO 6: $9400):
  LDA #$FF    ; All outputs
  STA $9403   ; DDRA
  LDA $9403   ; Read back
  CMP #$FF    ; Should match
```
- If doesn't match: VIA not selected or defective

**Step 2: Check VIA chip selects**
```
Test: Logic probe on VIA CS1, CS2
Expected: CS1 = HIGH, CS2 = LOW when VIA addressed
Observed: CS1 = _______,  CS2 = _______
```
- If wrong: Address decoding error

**Step 3: Test specific VIA function**
- Output test: Set port to output, write data, measure pins with multimeter
- Input test: Apply voltage to input pin, read register
- Timer test: Configure timer, verify interrupt or poll flag

**Step 4: Swap VIA chip**
- Replace with known-good 6522
- If problem persists: Not VIA chip (check address decoder, connections)

### Symptom: ACIA (Serial) Not Working

**See Serial Communication Issues section below.**

---

## Display Problems

### Symptom: LCD Shows Nothing

**Diagnostic procedure:**

**Step 1: Check LCD power**
```
Test: Measure voltage on LCD pins
Pin 2 (VDD): _______V (should be +5V)
Pin 1 (VSS): 0V (ground)
Pin 3 (VO):  _______V (contrast, 0-5V adjustable)
```
- If Pin 2 not 5V: Check power connection
- If Pin 3 near 5V: Contrast too low (invisible text) → adjust pot to ~0.5V

**Step 2: Adjust contrast**
- Locate contrast potentiometer (near LCD or on board)
- Turn very slowly, watching LCD
- Should see blocks appear (may be faint at first)
- Adjust until text clearly visible

**Step 3: Check LCD enable signal**
```
Test: Logic probe on E (Enable) pin (pin 6 typical)
Expected: Pulse activity when CPU writes to LCD
Observed: [ ] Pulses
          [ ] Stuck HIGH or LOW
          [ ] No activity
```
- If no activity: LCD controller not being written → check firmware

**Step 4: Verify initialization sequence**
- LCD requires specific initialization (see HD44780 datasheet)
- Check firmware sends correct init commands:
  1. Wait >40ms after power-on
  2. Send function set commands (3 times)
  3. Display on/off control
  4. Clear display
  5. Entry mode set
- Missing or wrong init: LCD won't work

**Step 5: Check data pins**
- Verify D0-D7 (or D4-D7 for 4-bit mode) connected correctly
- Check for swapped pins (symptom: garbage characters)

### Symptom: VGA No Signal

**Diagnostic procedure:**

**Step 1: Check VGA cable**
- Verify securely connected both ends
- Try different cable (cables can fail)

**Step 2: Check VGA connector**
```
Test: Measure voltages on VGA connector
Pin 9 (+5V): _______V (should be +5V if provided)
Pins 5, 6, 7, 8, 10 (GND): 0V
```

**Step 3: Check video signals**
```
Test: Oscilloscope on HSYNC, VSYNC pins
Expected: Periodic pulses
  HSYNC (pin 13): ~31 kHz (for 640×480@60Hz)
  VSYNC (pin 14): ~60 Hz
Observed: HSYNC = _______ Hz (or no signal)
          VSYNC = _______ Hz (or no signal)
```
- If no sync signals: Video generator not running → check firmware

**Step 4: Check RGB signals**
```
Test: Oscilloscope on Red/Green/Blue pins (pins 1, 2, 3)
Expected: Analog voltage 0-0.7V (varying with image content)
Observed: [ ] Varying voltage
          [ ] Stuck at 0V
          [ ] Stuck at high voltage
```

**Step 5: Check DAC resistors (if using resistor DAC)**
- Verify correct resistor values for R-2R or simple resistor DAC
- Typical: 470Ω series resistor for each color
- Check for cold solder joints

---

## Serial Communication Issues

### Symptom: No Serial Output

**Diagnostic procedure:**

**Step 1: Check terminal settings**
```
Baud rate: 9600 (or as configured)
Data bits: 8
Parity: None
Stop bits: 1
Flow control: None
```
- Must match ACIA configuration exactly

**Step 2: Verify serial cable**
- Null-modem vs straight-through (usually need null-modem for PC)
- Pins: TX → RX, RX → TX, GND → GND (minimum)
- Test cable with loopback (jumper TX to RX, type in terminal, should echo)

**Step 3: Check ACIA initialization**
```assembly
; Typical ACIA init (for 6551)
LDA #$1F      ; 19200 baud, 8-N-1
STA $C103     ; Control register
LDA #$0B      ; No parity, enable TX/RX
STA $C102     ; Command register
```
- Verify firmware includes init code
- Verify baud rate matches terminal setting

**Step 4: Measure TX signal**
```
Test: Oscilloscope or logic probe on TX pin
Expected: Idle HIGH (~5V), data pulses LOW during transmission
Observed: [ ] Idle HIGH, pulses during transmit
          [ ] Stuck HIGH (no transmission)
          [ ] Stuck LOW (short or wrong init)
```

**Step 5: Loop test**
- Connect TX pin to RX pin (serial loopback)
- Transmit character, should receive same character
- If works: Cable or terminal issue
- If doesn't work: ACIA or software issue

### Symptom: Garbage Characters Received

**Possible causes:**
- Baud rate mismatch
- Framing error (wrong data/stop bit config)
- Noise or signal integrity

**Diagnostic procedure:**

**Step 1: Verify baud rate**
- Try different baud rates in terminal (115200, 57600, 38400, 19200, 9600, etc.)
- If characters suddenly correct at specific rate: That's the actual baud rate

**Step 2: Check data format**
- Try 8-N-1, 8-E-1, 7-E-1, etc.
- Most systems use 8-N-1 (8 data bits, no parity, 1 stop bit)

**Step 3: Check clock**
- ACIA derives baud from clock input
- Verify clock frequency correct (usually 1.8432 MHz for standard baud rates)
- Measure clock input to ACIA with oscilloscope

**Step 4: Check for noise**
- Scope TX signal, look for clean square waves
- Ringing, overshoot, or glitches: Reduce baud rate or improve signal integrity
- Add termination resistor if needed

---

## Intermittent Faults

### Symptom: System Works Sometimes, Fails Other Times

**Possible causes:**
- Thermal issue (component fails when hot)
- Cold solder joint (connection breaks with vibration/temp)
- Marginal power supply
- Timing issue (race condition)

**Diagnostic procedure:**

**Step 1: Thermal correlation**
- Does failure happen after warm-up? → Thermal issue
  - Use freeze spray or heat gun to isolate chip
  - Spray candidate chip, if problem goes away → That chip is culprit
- Does failure happen immediately? → Not thermal

**Step 2: Physical stress test**
- Gently flex PCB, tap components (gently!)
- If failure appears/disappears with physical stress: **Cold solder joint**
  - Inspect suspect area under magnification
  - Reflow (re-heat) all joints in area
  - Check for cracked solder joints

**Step 3: Reseat ICs**
- Remove all ICs, re-insert firmly
- Oxidation on socket/IC pins can cause intermittent contact
- Clean pins with isopropyl alcohol if oxidized

**Step 4: Timing check**
- Measure all clock frequencies (ensure within spec)
- Check setup/hold times with oscilloscope
- If marginal timing: Reduce clock speed to verify

---

## Testing Procedures

### Comprehensive Power-On Test

**Step 1: Visual inspection**
- [ ] No smoke, no burning smell
- [ ] No hot components (touch test after 30 seconds)
- [ ] Power LED illuminated

**Step 2: DC voltage test**
- [ ] +5V rail: 4.75V - 5.25V
- [ ] GND: 0V (reference)
- [ ] All IC VCC pins: ~5V
- [ ] All IC GND pins: 0V

**Step 3: Clock test**
- [ ] PHI2 clock present at CPU
- [ ] Frequency correct: _______ Hz (expected: _______ Hz)
- [ ] Clean square wave (10-90% rise time <20% period)

**Step 4: Reset test**
- [ ] RES pin HIGH during normal operation
- [ ] RES pin goes LOW when reset button pressed
- [ ] System restarts after reset released

**Step 5: Bus activity test (logic analyzer)**
- [ ] Address bus changing (CPU fetching instructions)
- [ ] Data bus activity present
- [ ] RW signal toggles

**Step 6: Memory test**
- [ ] RAM write/read test passes
- [ ] ROM read test (read reset vector): $FFFC-$FFFD = $________

**Step 7: Functional test**
- [ ] Monitor prompt appears (or other expected boot message)
- [ ] Keyboard/input responds
- [ ] Display shows output
- [ ] Serial communication works

### ROM Test

**Reading ROM:**
```assembly
; Quick ROM test: Read reset vector
LDA $FFFC     ; Low byte of reset vector
STA $0800     ; Store to RAM for inspection
LDA $FFFD     ; High byte
STA $0801

; Full ROM checksum (example for HI ROM)
LDX #$00      ; Start at $C000
LDY #$C0
LDA #$00      ; Accumulator = checksum
Loop:
  CLC
  ADC $0000,X  ; Add ROM byte
  INX
  BNE Loop
  INY
  CPY #$00     ; Done when Y wraps to $00 (checked $C000-$FFFF)
  BNE Loop
; A now contains checksum (compare to expected)
```

### RAM Test

**Walking bit test:**
```assembly
; Test address $0800 with walking bit pattern
LDX #$01      ; Start with bit 0
Loop:
  STX $0800   ; Write pattern
  LDA $0800   ; Read back
  CPX $0800   ; Compare (should be same as X)
  BNE Fail
  ASL X       ; Shift left (next bit)
  BNE Loop    ; Loop until all 8 bits tested
```

**Full RAM test:**
- Write pattern to all addresses
- Read back and verify
- Repeat with multiple patterns ($00, $FF, $55, $AA, random)

---

## Component Testing

### Testing ICs Out-of-Circuit

**RAM (Static RAM):**
1. Programmer or dedicated RAM tester
2. Write pattern, read back, verify
3. Test all addresses
4. If fails: Replace

**ROM/EEPROM:**
1. Read with programmer
2. Verify checksum matches original
3. If corrupted: Re-program
4. If won't program: Replace

**CPU:**
- Difficult to test in isolation (requires complete system)
- Best test: Swap with known-good CPU
- If system works with different CPU: Original CPU defective

**Logic ICs (74-series):**
- Test with logic probe and power supply
- Apply signals to inputs, observe outputs
- Compare to truth table in datasheet
- Example (74LS00 NAND gate):
  ```
  Inputs  | Output
  A    B  |  Y
  --------+-------
  0    0  |  1
  0    1  |  1
  1    0  |  1
  1    1  |  0    (NAND: only output low when both inputs high)
  ```

### Identifying Defective Components

**Common failure symptoms:**

| Component | Failure Mode | Symptom |
|-----------|-------------|---------|
| CPU       | Shorted | Gets very hot immediately |
| CPU       | Dead | No bus activity, no response |
| RAM       | Defective | Fails RAM test, specific address range |
| RAM       | Shorted data line | One bit always wrong |
| ROM       | Unprogrammed | System doesn't boot, random execution |
| ROM       | Corrupted | Boots partially, crashes at specific code |
| Oscillator | Dead | No clock signal |
| Decoupling cap | Shorted | Shorts power rail |
| Electrolytic cap | Reversed polarity | Gets hot, may explode! |
| VIA/ACIA | Dead | Doesn't respond to reads/writes |
| Solder joint | Cold | Intermittent, affected by temp/vibration |
| Trace | Broken | Open circuit between connected points |

---

## Advanced Diagnostics

### Using Logic Analyzer

**Capture reset sequence:**
1. Trigger on RESET rising edge
2. Capture address+data+RW for ~100 cycles
3. Verify:
   - CPU reads reset vector from $FFFC-$FFFD
   - PC loaded with vector value
   - Execution begins at reset address

**Capture program execution:**
1. Trigger on specific address (e.g., $E000)
2. Capture bus activity
3. Disassemble captured data (compare to expected code)

**Find bus conflicts:**
1. Capture address+data+all chip enable signals
2. Look for multiple CE signals active simultaneously
3. Identify conflicting devices

### Using Oscilloscope

**Check clock quality:**
```
Expected:
  Frequency: Matches crystal/oscillator spec
  Duty cycle: 40-60% (50% ideal)
  Rise time: <20% of period
  Overshoot: <10%
  Ringing: Minimal
```

**Check bus timing:**
```
Measure setup/hold times:
  Address setup before PHI2 rise: >20ns (typical)
  Data setup before PHI2 fall: >50ns (typical)
  Address hold after PHI2 fall: >10ns (typical)
```

---

## When to Seek Help

**Ask for help if:**
- Tried all applicable diagnostic procedures
- Swapped suspected bad components
- Still unable to isolate problem
- Need additional tools/expertise

**Where to get help:**
- GitHub Discussions (post photos, scope traces, description)
- 6502.org forum (active community)
- Vintage computer subreddits

**What to provide:**
- Clear description of problem
- Steps already taken
- Photos of PCB (both sides, high resolution)
- Schematic (if custom board)
- Scope traces or logic analyzer captures
- Multimeter measurements

---

**For more information, see:**
- [Getting Started Guide](GETTING_STARTED.md) - Assembly procedures
- [Assembly Guide](ASSEMBLY_GUIDE.md) - Soldering techniques
- [Architecture Guide](ARCHITECTURE.md) - System architecture details
- [Main README](../../README.md) - Project overview

---
