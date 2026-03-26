---
description: "Implement SID sound (IO slot 6) and TMS9918A video (IO slot 7) emulation on the Teensy 4.1 firmware, streaming register writes over SerialUSB2 to the web app via the Web Serial API for browser-side audio synthesis and canvas rendering. Merges the Emulator and CPU web UI panels into a single SYSTEM panel and adds a new VIDEO panel."
name: "AV Streaming Plan"
agent: "agent"
---

Implement the SID sound card (IO slot 6) and TMS9918A video display card (IO slot 7) for the DB Emulator project, streaming their register writes over `SerialUSB2` to the web application for browser-side audio and video rendering.

Read the following project files before starting to understand the current architecture:

- [platformio.ini](../../platformio.ini)
- [src/main.cpp](../../src/main.cpp)
- [lib/6502/6502.h](../../lib/6502/6502.h)
- [lib/6502/constants.h](../../lib/6502/constants.h)
- [lib/6502/IO/IO.h](../../lib/6502/IO/IO.h)
- [lib/6502/IO/SerialCard.h](../../lib/6502/IO/SerialCard.h)
- [lib/6502/IO/SerialCard.cpp](../../lib/6502/IO/SerialCard.cpp)
- [html/index.html](../../html/index.html)
- [html/src/index.js](../../html/src/index.js)
- [html/src/index.css](../../html/src/index.css)
- [html/mock/api.mock.js](../../html/mock/api.mock.js)
- [html/vite.config.js](../../html/vite.config.js)
- [README.md](../../README.md)
- [html/README.md](../../html/README.md)

---

## Architecture

```
Teensy 4.1
├─ IO Slot 6 ($9800): SoundCard ──┐
├─ IO Slot 7 ($9C00): VideoCard ──┤── 4-byte binary packets → SerialUSB2 (USB CDC #3)
│
├─ Ethernet ──→ HTTP REST API ──→ Browser (control, memory, storage)
└─ USB CDC #3 (SerialUSB2) ────→ Browser Web Serial API
                                   ├─ SID register writes → AudioWorklet → Web Audio API
                                   └─ Video writes → VRAM mirror → <canvas>
```

**Binary packet protocol** — fixed 4-byte, sync-aligned:
- `[0xAA][type][addr/reg][value]`
- `0x01`: SID register write
- `0x02`: Video data port write (browser mirrors VRAM autoincrement)
- `0x03`: Video register write
- `0x04`: Video address set — `[0xAA][0x04][addr_hi][addr_lo]`
- `0xFE`: Reset all AV state

---

## Phase 1 — Firmware: USB Configuration

1. In [platformio.ini](../../platformio.ini), change `-D USB_DUAL_SERIAL` to `-D USB_TRIPLE_SERIAL` in both `[env:devboard_0]` and `[env:devboard_1]` build flag sections. This exposes `SerialUSB2` as a third USB CDC endpoint.

2. In [src/main.cpp](../../src/main.cpp) `setup()`, add `SerialUSB2.begin(115200);` immediately after the existing `SerialUSB1.begin(115200);` line.

---

## Phase 2 — Firmware: AV Stream Protocol Header

3. Create `lib/6502/IO/AVStream.h` defining:
   - Packet sync byte `AV_SYNC = 0xAA`
   - Type constants: `AV_TYPE_SOUND`, `AV_TYPE_VIDEO_DATA`, `AV_TYPE_VIDEO_REG`, `AV_TYPE_VIDEO_ADDR`, `AV_TYPE_RESET`
   - An inline `avSend(uint8_t type, uint8_t a, uint8_t b)` function that writes `[AV_SYNC, type, a, b]` to `SerialUSB2`

---

## Phase 3 — Firmware: SoundCard IO Class

Model this class on `SerialCard.h`/`.cpp` as a pattern reference.

4. Create `lib/6502/IO/SoundCard.h`:
   - Register definitions for the MOS 6581/8580 SID chip (29 write registers $00–$1C, 4 read registers $19–$1C)
   - Private members: `uint8_t registers[32]`
   - Inherits from `IO`

