// ---------------------------------------------------------------------------
// AV Stream Mock — development utility for testing without hardware
// ---------------------------------------------------------------------------
// Usage: import { startMock } from './av-stream-mock.js'
// Call startMock(callbacks) with the same callback shape as av-stream.startReading()
// Returns a stop() function to cancel the mock.

export function startMock({ onSoundWrite, onVideoDataWrite, onVideoRegWrite, onVideoAddrSet, onReset }) {
  let frame = 0;

  // Set up TMS9918A Graphics II mode registers
  const setupVideo = () => {
    onReset();

    // R0: M3 bit set (Graphics II)
    onVideoRegWrite(0, 0x02);
    // R1: 16K VRAM, display enabled, interrupt enabled, 16×16 sprites
    onVideoRegWrite(1, 0xC2);
    // R2: Name table at $3800
    onVideoRegWrite(2, 0x0E);
    // R3: Color table at $2000 (bit 7 set for GII)
    onVideoRegWrite(3, 0x80);
    // R4: Pattern table at $0000 (bit 2 set for GII)
    onVideoRegWrite(4, 0x04);
    // R5: Sprite attribute table at $3B00
    onVideoRegWrite(5, 0x76);
    // R6: Sprite pattern table at $1800
    onVideoRegWrite(6, 0x03);
    // R7: Backdrop = dark blue (4)
    onVideoRegWrite(7, 0xF4);
  };

  // Write a test pattern into VRAM
  const writeTestPattern = () => {
    // Write pattern table — simple checkerboard pattern for first few chars
    onVideoAddrSet(0x00, 0x00);
    for (let ch = 0; ch < 32; ch++) {
      for (let row = 0; row < 8; row++) {
        onVideoDataWrite(row % 2 === 0 ? 0xAA : 0x55);
      }
    }

    // Write color table — alternating colors
    onVideoAddrSet(0x20, 0x00);
    for (let ch = 0; ch < 32; ch++) {
      for (let row = 0; row < 8; row++) {
        const fg = ((ch + frame) % 14) + 1;
        const bg = 1; // black
        onVideoDataWrite((fg << 4) | bg);
      }
    }

    // Write name table — fill with character indices
    onVideoAddrSet(0x38, 0x00);
    for (let row = 0; row < 24; row++) {
      for (let col = 0; col < 32; col++) {
        onVideoDataWrite((col + row + frame) % 32);
      }
    }
  };

  // Send a simple repeating tone via SID registers
  const writeSoundTone = () => {
    // Voice 1: ~440 Hz tone, triangle wave
    // Frequency = (440 * 16777216) / 1000000 ≈ 7382
    const freq = 7382 + Math.floor(Math.sin(frame * 0.1) * 500);
    onSoundWrite(0x00, freq & 0xFF);         // Freq Lo
    onSoundWrite(0x01, (freq >> 8) & 0xFF);  // Freq Hi
    onSoundWrite(0x05, 0x09);                // Attack=0, Decay=9
    onSoundWrite(0x06, 0xA0);                // Sustain=10, Release=0
    onSoundWrite(0x04, 0x11);                // Triangle + Gate
    onSoundWrite(0x18, 0x0F);                // Max volume
  };

  // BIOS-style beep: mirrors the exact register writes from BeepImpl/SidPlayNote
  const biosBeep = () => {
    console.log('[MOCK] Playing BIOS-style beep');
    onSoundWrite(0x18, 0x0F);                // Volume = max (InitSIDImpl)
    onSoundWrite(0x00, 0x20);                // Voice 1 Freq Lo ($1F20 ≈ 1000 Hz)
    onSoundWrite(0x01, 0x1F);                // Voice 1 Freq Hi
    onSoundWrite(0x05, 0x09);                // Attack=0, Decay=9
    onSoundWrite(0x06, 0x00);                // Sustain=0, Release=0 (beep override)
    onSoundWrite(0x04, 0x11);                // Triangle + Gate ON

    // Gate off after ~200ms (simulates BIOS delay loop)
    setTimeout(() => {
      onSoundWrite(0x04, 0x10);              // Triangle, Gate OFF (SidSilence)
      onSoundWrite(0x00, 0x00);              // Freq Lo = 0
      onSoundWrite(0x01, 0x00);              // Freq Hi = 0
      console.log('[MOCK] Beep ended');
    }, 200);
  };

  setupVideo();
  writeTestPattern();
  biosBeep();                   // Immediate one-shot beep for sound debugging

  const intervalId = setInterval(() => {
    frame++;
    writeTestPattern();
  }, 1000 / 60);

  return function stop() {
    clearInterval(intervalId);
  };
}
