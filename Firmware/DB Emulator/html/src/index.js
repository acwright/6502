import './index.css'
import * as avStream from './av-stream.js'
import * as videoRenderer from './video-renderer.js'
import * as soundEmulator from './sound-emulator.js'
// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

const RAM_PAGES  = 128;  // $0000-$7FFF  (0x8000 / 0x100)
const ROM_PAGES  = 256;  // $8000-$FFFF  (0x10000 - 0x8000) / 0x100
const PAGE_SIZE  = 256;  // bytes per memory page
const FILE_PER_PAGE  = 8;
const STORAGE_PAGES  = 32;  // FILE_MAX (0x100) / FILE_PER_PAGE (8)
const PAGINATION_WINDOW = 5;

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

const state = {
  // Memory panel
  memoryTarget: 'ram',   // 'ram' | 'rom'
  memoryPage: 0,

  // Storage panel
  storageTarget: 'rom',  // 'rom' | 'cart' | 'program'
  storagePage: 0,
};

// ---------------------------------------------------------------------------
// API helpers
// ---------------------------------------------------------------------------

async function api(path, params = {}) {
  const url = new URL(path, window.location.origin);
  Object.entries(params).forEach(([k, v]) => url.searchParams.set(k, v));
  const res = await fetch(url.toString());
  if (!res.ok) throw new Error(`API ${res.status}`);
  return res;
}

async function fetchInfo() {
  const res = await api('/info');
  return res.json();
}

async function sendControl(command) {
  await api('/control', { command });
}

async function fetchMemory(target, page) {
  const res = await api('/memory', { target, page });
  const buf = await res.arrayBuffer();
  return new Uint8Array(buf);
}

async function fetchStorage(target, page) {
  const res = await api('/storage', { target, page });
  return res.json();
}

async function loadFile(target, filename) {
  await api('/load', { target, filename });
}

// ---------------------------------------------------------------------------
// DOM references (cached once on init)
// ---------------------------------------------------------------------------

const $ = (id) => document.getElementById(id);

const dom = {};

function cacheDom() {
  // Header buttons
  dom.refresh            = $('refresh');
  dom.snapshot           = $('snapshot');
  dom.decreaseFreq       = $('decrease-frequency');
  dom.increaseFreq       = $('increase-frequency');
  dom.run                = $('run');
  dom.stop               = $('stop');
  dom.step               = $('step');
  dom.tick               = $('tick');
  dom.reset              = $('reset');

  // Emulator panel
  dom.isRunning          = $('is-running');
  dom.programFile        = $('program-file');
  dom.romFile            = $('rom-file');
  dom.cartFile           = $('cart-file');
  dom.freqLabel          = $('freq-label');
  dom.freqPeriod         = $('freq-period');
  dom.ipAddress          = $('ip-address');
  dom.rtc                = $('rtc');
  dom.lastSnapshot       = $('last-snapshot');
  dom.unloadCart         = $('unload-cart');

  // CPU panel
  dom.cpuAccHex          = $('cpu-accumulator-hex');
  dom.cpuAccDec          = $('cpu-accumulator-dec');
  dom.cpuXHex            = $('cpu-x-hex');
  dom.cpuXDec            = $('cpu-x-dec');
  dom.cpuYHex            = $('cpu-y-hex');
  dom.cpuYDec            = $('cpu-y-dec');
  dom.cpuPCHex           = $('cpu-pc-hex');
  dom.cpuPCDec           = $('cpu-pc-dec');
  dom.cpuSPHex           = $('cpu-sp-hex');
  dom.cpuSPDec           = $('cpu-sp-dec');
  dom.cpuStatusHex       = $('cpu-status-hex');
  dom.cpuStatusDec       = $('cpu-status-dec');
  dom.flagN              = $('cpu-flag-negative');
  dom.flagV              = $('cpu-flag-overflow');
  dom.flagB              = $('cpu-flag-break');
  dom.flagD              = $('cpu-flag-decimal');
  dom.flagI              = $('cpu-flag-interrupt');
  dom.flagZ              = $('cpu-flag-zero');
  dom.flagC              = $('cpu-flag-carry');
  dom.addressHex         = $('address-hex');
  dom.addressDec         = $('address-dec');
  dom.addressLeds        = $('address-leds');
  dom.dataHex            = $('data-hex');
  dom.dataDec            = $('data-dec');
  dom.dataLeds           = $('data-leds');
  dom.rwLed              = $('rw-led');
  dom.rwLabel            = $('rw-label');

  // Memory panel
  dom.memoryTabs         = $('memory-tabs');
  dom.memoryDump         = $('memory-dump');
  dom.memoryPagination   = $('memory-pagination');

  // Storage panel
  dom.storageTabs        = $('storage-tabs');
  dom.fileList           = $('file-list');
  dom.storagePagination  = $('storage-pagination');

  // Video / AV panel
  dom.videoCanvas        = $('video-canvas');
  dom.avStatus           = $('av-status');
  dom.avConnect          = $('av-connect');
  dom.avDisconnect       = $('av-disconnect');
  dom.soundMute          = $('sound-mute');

  // Footer
  dom.version            = $('version');
}