5. Create `lib/6502/IO/SoundCard.cpp`:
   - `write(addr, val)`: store `val` in `registers[addr & 0x1F]`, then call `avSend(AV_TYPE_SOUND, addr & 0x1F, val)`
   - `read(addr)`: return `registers[addr & 0x1F]` for readable registers ($19–$1C); return 0 otherwise
   - `tick(cpuFrequency)`: return `0x00` (SID generates no CPU interrupts; timing is browser-side)
   - `reset()`: `memset(registers, 0, sizeof(registers))`, then call `avSend(AV_TYPE_RESET, 0x00, 0x00)`

---

## Phase 4 — Firmware: VideoCard IO Class (TMS9918A)

6. Create `lib/6502/IO/VideoCard.h`:
   - Private: `uint8_t vram[16384]`, `uint8_t registers[8]`, `uint8_t status`, `uint16_t addr`, `uint8_t latch`, `bool firstByte`, `uint8_t readBuf`
   - Two memory-mapped ports: data ($00) and control/register ($01)
   - VDP generates a ~60 Hz vblank interrupt — include cycle counter members: `uint32_t cycleCount`, `uint32_t cyclesPerFrame`
   - Inherits from `IO`

7. Create `lib/6502/IO/VideoCard.cpp`:
   - `write($00)`: write `val` to `vram[addr & 0x3FFF]`, auto-increment `addr`, call `avSend(AV_TYPE_VIDEO_DATA, 0x00, val)`
   - `write($01)`: implement the standard TMS9918A two-byte address latch:
     - First byte: store as `latch`, set `firstByte = false`
     - Second byte: if bit 7 set → register write: store `latch` in `registers[val & 0x07]`, call `avSend(AV_TYPE_VIDEO_REG, val & 0x07, latch)`, else → set address: `addr = latch | ((val & 0x3F) << 8)`, call `avSend(AV_TYPE_VIDEO_ADDR, (addr >> 8) & 0x3F, addr & 0xFF)`. Reset `firstByte = true`.
   - `read($00)`: return `readBuf`, set `readBuf = vram[addr & 0x3FFF]`, auto-increment `addr`
   - `read($01)`: capture `uint8_t s = status`, clear bit 7 of status, return `s`
   - `tick(cpuFrequency)`: accumulate cycles; when `cycleCount >= cyclesPerFrame` (derived from `cpuFrequency / 60`), set status bit 7, reset counter, return `0x80` (IRQ). Otherwise return `0x00`.
   - `reset()`: `memset(vram, 0, sizeof(vram))`, `memset(registers, 0, sizeof(registers))`, zero all state, call `avSend(AV_TYPE_RESET, 0x00, 0x00)`

---

## Phase 5 — Firmware: main.cpp Integration

8. Add `#include "IO/SoundCard.h"` and `#include "IO/VideoCard.h"` to `lib/6502/6502.h`.

9. In [src/main.cpp](../../src/main.cpp):
   - Declare `SoundCard soundCard;` and `VideoCard videoCard;` alongside the other IO card globals
   - In `read()`, replace the `case 6:` and `case 7:` no-ops with calls to `soundCard.read()` and `videoCard.read()` respectively (same offset calculation pattern as the other slots)
   - In `write()`, do the same for `case 6:` and `case 7:`
   - In `tick()`, add `interrupt |= soundCard.tick(cachedCpuFrequency);` and `interrupt |= videoCard.tick(cachedCpuFrequency);` after the existing IO card tick calls
   - Do the same in `cpuTimerISR()` and `step()`
   - In the `reset()` function, add `soundCard.reset();` and `videoCard.reset();` after `cpu.reset()`
   - In `onServerInfo()`, add `doc["soundEnabled"] = true;`, `doc["videoEnabled"] = true;`, and `doc["avStreamConnected"] = (bool)SerialUSB2.dtr();` to the JSON document

---

## Phase 6 — Web App: UI Restructure

