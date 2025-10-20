#! /usr/bin/env node

import figlet from 'figlet'
import { Machine } from './components/Machine'
import { Command } from 'commander'

const VERSION = '1.0.0'

console.log(figlet.textSync("6502 Emulator"))
console.log(`Version: ${VERSION} | A.C. Wright Design`)
console.log()

const machine = new Machine()

const program = new Command()
program
  .name('6502emulator')
  .description('A 6502 Emulator written in TypeScript for the A.C. Wright 6502 project')
  .version(VERSION, '-v, --version', 'Output the current emulator version')
  .helpOption('-h, --help', 'Output help / options')
  .option('-c, --cart <path>', 'Path to 32K Cart binary file')
  .option('-d, --debug', 'Enable debug mode', false)
  .option('-f, --freq <freq>', 'Set the clock frequency in Hz', '1000000')
  .option('-r, --rom <path>', 'Path to 32K ROM binary file')
  .option('-s, --scale <scale>', 'Set the emulator scale', '1')
  .parse(process.argv)

const options = program.opts()
if (options.cart) {
  machine.loadCart(options.cart)
  console.log(`Loaded Cart: ${options.cart}`)
} else {
  console.log('Loaded Cart: NONE')
}
if (options.rom) {
  machine.loadROM(options.rom)
  console.log(`Loaded ROM: ${options.rom}`)
} else {
  console.log('Loaded ROM: NONE')
}
if (options.freq) {
  const frequency = Number(options.freq)
  
  if (!isNaN(frequency)) {
    machine.frequency = frequency
    console.log(`Frequency: ${options.freq} Hz`)
  } else {
    console.log()
    console.error(`Aborting... Error Invalid Frequency: '${options.freq}'`)
    process.exit(1)
  }
} else {
  console.log("Frequency: 1000000 Hz")
}
if (options.scale) {
  const scale = Number(options.scale)

  if (!isNaN(scale)) {
    machine.scale = scale
    console.log(`Scale: ${options.scale}x`)
  } else {
    console.log()
    console.error(`Aborting... Error Invalid Scale: '${options.scale}'`)
    process.exit(1)
  }
} else {
  console.log(`Scale: 1x`)
}
if (options.debug) {
  machine.isDebugging = true
  console.log("Debug: Enabled")
} else {
  console.log("Debug: Disabled")
}

machine.launch((uptime) => {
  console.log()
  console.log('Result:')
  console.table({
    'Time Elapsed': uptime / 1000,
    'CPU Cycles': machine.cycles,
    'Frames': machine.frames,
    'Avg FPS': machine.frames / (uptime / 1000)
  })
})