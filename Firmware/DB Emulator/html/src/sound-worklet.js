// ---------------------------------------------------------------------------
// SID Audio Worklet Processor
// ---------------------------------------------------------------------------

const SAMPLE_RATE = 44100;
const SID_CLOCK   = 1000000; // ~1 MHz SID clock rate
const CYCLES_PER_SAMPLE = SID_CLOCK / SAMPLE_RATE; // ~22.68

// ADSR envelope states
const ENV_ATTACK  = 0;
const ENV_DECAY   = 1;
const ENV_SUSTAIN = 2;
const ENV_RELEASE = 3;
const ENV_IDLE    = 4;

// Rate tables — SID clock cycle thresholds between each envelope step
const ATTACK_RATES = [
  2, 8, 16, 24, 38, 56, 68, 80,
  100, 250, 500, 800, 1000, 3000, 5000, 8000,
];

const DECAY_RELEASE_RATES = [
  6, 24, 48, 72, 114, 168, 204, 240,
  300, 750, 1500, 2400, 3000, 9000, 15000, 24000,
];

// Sustain level lookup: 4-bit register → 8-bit level (matching TS emulator)
const SUSTAIN_LEVELS = [
  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
  0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
];

// Exponential counter period thresholds for decay/release
function exponentialPeriod(level) {
  if (level >= 0x5D) return 1;
  if (level >= 0x36) return 2;
  if (level >= 0x1A) return 4;
  if (level >= 0x0E) return 8;
  if (level >= 0x06) return 16;
  return 30;
}

class Voice {
  constructor() {
    this.freqLo = 0;
    this.freqHi = 0;
    this.pwLo = 0;
    this.pwHi = 0;
    this.control = 0;
    this.attackDecay = 0;
    this.sustainRelease = 0;

    this.phase = 0;
    this.envLevel = 0;          // 0-255 integer (matches TS emulator)
    this.envState = ENV_IDLE;
    this.envCounter = 0;        // SID cycle counter
    this.expCounter = 0;        // exponential sub-counter
    this.expPeriod = 1;
    this.noiseShift = 0x7FFFF8;
    this.prevGate = 0;
  }

  get frequency() {
    return ((this.freqHi << 8) | this.freqLo);
  }

  get pulseWidth() {
    return ((this.pwHi & 0x0F) << 8) | this.pwLo;
  }

  get gate() { return this.control & 0x01; }
  get sync() { return (this.control >> 1) & 1; }
  get ring() { return (this.control >> 2) & 1; }
  get test() { return (this.control >> 3) & 1; }
  get waveTriangle() { return (this.control >> 4) & 1; }
  get waveSawtooth() { return (this.control >> 5) & 1; }
  get wavePulse() { return (this.control >> 6) & 1; }
  get waveNoise() { return (this.control >> 7) & 1; }

  get attackRate()  { return (this.attackDecay >> 4) & 0x0F; }
  get decayRate()   { return this.attackDecay & 0x0F; }
  get sustainLevelInt() { return SUSTAIN_LEVELS[(this.sustainRelease >> 4) & 0x0F]; }
  get releaseRate() { return this.sustainRelease & 0x0F; }

  // Clock the envelope by the number of SID cycles that one audio sample represents.
  // This mirrors the TS emulator's clockEnvelope() but batches ~22.68 cycles per call.
  updateEnvelope() {
    const gate = this.gate;

    // Gate transition detection
    if (gate && !this.prevGate) {
      this.envState = ENV_ATTACK;
      this.envCounter = 0;
      this.expCounter = 0;
      this.expPeriod = 1;
    } else if (!gate && this.prevGate) {
      this.envState = ENV_RELEASE;
      this.envCounter = 0;
    }
    this.prevGate = gate;

    // Advance the cycle counter by SID cycles per audio sample
    this.envCounter += CYCLES_PER_SAMPLE;

    switch (this.envState) {
      case ENV_ATTACK: {
        const rate = ATTACK_RATES[this.attackRate];
        while (this.envCounter >= rate) {
          this.envCounter -= rate;
          this.envLevel++;
          if (this.envLevel >= 0xFF) {
            this.envLevel = 0xFF;
            this.envState = ENV_DECAY;
            this.envCounter = 0;
            this.expCounter = 0;
            this.expPeriod = exponentialPeriod(this.envLevel);
            break;
          }
        }
        break;
      }
      case ENV_DECAY: {
        const rate = DECAY_RELEASE_RATES[this.decayRate];
        const sustain = this.sustainLevelInt;
        while (this.envCounter >= rate) {
          this.envCounter -= rate;
          this.expCounter++;
          if (this.expCounter >= this.expPeriod) {
            this.expCounter = 0;
            if (this.envLevel > sustain) {
              this.envLevel--;
              this.expPeriod = exponentialPeriod(this.envLevel);
            }
            if (this.envLevel <= sustain) {
              this.envLevel = sustain;
              this.envState = ENV_SUSTAIN;
              break;
            }
          }
        }
        break;
      }
      case ENV_SUSTAIN:
        this.envLevel = this.sustainLevelInt;
        break;
      case ENV_RELEASE: {
        const rate = DECAY_RELEASE_RATES[this.releaseRate];
        while (this.envCounter >= rate) {
          this.envCounter -= rate;
          this.expCounter++;
          if (this.expCounter >= this.expPeriod) {
            this.expCounter = 0;
            if (this.envLevel > 0) {
              this.envLevel--;
              this.expPeriod = exponentialPeriod(this.envLevel);
            }
          }
        }
        if (this.envLevel === 0) {
          this.envState = ENV_IDLE;
        }
        break;
      }
      case ENV_IDLE:
        this.envLevel = 0;
        break;
    }
  }