10. Modify [html/index.html](../../html/index.html) — change from 4 separate panels to a merged 2×2 layout:
    - **Top-Left: SYSTEM** — merge the existing Emulator and CPU panel contents into one panel. Use a two-column sub-layout: emulator status + info rows on the left column; CPU registers, flags, and bus indicators on the right column. Keep all existing element IDs intact.
    - **Top-Right: VIDEO** — new panel containing:
      - `<canvas id="video-canvas" width="256" height="192"></canvas>` centered in the panel
      - A `<div id="av-status" class="av-status-disconnected">DISCONNECTED</div>` indicator
      - A `<button id="av-connect" class="button button-green">Connect AV</button>` button
      - A `<button id="av-disconnect" class="button button-red" disabled>Disconnect</button>` button
      - A `<button id="sound-mute" class="button button-blue">Mute</button>` toggle
    - **Bottom-Left: MEMORY** — unchanged
    - **Bottom-Right: STORAGE** — unchanged

11. Modify [html/src/index.css](../../html/src/index.css):
    - Add `.system-columns` — `display: grid; grid-template-columns: 1fr 1fr; gap: 8px;` for the SYSTEM panel sub-layout
    - Add `.video-canvas-container` — `display: flex; justify-content: center; align-items: center; flex: 1;`
    - Add `#video-canvas` — `image-rendering: pixelated; max-width: 100%; max-height: 100%; aspect-ratio: 4/3; border: 1px solid #333;`
    - Add `.av-status-disconnected` — `color: #ff0000; font-weight: bold; text-align: center;`
    - Add `.av-status-connected` — `color: #00ff00; font-weight: bold; text-align: center;`
    - Add `.av-controls` — `display: flex; justify-content: center; gap: 8px; margin-top: 8px;`

---

## Phase 7 — Web App: Web Serial Connection Module

12. Create `html/src/av-stream.js` — exported singleton for the USB serial connection:
    - `connect()`: call `navigator.serial.requestPort()` (user gesture required), `port.open({ baudRate: 480000 })`, start read loop
    - `disconnect()`: cancel reader, close port
    - `startReading({ onSoundWrite, onVideoDataWrite, onVideoRegWrite, onVideoAddrSet, onReset })`: read loop that buffers incoming bytes, aligns on `0xAA` sync byte, parses 4-byte packets, dispatches to callbacks. Handle the `0x04` address set packet as 2 address bytes.
    - `isConnected()`: return boolean state
    - `isSupported()`: return `'serial' in navigator`

13. In [html/src/index.js](../../html/src/index.js):
    - Import `av-stream.js`, `video-renderer.js`, and `sound-emulator.js`
    - Cache the new DOM elements: `videoCanvas`, `avStatus`, `avConnect`, `avDisconnect`, `soundMute`
    - On page load, check `isSupported()` — if false, hide Connect button and show "Requires Chrome or Edge" in the VIDEO panel
    - Bind `avConnect` click → `avStream.connect()`, then call `videoRenderer.init(videoCanvas)` and `soundEmulator.init()`, update status indicator
    - Bind `avDisconnect` click → `avStream.disconnect()`, update status indicator
    - Bind `soundMute` click → toggle `soundEmulator.muted`
    - Pass stream callbacks to `avStream.startReading()` that forward to `videoRenderer` and `soundEmulator`

---

## Phase 8 — Web App: TMS9918A Canvas Renderer

14. Create `html/src/video-renderer.js`:
    - Internal state: `Uint8Array(16384)` for VRAM, `Uint8Array(8)` for registers, `vramAddr = 0`
    - `init(canvas)`: store canvas reference, get `ctx = canvas.getContext('2d')`, start `requestAnimationFrame` render loop
    - `onDataWrite(val)`: `vram[vramAddr & 0x3FFF] = val; vramAddr = (vramAddr + 1) & 0x3FFF;`
    - `onRegWrite(reg, val)`: `registers[reg & 7] = val`
    - `onAddrSet(hi, lo)`: `vramAddr = ((hi & 0x3F) << 8) | lo`
    - `onReset()`: zero VRAM and registers
    - `render()`: determine video mode from registers (bits in R1: M1, M2, M3 determine mode), dispatch to the correct render function, then call `requestAnimationFrame(render)` to loop
    - Implement `renderGraphicsI()`, `renderGraphicsII()`, `renderText()`, `renderMulticolor()`
    - Implement sprite rendering: read sprite attribute table and pattern table from VRAM, render up to 4 sprites per scanline (per TMS9918A spec limit)
    - TMS9918A 16-color palette as a lookup array of `[r, g, b]` values
    - All rendering via `ImageData` and `ctx.putImageData()` for performance