// ---------------------------------------------------------------------------
// Formatting helpers
// ---------------------------------------------------------------------------

function hex8(val)  { return '$' + val.toString(16).toUpperCase().padStart(2, '0'); }
function hex16(val) { return '$' + val.toString(16).toUpperCase().padStart(4, '0'); }

function formatFreqPeriod(period) {
  if (period >= 1000000) return (period / 1000000).toFixed(1) + ' s';
  if (period >= 1000)    return (period / 1000).toFixed(1) + ' ms';
  if (period >= 1)       return period.toFixed(1) + ' &micro;s';
  return (period * 1000).toFixed(0) + ' ns';
}

function printableAscii(byte) {
  return (byte >= 0x20 && byte <= 0x7E) ? String.fromCharCode(byte) : '.';
}

// ---------------------------------------------------------------------------
// Update the UI from /info data
// ---------------------------------------------------------------------------

function updateInfo(info) {
  // Running state & header buttons
  const running = info.isRunning;
  dom.isRunning.textContent = running ? '-- RUNNING --' : '-- STOPPED --';
  dom.isRunning.className   = 'emulator-status ' + (running ? 'green' : 'red');
  dom.run.disabled      = running;
  dom.stop.disabled     = !running;
  dom.step.disabled     = running;
  dom.tick.disabled     = running;

  // Emulator info rows
  dom.programFile.textContent = info.programFile || 'None';
  dom.romFile.textContent     = info.romFile || 'None';
  dom.cartFile.textContent    = info.cartFile || 'None';
  dom.freqLabel.innerHTML     = escapeHtml(info.freqLabel) + ' (<span id="freq-period">' + formatFreqPeriod(info.freqPeriod) + '</span>)';
  dom.ipAddress.textContent   = info.ipAddress || '0.0.0.0';

  // RTC - format Unix timestamp
  if (info.rtc) {
    const d = new Date(info.rtc * 1000);
    dom.rtc.textContent = d.toLocaleString();
  }

  // Last snapshot
  dom.lastSnapshot.textContent = info.lastSnapshot ? info.lastSnapshot : 'None';

  // Unload Cart button: hidden when no cart loaded, always enabled when visible
  dom.unloadCart.style.display = info.cartEnabled ? '' : 'none';

  // CPU registers
  dom.cpuAccHex.textContent    = hex8(info.cpuAccumulator);
  dom.cpuAccDec.textContent    = info.cpuAccumulator;
  dom.cpuXHex.textContent      = hex8(info.cpuX);
  dom.cpuXDec.textContent      = info.cpuX;
  dom.cpuYHex.textContent      = hex8(info.cpuY);
  dom.cpuYDec.textContent      = info.cpuY;
  dom.cpuPCHex.textContent     = hex16(info.cpuPC);
  dom.cpuPCDec.textContent     = info.cpuPC;
  dom.cpuSPHex.textContent     = hex8(info.cpuStackPointer);
  dom.cpuSPDec.textContent     = info.cpuStackPointer;
  dom.cpuStatusHex.textContent = hex8(info.cpuStatus);
  dom.cpuStatusDec.textContent = info.cpuStatus;

  // Flags  (NV-BDIZC)
  const st = info.cpuStatus;
  setFlag(dom.flagN, st & 0x80);
  setFlag(dom.flagV, st & 0x40);
  setFlag(dom.flagB, st & 0x10);
  setFlag(dom.flagD, st & 0x08);
  setFlag(dom.flagI, st & 0x04);
  setFlag(dom.flagZ, st & 0x02);
  setFlag(dom.flagC, st & 0x01);

  // Address bus
  dom.addressHex.textContent = hex16(info.address);
  dom.addressDec.textContent = info.address;
  updateLeds(dom.addressLeds, info.address, 16);

  // Data bus
  dom.dataHex.textContent = hex8(info.data);
  dom.dataDec.textContent = info.data;
  updateLeds(dom.dataLeds, info.data, 8);

  // R/W
  const rw = info.rw;  // 1 = read, 0 = write
  dom.rwLed.className   = 'cpu-led' + (rw ? ' on' : '');
  dom.rwLabel.textContent = rw ? 'READ' : 'WRITE';

  // Version
  dom.version.textContent = 'Version: ' + (info.version || '1.0');
}

