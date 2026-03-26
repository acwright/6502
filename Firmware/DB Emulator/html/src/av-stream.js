// ---------------------------------------------------------------------------
// AV Stream — Web Serial connection to Teensy SerialUSB2
// ---------------------------------------------------------------------------

const AV_SYNC           = 0xAA;
const AV_TYPE_SOUND     = 0x01;
const AV_TYPE_VIDEO_DATA = 0x02;
const AV_TYPE_VIDEO_REG = 0x03;
const AV_TYPE_VIDEO_ADDR = 0x04;
const AV_TYPE_RESET     = 0xFE;

const PACKET_SIZE = 4;

let port = null;
let reader = null;
let readLoopActive = false;

export function isSupported() {
  return 'serial' in navigator;
}

export function isSecureContext() {
  return window.isSecureContext;
}

export function isSupportedBrowser() {
  return /Chrome\/|Edg\//.test(navigator.userAgent);
}

export function isConnected() {
  return port !== null && readLoopActive;
}

export async function connect() {
  port = await navigator.serial.requestPort();
  await port.open({ baudRate: 480000 });
}

export async function disconnect() {
  readLoopActive = false;

  if (reader) {
    try { await reader.cancel(); } catch (_) { /* ignore */ }
    reader = null;
  }

  if (port) {
    try { await port.close(); } catch (_) { /* ignore */ }
    port = null;
  }
}

export async function startReading({ onSoundWrite, onVideoDataWrite, onVideoRegWrite, onVideoAddrSet, onReset }) {
  if (!port || !port.readable) return;

  readLoopActive = true;
  const buf = [];

  while (port.readable && readLoopActive) {
    reader = port.readable.getReader();
    try {
      while (readLoopActive) {
        const { value, done } = await reader.read();
        if (done) break;

        for (let i = 0; i < value.length; i++) {
          buf.push(value[i]);
        }

        // Process all complete packets in the buffer
        while (buf.length >= PACKET_SIZE) {
          // Align on sync byte
          if (buf[0] !== AV_SYNC) {
            buf.shift();
            continue;
          }

          const type = buf[1];
          const a    = buf[2];
          const b    = buf[3];
          buf.splice(0, PACKET_SIZE);

          switch (type) {
            case AV_TYPE_SOUND:
              onSoundWrite(a, b);
              break;
            case AV_TYPE_VIDEO_DATA:
              onVideoDataWrite(b);
              break;
            case AV_TYPE_VIDEO_REG:
              onVideoRegWrite(a, b);
              break;
            case AV_TYPE_VIDEO_ADDR:
              onVideoAddrSet(a, b);
              break;
            case AV_TYPE_RESET:
              onReset();
              break;
          }
        }
      }
    } catch (e) {
      if (readLoopActive) console.error('AV stream read error:', e);
    } finally {
      try { reader.releaseLock(); } catch (_) { /* ignore */ }
      reader = null;
    }
  }
}