---

## Phase 9 — Web App: SID Audio Emulation

15. Create `html/src/sound-worklet.js` — `AudioWorkletProcessor` named `"sound-processor"`:
    - Maintain internal SID state: 3 voices (frequency, pulse width, waveform control, ADSR), filter registers (cutoff, resonance, mode), volume
    - Handle `message` events from the main thread: `{ type: 'write', reg, val }` updates the corresponding SID register state
    - `process()`: generate audio samples — per voice: advance oscillator phase based on frequency register, select waveform (triangle/sawtooth/pulse/noise), apply ADSR envelope, mix through filter model, sum all voices scaled by master volume, output to `AudioContext` output buffer

16. Create `html/src/sound-emulator.js`:
    - `init()`: create `AudioContext`, call `audioCtx.audioWorklet.addModule('./sound-worklet.js')` (note: must be a separate file, not bundled), create `AudioWorkletNode`, connect to `audioCtx.destination`
    - `onRegWrite(reg, val)`: post `{ type: 'write', reg, val }` to the worklet via `node.port.postMessage()`
    - `onReset()`: post `{ type: 'reset' }` to worklet
    - `muted` property that disconnects/reconnects the worklet node from the destination

17. In [html/vite.config.js](../../html/vite.config.js), add `sound-worklet.js` as an additional entry point under `rollupOptions.input` so it is emitted as a separate file (AudioWorklet cannot be loaded from a bundled chunk):
    ```js
    input: {
      main: './index.html',
      'sound-worklet': './src/sound-worklet.js',
    }
    ```

---

## Phase 10 — Browser Keyboard Input

18. In [src/main.cpp](../../src/main.cpp):
    - Add forward declaration `void onServerKeyboard(AsyncWebServerRequest *request);` alongside the other server handler declarations at the top of the file
    - In the server route registration block, add `server.on("/keyboard", HTTP_GET, onServerKeyboard);` alongside the existing routes
    - Implement `onServerKeyboard()`:
      - Read the `action` query param: must be `"down"` or `"up"`; return `400` if missing or invalid
      - Read the `keycode` query param: a 1–2 character hex string representing a USB HID keycode (e.g. `"04"` for 'a'); return `400` if missing
      - Convert with `uint8_t kc = (uint8_t)strtoul(request->getParam("keycode")->value().c_str(), nullptr, 16)`
      - If `action == "down"`, call `onKeyDown(kc)`; if `action == "up"`, call `onKeyUp(kc)`
      - Return `200` on success