function setEnabledSpan(el, enabled) {
  el.textContent = enabled ? 'ENABLED' : 'DISABLED';
  el.className   = enabled ? 'enabled' : 'disabled';
}

function setFlag(el, on) {
  el.className = 'cpu-flag' + (on ? ' on' : '');
}

function updateLeds(container, value, bits) {
  const leds = container.querySelectorAll('.cpu-led');
  for (let i = 0; i < bits; i++) {
    // MSB first (bit 15 is the first LED for 16-bit, etc.)
    const bitIndex = bits - 1 - i;
    leds[i].className = 'cpu-led' + ((value >> bitIndex) & 1 ? ' on' : '');
  }
}

// ---------------------------------------------------------------------------
// Memory panel
// ---------------------------------------------------------------------------

function memoryTotalPages() {
  return state.memoryTarget === 'ram' ? RAM_PAGES : ROM_PAGES;
}

function memoryBaseAddress() {
  return state.memoryTarget === 'ram' ? 0x0000 : 0x8000;
}

async function refreshMemory() {
  try {
    const bytes = await fetchMemory(state.memoryTarget, state.memoryPage);
    renderMemoryDump(bytes);
    renderPagination(
      dom.memoryPagination,
      state.memoryPage,
      memoryTotalPages(),
      (page) => { state.memoryPage = page; refreshMemory(); }
    );
  } catch (e) {
    dom.memoryDump.innerHTML = '<div class="memory-row" style="color:red">Error loading memory</div>';
  }
}

function renderMemoryDump(bytes) {
  const base = memoryBaseAddress() + state.memoryPage * PAGE_SIZE;
  let html = '';

  for (let row = 0; row < PAGE_SIZE; row += 16) {
    const addr = base + row;
    let hexPart = '';
    let asciiPart = '';

    for (let col = 0; col < 16; col++) {
      const b = bytes[row + col] ?? 0;
      hexPart += b.toString(16).toUpperCase().padStart(2, '0') + ' ';
      asciiPart += escapeHtmlChar(b);
    }

    html += '<div class="memory-row">'
      + '<span class="memory-addr">' + hex16(addr) + ':</span>'
      + '<span>' + hexPart + asciiPart + '</span>'
      + '</div>';
  }

  dom.memoryDump.innerHTML = html;
}

