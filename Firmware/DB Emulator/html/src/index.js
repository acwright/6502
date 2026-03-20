import './index.css'
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
  dom.refresh.disabled  = running;
  dom.snapshot.disabled = running;

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

  // Unload Cart button: disabled while running or no cart loaded
  dom.unloadCart.disabled = running || !info.cartEnabled;

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
}

// ---------------------------------------------------------------------------
// Bootstrap
// ---------------------------------------------------------------------------

document.addEventListener('DOMContentLoaded', () => {
  cacheDom();
  bindEvents();
  refreshAll();
});