19. In [html/src/index.js](../../html/src/index.js):
    - Add a `USB_HID_KEYMAP` constant object mapping `KeyboardEvent.code` strings to USB HID keycode numbers. Cover at minimum: `KeyA`–`KeyZ` (0x04–0x1D), `Digit1`–`Digit9` (0x1E–0x26), `Digit0` (0x27), `Enter` (0x28), `Escape` (0x29), `Backspace` (0x2A), `Tab` (0x2B), `Space` (0x2C), `Minus` (0x2D), `Equal` (0x2E), `BracketLeft` (0x2F), `BracketRight` (0x30), `Backslash` (0x31), `Semicolon` (0x33), `Quote` (0x34), `Backquote` (0x35), `Comma` (0x36), `Period` (0x37), `Slash` (0x38), `CapsLock` (0x39), `F1`–`F12` (0x3A–0x45), `ArrowRight` (0x4F), `ArrowLeft` (0x50), `ArrowDown` (0x51), `ArrowUp` (0x52), `ShiftLeft` (0xE1), `ShiftRight` (0xE5), `ControlLeft` (0xE0), `ControlRight` (0xE4), `AltLeft` (0xE2), `AltRight` (0xE6), `MetaLeft` (0xE3), `MetaRight` (0xE7)
    - Add a helper `sendKeyEvent(action, code)` that:
      - Looks up `USB_HID_KEYMAP[code]` — if not found, returns immediately (unmapped key, no request sent)
      - Calls `fetch('/keyboard?action=' + action + '&keycode=' + hid.toString(16).padStart(2, '0'))` as a fire-and-forget (no `await`)
    - Attach a `keydown` listener on `document`: call `sendKeyEvent('down', event.code)`, and call `event.preventDefault()` for Tab, F1–F12, Backspace, Space, arrow keys, and Escape (to prevent browser default behaviour during emulation)
    - Attach a `keyup` listener on `document`: call `sendKeyEvent('up', event.code)`

---

## Phase 11 — Web App: Mock Updates

20. In [html/mock/api.mock.js](../../html/mock/api.mock.js), add `soundEnabled: true`, `videoEnabled: true`, and `avStreamConnected: false` to the `/info` response body. Also add a handler for `GET /keyboard` that returns HTTP 200 (no-op stub so keyboard events do not produce network errors during `npm run dev`).

21. Create `html/mock/av-stream-mock.js` — a development utility (not part of the production bundle) that simulates AV stream events for testing without hardware. Export a `startMock(callbacks)` function that uses `setInterval` to emit:
    - A sequence of TMS9918A register writes that set up Graphics II mode
    - A repeating pattern of VRAM writes that draw test content on the canvas
    - A sequence of SID register writes that produce a simple repeating tone

---

## Phase 12 — Documentation

22. Update [README.md](../../README.md):
    - In **Features / I/O Support**: replace "Sound Card (Not emulated)" with "Sound Card (SID — MOS 6581 emulation, streamed to web interface)" and "Video Card (Not emulated)" with "Video Card (TMS9918A emulation, streamed to web interface)"
    - In **Prerequisites / Software Requirements**: add "Chromium-based browser (Chrome or Edge) required for audio/video streaming via Web Serial API"
    - In **Build Flags**: change all references to `USB_DUAL_SERIAL` to `USB_TRIPLE_SERIAL`
    - In **Memory Map**: update IO Slot 6 to "Sound Card (SID)" and IO Slot 7 to "Video Card (TMS9918A)"
    - In **Usage / Web Control Interface**: add a "**Audio/Video Streaming**" subsection explaining:
      - The Teensy must be USB-connected to the same machine running the browser
      - Click "Connect AV" in the VIDEO panel to open the USB serial link
      - Requires Chrome or Edge (Web Serial API); requires HTTPS or localhost
      - Audio and video are rendered entirely in the browser; the Teensy streams register writes only
    - In **API Endpoints / GET /info**: add documentation for the new `soundEnabled`, `videoEnabled`, and `avStreamConnected` fields
    - In **Troubleshooting**: add entries for:
      - "Connect AV button not visible" → must use Chrome or Edge; page must be served over HTTPS or from localhost
      - "No video or audio after connecting" → verify USB cable supports data, check SerialUSB2 initialized (USB_TRIPLE_SERIAL flag set), try reconnecting
      - "Audio glitches or dropouts" → lower CPU frequency, ensure browser tab is not throttled/backgrounded