  generate() {
    // Must update envelope BEFORE the idle check so gate transitions are detected
    this.updateEnvelope();

    if (this.envState === ENV_IDLE) return 0;

    // Advance phase: SID uses a 24-bit phase accumulator at ~1 MHz
    // Map to 0-1 range for worklet at SAMPLE_RATE
    const freqVal = this.frequency;
    const phaseInc = (freqVal * 0.0596) / SAMPLE_RATE; // ~0.0596 Hz per freq unit
    this.phase += phaseInc;
    if (this.phase >= 1.0) this.phase -= 1.0;

    let sample = 0;
    let waveCount = 0;

    if (this.waveTriangle) {
      // Triangle: 0→1→0 over one cycle
      const tri = this.phase < 0.5 ? this.phase * 4 - 1 : 3 - this.phase * 4;
      sample += tri;
      waveCount++;
    }

    if (this.waveSawtooth) {
      const saw = this.phase * 2 - 1;
      sample += saw;
      waveCount++;
    }

    if (this.wavePulse) {
      const pw = this.pulseWidth / 4095;
      const pulse = this.phase < pw ? 1 : -1;
      sample += pulse;
      waveCount++;
    }

    if (this.waveNoise) {
      // Simple LFSR noise
      if (this.phase < phaseInc) {
        const bit0 = this.noiseShift & 1;
        const bit1 = (this.noiseShift >> 2) & 1;
        this.noiseShift = (this.noiseShift >> 1) | ((bit0 ^ bit1) << 22);
      }
      sample += ((this.noiseShift & 0xFF) / 127.5) - 1;
      waveCount++;
    }

    if (waveCount > 1) sample /= waveCount;

    return sample * (this.envLevel / 255);
  }
}

class SoundProcessor extends AudioWorkletProcessor {
  constructor() {
    super();
    this.voices = [new Voice(), new Voice(), new Voice()];
    this.filterCutoff = 0;
    this.filterResonance = 0;
    this.filterMode = 0;
    this.volume = 15;

    this.port.onmessage = (e) => this.handleMessage(e.data);
  }

  handleMessage(msg) {
    if (msg.type === 'reset') {
      this.voices.forEach(v => {
        v.freqLo = 0; v.freqHi = 0;
        v.pwLo = 0; v.pwHi = 0;
        v.control = 0;
        v.attackDecay = 0; v.sustainRelease = 0;
        v.phase = 0; v.envLevel = 0;
        v.envState = ENV_IDLE;
        v.envCounter = 0; v.expCounter = 0; v.expPeriod = 1;
        v.prevGate = 0;
      });
      this.volume = 15;
      return;
    }

    if (msg.type !== 'write') return;

    const { reg, val } = msg;

    // Map SID registers to voice state
    // Voice 1: $00-$06, Voice 2: $07-$0D, Voice 3: $0E-$14
    if (reg <= 0x14) {
      const voiceIdx = Math.floor(reg / 7);
      const voiceReg = reg % 7;
      const voice = this.voices[voiceIdx];
      if (!voice) return;

      switch (voiceReg) {
        case 0: voice.freqLo = val; break;
        case 1: voice.freqHi = val; break;
        case 2: voice.pwLo = val; break;
        case 3: voice.pwHi = val; break;
        case 4: voice.control = val; break;
        case 5: voice.attackDecay = val; break;
        case 6: voice.sustainRelease = val; break;
      }
    } else {
      // Filter & volume registers
      switch (reg) {
        case 0x15: this.filterCutoff = (this.filterCutoff & 0x7F8) | (val & 0x07); break;
        case 0x16: this.filterCutoff = (this.filterCutoff & 0x07) | (val << 3); break;
        case 0x17: this.filterResonance = (val >> 4) & 0x0F; break;
        case 0x18: this.volume = val & 0x0F; this.filterMode = (val >> 4) & 0x07; break;
      }
    }
  }

  process(inputs, outputs) {
    const output = outputs[0];
    const channel = output[0];
    if (!channel) return true;

    const vol = this.volume / 15;

    for (let i = 0; i < channel.length; i++) {
      let mix = 0;
      for (let v = 0; v < 3; v++) {
        mix += this.voices[v].generate();
      }
      // Mix 3 voices, apply volume, scale down to avoid clipping
      channel[i] = (mix / 3) * vol * 0.8;
    }

    return true;
  }
}

registerProcessor('sound-processor', SoundProcessor);
