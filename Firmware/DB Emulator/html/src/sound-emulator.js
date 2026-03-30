// ---------------------------------------------------------------------------
// SID Sound Emulator — main thread controller
// ---------------------------------------------------------------------------

let audioCtx = null;
let workletNode = null;
let _muted = true;

export async function init() {
  if (audioCtx) return;

  audioCtx = new AudioContext({ sampleRate: 44100 });

  // AudioWorklet must load from a separate file (not bundled)
  // Dev server serves source directly; production build outputs to /assets/
  const workletUrl = import.meta.env.DEV ? '/src/sound-worklet.js' : '/assets/sound-worklet.js';
  await audioCtx.audioWorklet.addModule(workletUrl);
  workletNode = new AudioWorkletNode(audioCtx, 'sound-processor');

  if (!_muted) {
    workletNode.connect(audioCtx.destination);
  }

  // Resume context (browsers require user gesture)
  if (audioCtx.state === 'suspended') {
    await audioCtx.resume();
  }
}

export function onRegWrite(reg, val) {
  if (!workletNode) return;
  workletNode.port.postMessage({ type: 'write', reg, val });
}

export function onReset() {
  if (!workletNode) return;
  workletNode.port.postMessage({ type: 'reset' });
}

export function getMuted() {
  return _muted;
}

export function setMuted(val) {
  _muted = !!val;
  if (!workletNode || !audioCtx) return;
  if (_muted) {
    try { workletNode.disconnect(audioCtx.destination); } catch (_) { /* ignore */ }
  } else {
    try { workletNode.connect(audioCtx.destination); } catch (_) { /* ignore */ }
  }
}