function escapeHtmlChar(byte) {
  if (byte < 0x20 || byte > 0x7E) return '.';
  switch (byte) {
    case 0x26: return '&amp;';   // &
    case 0x3C: return '&lt;';    // <
    case 0x3E: return '&gt;';    // >
    case 0x22: return '&quot;';  // "
    case 0x27: return '&#39;';   // '
    default:   return String.fromCharCode(byte);
  }
}

// ---------------------------------------------------------------------------
// Storage panel
// ---------------------------------------------------------------------------

async function refreshStorage() {
  try {
    const files = await fetchStorage(state.storageTarget, state.storagePage);
    renderFileList(files);
    renderPagination(
      dom.storagePagination,
      state.storagePage,
      STORAGE_PAGES,
      (page) => { state.storagePage = page; refreshStorage(); }
    );
  } catch (e) {
    dom.fileList.innerHTML = '<div style="color:red">Error loading storage</div>';
  }
}

function renderFileList(files) {
  let html = '';

  for (let i = 0; i < FILE_PER_PAGE; i++) {
    const name = files[i] ?? '?';
    const isEmpty = !name || name === '?';
    const slotIndex = i;

    if (isEmpty) {
      html += '<div class="file-item">'
        + '<span class="file-button empty">EMPTY</span>'
        + '<span class="file-name">' + slotIndex + ': ?</span>'
        + '</div>';
    } else {
      html += '<div class="file-item">'
        + '<button class="file-button button load" data-filename="' + escapeAttr(name) + '">LOAD</button>'
        + '<span class="file-name">' + slotIndex + ': ' + escapeHtml(name) + '</span>'
        + '</div>';
    }
  }

  dom.fileList.innerHTML = html;

  // Bind LOAD buttons
  dom.fileList.querySelectorAll('button.load').forEach((btn) => {
    btn.addEventListener('click', async () => {
      const filename = btn.dataset.filename;
      try {
        btn.disabled = true;
        btn.textContent = '...';
        await loadFile(state.storageTarget, filename);
        await refreshAll();
      } catch (e) {
        btn.textContent = 'ERR';
      } finally {
        btn.disabled = false;
        btn.textContent = 'LOAD';
      }
    });
  });
}

function escapeHtml(str) {
  const div = document.createElement('div');
  div.textContent = str;
  return div.innerHTML;
}