23. Update [html/README.md](../../html/README.md):
    - In **Overview**: add "TMS9918A video display on canvas" and "SID audio synthesis via Web Audio API" to the feature list
    - In **Project Structure**: add entries for `av-stream.js`, `video-renderer.js`, `sound-emulator.js`, `sound-worklet.js`, and `mock/av-stream-mock.js`
    - In **Prerequisites**: add "Chromium-based browser (Chrome or Edge) for AV streaming features"
    - In **API Endpoints**: add a note that AV data flows via Web Serial API, not REST
    - Add a new section "**AV Streaming (Video & Sound)**" covering:
      - Dual-channel architecture: Ethernet for REST control, USB serial for real-time AV
      - Binary packet format: 4-byte fixed-width with `0xAA` sync
      - Browser requirements: Web Serial API (Chrome/Edge only), secure context (HTTPS or localhost), one-time user gesture for port selection
      - Development workflow: enable mock in `av-stream-mock.js` during `npm run dev` to test rendering and audio without hardware
      - AudioWorklet constraint: `sound-worklet.js` must be served as a separate file — Vite is configured with a dedicated rollup entry for it

---

## Verification Checklist

After completing all phases, verify:

- [ ] `platformio run` compiles successfully on both `devboard_0` and `devboard_1`
- [ ] Writing to $9800–$981C emits correct SID packets on `SerialUSB2` (verify with a serial terminal)
- [ ] Writing to $9C00–$9C01 emits correct Video packets on `SerialUSB2`
- [ ] Video card fires an IRQ at ~60 Hz proportional to `cpuFrequency`; CPU services the interrupt
- [ ] `npm run dev` loads with the merged SYSTEM panel and VIDEO canvas panel rendering correctly
- [ ] "Connect AV" button opens a serial port picker; selecting the Teensy shows green CONNECTED status
- [ ] Disconnect is clean (no console errors; reconnect works)
- [ ] Loading a TMS9918A-writing ROM produces visible output on the canvas
- [ ] Loading a SID-writing ROM produces audible output in the browser
- [ ] `npm run build` produces valid `dist/` output with `sound-worklet.js` as a separate asset
- [ ] Copying `dist/` to the SD card `WWW/` folder and serving from Teensy works end-to-end
- [ ] Browser `keydown`/`keyup` events on the web page are forwarded to the firmware via `GET /keyboard`
- [ ] A key press in the browser is received by `onKeyDown()` / `onKeyUp()` in `main.cpp` and routed to `keyboardMatrixAttachment` and `keyboardEncoderAttachment`
- [ ] Invalid or missing `action`/`keycode` params return HTTP 400
- [ ] Unmapped `KeyboardEvent.code` values produce no network request
- [ ] Tab, F-keys, arrows, Backspace, and Escape are prevented from triggering browser defaults while in the emulator UI
- [ ] All README sections reflect the actual implemented behavior

---

## Key Design Decisions

- **USB CDC over WebSocket**: lower latency than Ethernet, does not compete with the existing HTTP server
- **Browser-side rendering/synthesis**: keeps Teensy CPU budget for 6502 emulation; only register writes are transmitted
- **4-byte fixed packets**: worst-case ~400 KB/s at 1 MHz, well within USB 2.0 HS capacity
- **VRAM dual storage**: Teensy holds VRAM for CPU read-back; browser mirrors it for rendering
- **Merged SYSTEM panel**: keeps the 2×2 grid balanced while making room for VIDEO
- **HTTPS constraint**: Web Serial requires a secure context; document that the browser must be on the same machine as the Teensy is USB-connected to (which is the natural local-use scenario)
- **TMS9918A scope**: implement Graphics I, II, Text, Multicolor + basic sprites first; iterate on sprite collision, 5th-sprite-flag, and interrupt timing accuracy based on ROM compatibility testing
- **SID implementation**: evaluate existing JS SID libraries (jsSID, WebSID, resid-js) before writing from scratch — the SID filter analog model is extremely complex; a 3-voice + ADSR simplified implementation is the fallback MVP
- **Keyboard over HTTP GET**: browser keyboard events are forwarded via the existing Ethernet REST API (`GET /keyboard?action=down&keycode=XX`) rather than through SerialUSB2. This reuses the established HTTP server infrastructure, keeps the serial channel unidirectional (firmware → browser only), and avoids any USB serial latency penalty for keystroke delivery. USB HID keycodes are transmitted so `onKeyDown()`/`onKeyUp()` can reuse the existing attachment routing unchanged.
