#! /usr/bin/env node

import sdl from '@kmamal/sdl'
import figlet from 'figlet'
import { Machine } from './components/Machine'
import { createCanvas } from '@napi-rs/canvas'
import { Command } from 'commander'

console.log(figlet.textSync("6502 Emulator"))
console.table({
  'Version': 1,
  'Platform': 2,
  'CPU Count': 3,
  'Total RAM': "test",
})

const program = new Command()
program
  .name('6502emulator')
  .description('A 6502 Emulator written in TypeScript')
  .version('1.0.0')
  .option('-r, --rom <path>', 'Path to ROM file')
  .option('-d, --debug', 'Enable debug mode', false)
  .parse(process.argv)

const options = program.opts()
if (options.debug) {
  console.log("Debug mode enabled")
}
if (options.rom) {
  console.log(`Loading ROM from ${options.rom}`)
}

const machine = new Machine()
const scale = 1

const window = sdl.video.createWindow({ 
  title: "6502 Emulator", 
  width: 960 * scale, 
  height: 480 * scale
})
console.log(`Window size: ${window.width}x${window.height}, pixel size: ${window.pixelWidth}x${window.pixelHeight}`)
// window.on('*', console.log)

const { pixelWidth: width, pixelHeight: height } = window
const canvas = createCanvas(width / 3, height)
const ctx = canvas.getContext('2d')

ctx.fillStyle = 'black'
ctx.fillRect(0, 0, width / 3, height)
ctx.font = "monospace 16px"
ctx.fillStyle = 'green'
ctx.fillText("00 00 FF 00", 0, 16)

window.render(
  width / 3, 
  height, 
  (width / 3) * 4, 
  'rgba32', 
  Buffer.from(ctx.getImageData(0, 0, width / 3, height).data), 
  { dstRect: { x: width - (width / 3), y: 0, width: width / 3, height: height }}
)