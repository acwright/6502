export default defineEventHandler(async (event) => {
  const info = await $fetch(/*'http://6502.local/info'*/ 'http://192.168.0.124/info')
  
  return info
  
  // return {
  //   address: 40961,
  //   cartCode: 49152,
  //   cartEnabled: false,
  //   cartEnd: 65535,
  //   cartFile: "None",
  //   cartFilePage: 0,
  //   cartFiles: [
  //     "Wozmon.bin",
  //     "NOP.bin",
  //     "IB Keyboard Transmit.bin",
  //     "IB Mouse Transmit.bin",
  //     "IB SNES Transmit.bin",
  //     "SC Echo Flow Control.bin",
  //     "SC Echo Interrupt.bin",
  //     "EhBASIC.bin"
  //   ],
  //   cartStart: 32768,
  //   cpuAccumulator: 16,
  //   cpuOpcodeCycle: 1,
  //   cpuPC: 40962,
  //   cpuStackPointer: 186,
  //   cpuStatus: 53,
  //   cpuX: 0,
  //   cpuY: 0,
  //   data: 16,
  //   fileMax: 256,
  //   freqLabel: "2 MHz",
  //   freqPeriod: 0.5,
  //   inputCtx: 2,
  //   io: [
  //     "RAM Card",
  //     "Emulator",
  //     "Empty",
  //     "Empty",
  //     "Serial Card",
  //     "Empty",
  //     "Empty",
  //     "Empty"
  //   ],
  //   ioEnd: 40959,
  //   ioSlots: 8,
  //   ioSlotSize: 1024,
  //   ioStart: 32768,
  //   ipAddress: "192.168.0.124",
  //   isRunning: false,
  //   lastSnapshot: 1763465397,
  //   programFile: "DB Hello World.prg",
  //   programFilePage: 0,
  //   ramCode: 2048,
  //   ramEnabled: true,
  //   ramEnd: 32767,
  //   ramFiles: [
  //     "DB Hello World.prg","GPIO Blink.prg",
  //     "SC LCD Echo.prg",
  //     "VC Background.prg",
  //     "VC Init.prg",
  //     "GPIO Input.prg",
  //     "LCD Clear.prg",
  //     "LCD Hello World.prg"
  //   ],
  //   ramStart: 0,
  //   romEnabled: true,
  //   romEnd: 65535,
  //   romFile: "Wozmon.bin",
  //   romFilePage: 0,
  //   romFiles: [
  //     "Wozmon.bin",
  //     "NOP.bin",
  //     "DB BIOS.bin",
  //     "?",
  //     "?",
  //     "?",
  //     "?",
  //     "?"
  //   ],
  //   romStart: 32768,
  //   rtc: 1763465473,
  //   rw: 1,
  //   version: "1.0"
  // }
})