function escapeAttr(str) {
  return str.replace(/"/g, '&quot;').replace(/'/g, '&#39;');
}

// ---------------------------------------------------------------------------
// Pagination renderer (shared by Memory and Storage)
// ---------------------------------------------------------------------------

function renderPagination(container, currentPage, totalPages, onNavigate) {
  let html = '';

  // First / Prev
  html += '<button class="page-btn" data-page="0"' + (currentPage === 0 ? ' disabled' : '') + '>&laquo;</button>';
  html += '<button class="page-btn" data-page="' + (currentPage - 1) + '"' + (currentPage === 0 ? ' disabled' : '') + '>&lt;</button>';

  // Page window
  const half = Math.floor(PAGINATION_WINDOW / 2);
  let start = Math.max(0, currentPage - half);
  let end = start + PAGINATION_WINDOW;
  if (end > totalPages) {
    end = totalPages;
    start = Math.max(0, end - PAGINATION_WINDOW);
  }

  for (let p = start; p < end; p++) {
    html += '<button class="page-btn' + (p === currentPage ? ' active' : '') + '" data-page="' + p + '">' + (p + 1) + '</button>';
  }

  // Next / Last
  html += '<button class="page-btn" data-page="' + (currentPage + 1) + '"' + (currentPage >= totalPages - 1 ? ' disabled' : '') + '>&gt;</button>';
  html += '<button class="page-btn" data-page="' + (totalPages - 1) + '"' + (currentPage >= totalPages - 1 ? ' disabled' : '') + '>&raquo;</button>';

  container.innerHTML = html;

  container.querySelectorAll('.page-btn').forEach((btn) => {
    btn.addEventListener('click', () => {
      const page = parseInt(btn.dataset.page, 10);
      if (!isNaN(page) && page >= 0 && page < totalPages) {
        onNavigate(page);
      }
    });
  });
}

// ---------------------------------------------------------------------------
// Tab switching
// ---------------------------------------------------------------------------

function initTabs(tabContainer, onSwitch) {
  tabContainer.querySelectorAll('.tab').forEach((tab) => {
    tab.addEventListener('click', () => {
      tabContainer.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
      tab.classList.add('active');
      onSwitch(tab.dataset.target);
    });
  });
}

// ---------------------------------------------------------------------------
// Header button commands
// ---------------------------------------------------------------------------

async function controlAndRefresh(command) {
  try {
    await sendControl(command);
    await refreshAll();
  } catch (e) {
    console.error('Control error:', e);
  }
}

// ---------------------------------------------------------------------------
// Refresh everything
// ---------------------------------------------------------------------------

async function refreshAll() {
  try {
    const info = await fetchInfo();
    updateInfo(info);
  } catch (e) {
    console.error('Info error:', e);
  }
  await Promise.all([refreshMemory(), refreshStorage()]);
}

// ---------------------------------------------------------------------------
// Event binding
// ---------------------------------------------------------------------------

function bindEvents() {
  // Header buttons -> /control commands
  dom.run.addEventListener('click',            () => controlAndRefresh('r'));
  dom.stop.addEventListener('click',           () => controlAndRefresh('r'));
  dom.step.addEventListener('click',           () => controlAndRefresh('s'));
  dom.tick.addEventListener('click',           () => controlAndRefresh('k'));
  dom.reset.addEventListener('click',          () => controlAndRefresh('t'));
  dom.snapshot.addEventListener('click',       () => controlAndRefresh('p'));
  dom.decreaseFreq.addEventListener('click',   () => controlAndRefresh('-'));
  dom.increaseFreq.addEventListener('click',   () => controlAndRefresh('+'));
  dom.refresh.addEventListener('click',        () => refreshAll());

  // Unload Cart button
  dom.unloadCart.addEventListener('click', () => controlAndRefresh('u'));

  // Memory tabs
  initTabs(dom.memoryTabs, (target) => {
    state.memoryTarget = target;
    state.memoryPage = 0;
    refreshMemory();
  });

  // Storage tabs
  initTabs(dom.storageTabs, (target) => {
    state.storageTarget = target;
    state.storagePage = 0;
    refreshStorage();
  });

  // AV stream controls
  if (!avStream.isSupported()) {
    dom.avConnect.style.display = 'none';
    if (avStream.isSupportedBrowser() && !avStream.isSecureContext()) {
      dom.avStatus.textContent = 'Requires HTTPS or localhost (Web Serial needs a secure context)';
    } else {
      dom.avStatus.textContent = 'Requires Chrome or Edge';
    }
    dom.avStatus.className = 'av-status-disconnected';
  }

  dom.avConnect.addEventListener('click', async () => {
    try {
      await avStream.connect();
      videoRenderer.init(dom.videoCanvas);
      await soundEmulator.init();
      updateAvStatus(true);
      avStream.startReading({
        onSoundWrite: (reg, val) => soundEmulator.onRegWrite(reg, val),
        onVideoDataWrite: (val) => videoRenderer.onDataWrite(val),
        onVideoRegWrite: (reg, val) => videoRenderer.onRegWrite(reg, val),
        onVideoAddrSet: (hi, lo) => videoRenderer.onAddrSet(hi, lo),
        onReset: () => { videoRenderer.onReset(); soundEmulator.onReset(); },
      });
    } catch (e) {
      console.error('AV connect error:', e);
    }
  });

  dom.avDisconnect.addEventListener('click', async () => {
    try {
      await avStream.disconnect();
      updateAvStatus(false);
    } catch (e) {
      console.error('AV disconnect error:', e);
    }
  });

  // Start with correct muted label
  dom.soundMute.textContent = 'Unmute';

  let firstInit = true;

  dom.soundMute.addEventListener('click', async () => {
    // Ensure AudioContext is initialized (needs user gesture)
    await soundEmulator.init();
    soundEmulator.setMuted(!soundEmulator.getMuted());
    dom.soundMute.textContent = soundEmulator.getMuted() ? 'Unmute' : 'Mute';

    // Fire a test beep on unmute so we can verify sound works
    if (!soundEmulator.getMuted()) {
      // On first init the worklet needs time to start processing after
      // being connected to the audio destination.
      if (firstInit) {
        firstInit = false;
        await new Promise(r => setTimeout(r, 500));
      }
      console.log('[DEBUG] Test beep fired from Mute button');
      soundEmulator.onRegWrite(0x18, 0x0F); // Volume max
      soundEmulator.onRegWrite(0x00, 0x20); // Freq Lo
      soundEmulator.onRegWrite(0x01, 0x1F); // Freq Hi
      soundEmulator.onRegWrite(0x05, 0x09); // Attack=0, Decay=9
      soundEmulator.onRegWrite(0x06, 0x00); // Sustain=0, Release=0
      soundEmulator.onRegWrite(0x04, 0x11); // Triangle + Gate ON
      setTimeout(() => {
        soundEmulator.onRegWrite(0x04, 0x10); // Gate OFF
        soundEmulator.onRegWrite(0x00, 0x00);
        soundEmulator.onRegWrite(0x01, 0x00);
        console.log('[DEBUG] Test beep ended');
      }, 200);
    }
  });
}

// ---------------------------------------------------------------------------
// Keyboard input — USB HID keycode mapping
// ---------------------------------------------------------------------------

const USB_HID_KEYMAP = {
  KeyA: 0x04, KeyB: 0x05, KeyC: 0x06, KeyD: 0x07, KeyE: 0x08, KeyF: 0x09,
  KeyG: 0x0A, KeyH: 0x0B, KeyI: 0x0C, KeyJ: 0x0D, KeyK: 0x0E, KeyL: 0x0F,
  KeyM: 0x10, KeyN: 0x11, KeyO: 0x12, KeyP: 0x13, KeyQ: 0x14, KeyR: 0x15,
  KeyS: 0x16, KeyT: 0x17, KeyU: 0x18, KeyV: 0x19, KeyW: 0x1A, KeyX: 0x1B,
  KeyY: 0x1C, KeyZ: 0x1D,
  Digit1: 0x1E, Digit2: 0x1F, Digit3: 0x20, Digit4: 0x21, Digit5: 0x22,
  Digit6: 0x23, Digit7: 0x24, Digit8: 0x25, Digit9: 0x26, Digit0: 0x27,
  Enter: 0x28, Escape: 0x29, Backspace: 0x2A, Tab: 0x2B, Space: 0x2C,
  Minus: 0x2D, Equal: 0x2E, BracketLeft: 0x2F, BracketRight: 0x30,
  Backslash: 0x31, Semicolon: 0x33, Quote: 0x34, Backquote: 0x35,
  Comma: 0x36, Period: 0x37, Slash: 0x38, CapsLock: 0x39,
  F1: 0x3A, F2: 0x3B, F3: 0x3C, F4: 0x3D, F5: 0x3E, F6: 0x3F,
  F7: 0x40, F8: 0x41, F9: 0x42, F10: 0x43, F11: 0x44, F12: 0x45,
  ArrowRight: 0x4F, ArrowLeft: 0x50, ArrowDown: 0x51, ArrowUp: 0x52,
  ShiftLeft: 0xE1, ShiftRight: 0xE5,
  ControlLeft: 0xE0, ControlRight: 0xE4,
  AltLeft: 0xE2, AltRight: 0xE6,
  MetaLeft: 0xE3, MetaRight: 0xE7,
};

const PREVENT_DEFAULT_CODES = new Set([
  'Tab', 'Backspace', 'Space', 'Escape', 'CapsLock',
  'F1', 'F2', 'F3', 'F4', 'F5', 'F6', 'F7', 'F8', 'F9', 'F10', 'F11', 'F12',
  'ArrowRight', 'ArrowLeft', 'ArrowDown', 'ArrowUp',
]);

function sendKeyEvent(action, code) {
  const hid = USB_HID_KEYMAP[code];
  if (hid === undefined) return;
  fetch('/keyboard?action=' + encodeURIComponent(action) + '&keycode=' + hid.toString(16).padStart(2, '0'));
}

// ---------------------------------------------------------------------------
// AV status helper
// ---------------------------------------------------------------------------

function updateAvStatus(connected) {
  dom.avStatus.textContent = connected ? 'CONNECTED' : 'DISCONNECTED';
  dom.avStatus.className = connected ? 'av-status-connected' : 'av-status-disconnected';
  dom.avConnect.disabled = connected;
  dom.avDisconnect.disabled = !connected;
}

// ---------------------------------------------------------------------------
// Bootstrap
// ---------------------------------------------------------------------------

document.addEventListener('DOMContentLoaded', async () => {
  cacheDom();
  bindEvents();
  refreshAll();

  // --- DEV MOCK: auto-start sound + video mock when running under Vite dev server ---
  if (import.meta.env.DEV) {
    const { startMock } = await import('../mock/av-stream-mock.js');
    videoRenderer.init(dom.videoCanvas);

    // Browser requires a user gesture before AudioContext can start.
    // Start the mock immediately for video, but defer sound until first click.
    let soundReady = false;
    const mockCallbacks = {
      onSoundWrite: (reg, val) => soundEmulator.onRegWrite(reg, val),
      onVideoDataWrite: (val) => videoRenderer.onDataWrite(val),
      onVideoRegWrite: (reg, val) => videoRenderer.onRegWrite(reg, val),
      onVideoAddrSet: (hi, lo) => videoRenderer.onAddrSet(hi, lo),
      onReset: () => { videoRenderer.onReset(); soundEmulator.onReset(); },
    };
    startMock(mockCallbacks);
    updateAvStatus(true);
    console.log('[DEV] AV mock started — click anywhere to enable sound');

    document.addEventListener('click', async () => {
      if (soundReady) return;
      soundReady = true;
      await soundEmulator.init();
      // Re-fire a test beep now that AudioContext is live
      mockCallbacks.onSoundWrite(0x18, 0x0F); // Volume max
      mockCallbacks.onSoundWrite(0x00, 0x20); // Freq Lo
      mockCallbacks.onSoundWrite(0x01, 0x1F); // Freq Hi
      mockCallbacks.onSoundWrite(0x05, 0x09); // AD
      mockCallbacks.onSoundWrite(0x06, 0x00); // SR
      mockCallbacks.onSoundWrite(0x04, 0x11); // Triangle + Gate
      setTimeout(() => {
        mockCallbacks.onSoundWrite(0x04, 0x10); // Gate off
      }, 200);
      console.log('[DEV] Sound enabled after user gesture');
    }, { once: true });
  }

  // Keyboard forwarding
  document.addEventListener('keydown', (event) => {
    // CapsLock: send a complete press+release on keydown so the firmware
    // sees exactly one toggle per physical key press on all platforms
    // (macOS fires keydown on lock-on and keyup on lock-off, not symmetrically).
    if (event.code === 'CapsLock') {
      sendKeyEvent('down', 'CapsLock');
      sendKeyEvent('up',   'CapsLock');
      event.preventDefault();
      return;
    }
    sendKeyEvent('down', event.code);
    if (PREVENT_DEFAULT_CODES.has(event.code)) {
      event.preventDefault();
    }
  });

  document.addEventListener('keyup', (event) => {
    if (event.code === 'CapsLock') return; // already handled on keydown
    sendKeyEvent('up', event.code);
  });
});