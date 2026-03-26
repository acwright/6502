import { defineMock } from 'vite-plugin-mock-dev-server'

// ---------------------------------------------------------------------------
// Shared mock state (mutable so controls actually toggle things)
// ---------------------------------------------------------------------------

const FREQ_LABELS  = ["1 Hz","2 Hz","4 Hz","8 Hz","16 Hz","32 Hz","64 Hz","122 Hz","244 Hz","488 Hz","976 Hz","1.9 kHz","3.9 kHz","7.8 kHz","15.6 kHz","31.2 kHz","62.5 kHz","125 kHz","250 kHz","500 kHz","1 MHz","2 MHz"]
const FREQ_PERIODS = [1000000,500000,250000,125000,62500,31250,15625,7812,3906,2048,1024,512,256,128,64,32,16,8,4,2,1,0.5]

const FILE_PER_PAGE = 8

const state = {
  isRunning: false,
  ramEnabled: true,
  romEnabled: true,
  cartEnabled: false,
  freqIndex: 20,               // 1 MHz
  programFile: 'hello.bin',
  romFile: 'BIOS.bin',
  cartFile: '',
  lastSnapshot: 0,
  cpuAccumulator: 0x42,
  cpuX: 0x10,
  cpuY: 0x20,
  cpuPC: 0xA000,
  cpuStackPointer: 0xFD,
  cpuStatus: 0x34,             // NV-BDIZC  => 0b00110100 = I,unused,B set
  cpuOpcodeCycle: 0,
  address: 0xA000,
  data: 0xEA,                  // NOP
  rw: 1,                       // read
}

// Fake ROM data – fill with a simple pattern
function generateMemPage(base, page) {
  const buf = Buffer.alloc(256)
  for (let i = 0; i < 256; i++) {
    buf[i] = (base + page * 256 + i) & 0xFF
  }
  return buf
}

// Fake file lists on SD card
const romFiles     = ['BIOS.bin', 'Monitor.bin', 'BASIC.bin', '?', '?', '?', '?', '?',
                       'Forth.bin', 'TinyBASIC.bin', '?', '?', '?', '?', '?', '?']
const cartFiles    = ['Hozmom.bin', 'MDP.bin', '?', '?', '?', '?', '?', '?']
const programFiles = ['hello.bin', 'blink.bin', 'fib.bin', 'snake.bin', '?', '?', '?', '?',
                       'test1.bin', '?', '?', '?', '?', '?', '?', '?']

// Pad all lists to 256 entries (FILE_MAX)
while (romFiles.length < 256)     romFiles.push('?')
while (cartFiles.length < 256)    cartFiles.push('?')
while (programFiles.length < 256) programFiles.push('?')

// ---------------------------------------------------------------------------
// GET /info
// ---------------------------------------------------------------------------

