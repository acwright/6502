

## Remaining Bottlenecks (in order of impact)

### 1. `SerialCard::tick()` polls `SerialUSB1.available()` every cycle (~15-20% of tick cost)

This USB API call happens ~700K+ times/sec but data only arrives at ~11,520 bytes/sec max. Rate-limit it:

In SerialCard.h, add a counter:
```cpp
    bool txPending;
    uint8_t rxPollCounter;  // ADD THIS
```

In SerialCard.cpp, change the tick:
```cpp
uint8_t SerialCard::tick(uint32_t cpuFrequency) {
  // Rate-limit RX polling: check every 64 cycles (~15,625 checks/sec at 1MHz)
  if (++rxPollCounter >= 64) {
    rxPollCounter = 0;
    if (SerialUSB1.available()) {
      // ... existing RX handling ...
    }
  }

  // Handle transmitting data immediately (no baud rate delay)
  if (txPending) {
    // ... existing TX handling unchanged ...
  }
  
  return this->status & SC_STATUS_IRQ;
}
```

Initialize `rxPollCounter = 0` in `reset()`.

### 2. `GPIOCard::tick()` does ~10 virtual dispatch calls to no-op attachment methods

None of the attachments override `tick()` — they all use the base class empty `virtual void tick() {}`. But the GPIOCard still iterates all attachments (3 on Port A, 3 on Port B) doing vtable lookups, plus 4 more loops for `hasCA1Interrupt()` / `hasCA2Interrupt()` / `hasCB1Interrupt()` / `hasCB2Interrupt()`.

In GPIOCard.cpp, eliminate the attachment tick loops (they're all no-ops) and merge the interrupt check loops:

```cpp
uint8_t GPIOCard::tick(uint32_t cpuFrequency) {
  tickCounter++;
  
  // Update Timer 1
  if (T1_running && regT1C > 0) {
    regT1C--;
    if (regT1C == 0) {
      setIRQFlag(IRQ_T1);
      if (regACR & 0x40) {
        regT1C = regT1L;
      } else {
        T1_running = false;
      }
      if (regACR & 0x80) {
        regORB ^= 0x80;
      }
    }
  }
  
  // Update Timer 2
  if (T2_running && regT2C > 0) {
    regT2C--;
    if (regT2C == 0) {
      setIRQFlag(IRQ_T2);
      T2_running = false;
    }
  }
  
  // Check for attachment interrupts (combined loop, no tick — attachments are event-driven)
  for (uint8_t i = 0; i < portA_attachmentCount; i++) {
    if (portA_attachments[i]->hasCA1Interrupt()) setIRQFlag(IRQ_CA1);
    if (portA_attachments[i]->hasCA2Interrupt()) setIRQFlag(IRQ_CA2);
  }
  for (uint8_t i = 0; i < portB_attachmentCount; i++) {
    if (portB_attachments[i]->hasCB1Interrupt()) setIRQFlag(IRQ_CB1);
    if (portB_attachments[i]->hasCB2Interrupt()) setIRQFlag(IRQ_CB2);
  }
  
  return (regIFR & regIER & 0x7F) ? IRQ_IRQ : 0x00;
}
```

This eliminates 6 attachment tick virtual calls and 6 null checks, and removes null checks from the interrupt loops (counts are accurate from `attachToPort`).

### 3. No `-flto` (Link-Time Optimization) — biggest compiler-level win

The vrEmu6502 `tick()` calls your `read()`/`write()` via function pointers. Without LTO the compiler can't see across translation units. With LTO, GCC can devirtualize and inline the hot path.

In platformio.ini, add `-flto` to both environments:
```ini
build_flags = 
	-D DEVBOARD_1
	-D USB_TRIPLE_SERIAL
	-O2
	-flto
```

**Warning**: `-flto` occasionally causes issues with some libraries. If it fails to link, try `-flto=auto` instead, or drop it and keep the other changes.

### 4. `tick()` and IO card methods aren't in ITCM

On Teensy 4.1, code in flash goes through the FlexSPI cache. Hot-path code benefits from ITCM (zero wait-state). The `tick()` function in main.cpp is not marked `FASTRUN`:

```cpp
FASTRUN void tick() {
  // ... existing body ...
}
```

Also the vrEmu6502Tick function uses `__time_critical_func()` which is a **no-op** on non-Pico platforms (see vrEmu6502.c: `#define __time_critical_func(fn) fn`). To fix this, add a Teensy-specific override before the macro in vrEmu6502.c:

```c
#if PICO_BUILD
#include "pico/stdlib.h"
#elif defined(ARDUINO_TEENSY41)
#define __not_in_flash(x)
#define __time_critical_func(fn) __attribute__((section(".fastrun"))) fn
#else
#define __not_in_flash(x)
#define __time_critical_func(fn) fn
#endif
```

This puts `vrEmu6502Tick` and `vrEmu6502InstCycle` into ITCM RAM.

### 5. Increase batch size from 512 to 2048

At ~700kHz, each tick takes ~1.4µs. A batch of 512 is ~720µs. You can safely go to 2048 (~2.9ms per batch), which still gives `loop()` ~345 iterations/sec for USB polling — plenty responsive:

```cpp
  if (isRunning && !timerMode) {
    for (uint16_t i = 0; i < 2048; i++) {
      tick();
    }
  }
```

### 6. Remove `storageCard.tick()` and `ramCard` tick calls from the hot loop

These are confirmed no-ops (`return 0x00` inlined in the header), but the compiler still generates a call unless LTO is enabled. Remove them explicitly:

```cpp
FASTRUN void tick() {
  if (freqIndex != cachedCpuFreqIndex) {
    cachedCpuFrequency = (uint32_t)(1000000.0 / FREQ_PERIODS[freqIndex]);
    cachedCpuFreqIndex = freqIndex;
  }

  cpu.tick();

  // Tick only IO cards that do actual work
  uint8_t interrupt = 0x00;
  interrupt |= rtcCard.tick(cachedCpuFrequency);
  interrupt |= serialCard.tick(cachedCpuFrequency);
  interrupt |= gpioCard.tick(cachedCpuFrequency);
  interrupt |= soundCard.tick(cachedCpuFrequency);
  interrupt |= videoCard.tick(cachedCpuFrequency);
  
  if (interrupt & 0x40) { cpu.nmiTrigger(); }
  if (interrupt & 0x80) { cpu.irqTrigger(); }
  else { cpu.irqClear(); }
}
```

Do the same in `cpuTimerISR()` and `step()`.

---

## Expected Impact Summary

| Optimization | Estimated Gain |
|---|---|
| Rate-limit SerialCard RX polling | ~10-15% |
| Eliminate GPIOCard attachment tick vtable calls | ~5-10% |
| `-flto` (cross-TU inlining) | ~10-20% |
| FASTRUN on `tick()` + vrEmu6502Tick ITCM | ~5-10% |
| Increase batch to 2048 | ~3-5% |
| Remove no-op tick calls | ~2-3% |

Combined these should push you from ~700kHz to 1MHz or beyond. The `-flto` and SerialCard rate-limiting are likely the biggest individual wins.