export default defineMock([
  {
    url: '/info',
    method: 'GET',
    body: (request) => ({
      address:          state.address,
      cartEnabled:      state.cartEnabled,
      cartFile:         state.cartFile,
      cpuAccumulator:   state.cpuAccumulator,
      cpuOpcodeCycle:   state.cpuOpcodeCycle,
      cpuPC:            state.cpuPC,
      cpuStackPointer:  state.cpuStackPointer,
      cpuStatus:        state.cpuStatus,
      cpuX:             state.cpuX,
      cpuY:             state.cpuY,
      data:             state.data,
      freqLabel:        FREQ_LABELS[state.freqIndex],
      freqPeriod:       FREQ_PERIODS[state.freqIndex],
      ipAddress:        '192.168.1.42',
      isRunning:        state.isRunning,
      lastSnapshot:     state.lastSnapshot,
      programFile:      state.programFile,
      ramEnabled:       state.ramEnabled,
      romEnabled:       state.romEnabled,
      romFile:          state.romFile,
      rtc:              Math.floor(Date.now() / 1000),
      rw:               state.rw,
      version:          '1.0',
      soundEnabled:     true,
      videoEnabled:     true,
      avStreamConnected: false,
    }),
  },

  // ---------------------------------------------------------------------------
  // GET /control?command=X
  // ---------------------------------------------------------------------------
  {
    url: '/control',
    method: 'GET',
    body: (request) => {
      const url = new URL(request.url, 'http://localhost')
      const cmd = url.searchParams.get('command')

      switch (cmd) {
        case 'r': case 'R': state.isRunning = !state.isRunning; break
        case 'a': case 'A': state.ramEnabled = !state.ramEnabled; break
        case 'o': case 'O': state.romEnabled = !state.romEnabled; break
        case 'l': case 'L': state.cartEnabled = !state.cartEnabled; break
        case 's': case 'S':                                         // step
          state.cpuPC = (state.cpuPC + 1) & 0xFFFF
          state.address = state.cpuPC
          state.data = Math.floor(Math.random() * 256)
          state.rw = 1
          break
        case 'k': case 'K':                                         // tick
          state.address = (state.address + 1) & 0xFFFF
          state.data = Math.floor(Math.random() * 256)
          state.rw = Math.round(Math.random())
          break
        case 't': case 'T':                                         // reset
          state.cpuPC = 0xA000
          state.cpuAccumulator = 0x00
          state.cpuX = 0x00
          state.cpuY = 0x00
          state.cpuStackPointer = 0xFD
          state.cpuStatus = 0x34
          state.address = 0xA000
          state.data = 0xEA
          state.rw = 1
          break
        case 'p': case 'P':                                         // snapshot
          state.lastSnapshot = Math.floor(Date.now() / 1000)
          break
        case '+':
          if (state.freqIndex < FREQ_LABELS.length - 1) state.freqIndex++
          break
        case '-':
          if (state.freqIndex > 0) state.freqIndex--
          break
      }

      return ''
    },
  },

  // ---------------------------------------------------------------------------
  // GET /memory?target=ram|rom&page=N
  // ---------------------------------------------------------------------------
  {
    url: '/memory',
    method: 'GET',
    response: (request, response) => {
      const url = new URL(request.url, 'http://localhost')
      const target = url.searchParams.get('target')
      const page   = parseInt(url.searchParams.get('page') || '0', 10)

      if (!target || (target !== 'ram' && target !== 'rom')) {
        response.statusCode = 400
        response.end()
        return
      }

      const base = target === 'ram' ? 0x0000 : 0x8000
      const buf = generateMemPage(base, page)

      response.setHeader('Content-Type', 'application/octet-stream')
      response.end(buf)
    },
  },

  // ---------------------------------------------------------------------------
  // GET /storage?target=rom|cart|program&page=N
  // ---------------------------------------------------------------------------
  {
    url: '/storage',
    method: 'GET',
    body: (request) => {
      const url = new URL(request.url, 'http://localhost')
      const target = url.searchParams.get('target')
      const page   = parseInt(url.searchParams.get('page') || '0', 10)

      let list
      if (target === 'rom')          list = romFiles
      else if (target === 'cart')    list = cartFiles
      else if (target === 'program') list = programFiles
      else return []

      const start = page * FILE_PER_PAGE
      return list.slice(start, start + FILE_PER_PAGE)
    },
  },

  // ---------------------------------------------------------------------------
  // GET /load?target=rom|cart|program&filename=X
  // ---------------------------------------------------------------------------
  {
    url: '/load',
    method: 'GET',
    body: (request) => {
      const url = new URL(request.url, 'http://localhost')
      const target   = url.searchParams.get('target')
      const filename = url.searchParams.get('filename')

      if (target === 'rom')          state.romFile = filename
      else if (target === 'cart')    state.cartFile = filename
      else if (target === 'program') state.programFile = filename

      return ''
    },
  },

  // ---------------------------------------------------------------------------
  // GET /keyboard?action=down|up&keycode=XX
  // ---------------------------------------------------------------------------
  {
    url: '/keyboard',
    method: 'GET',
    body: () => '',
  },
])
