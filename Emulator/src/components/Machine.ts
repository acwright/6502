import { CPU } from './CPU'
import { RAM } from './RAM'
import { ROM } from './ROM'
import { Cart } from './Cart'
import { type IO, type IODescription } from './IO'
import { Empty } from './IO/Empty'
import { GPIOCard } from './IO/GPIOCard'
import { InputBoard } from './IO/InputBoard'
import { LCDCard } from './IO/LCDCard'
import { RTCCard } from './IO/RTCCard'
import { SerialCard } from './IO/SerialCard'
import { SoundCard } from './IO/SoundCard'
import { StorageCard } from './IO/StorageCard'
import { VGACard } from './IO/VGACard'
import { VideoCard } from './IO/VideoCard'
import { readFile } from 'fs/promises'
import sdl, { Sdl } from '@kmamal/sdl'
import { Canvas, createCanvas, SKRSContext2D } from '@napi-rs/canvas'

export class Machine {

  static IO_DESCRIPTIONS: IODescription[] = [
    Empty.DESCRIPTION,
    GPIOCard.DESCRIPTION,
    InputBoard.DESCRIPTION,
    LCDCard.DESCRIPTION,
    RTCCard.DESCRIPTION,
    SerialCard.DESCRIPTION,
    SoundCard.DESCRIPTION,
    StorageCard.DESCRIPTION,
    VGACard.DESCRIPTION,
    VideoCard.DESCRIPTION
  ]

  static MAX_FPS: number = 60
  static FRAME_INTERVAL_MS: number = 1000 / Machine.MAX_FPS
  private startTime: number = 0
  private previousTime: number = Date.now()

  cpu: CPU
  ram: RAM
  rom: ROM
  io: IO[] 

  cart?: Cart

  isRunning: boolean = false
  isDebugging: boolean = false
  frequency: number = 1000000 // 1 MHz
  scale: number = 1
  cycles: number = 0
  frames: number = 0
  frameDelay: number = 0
  frameDelayCount: number = 0

  window?: Sdl.Video.Window
  canvas?: Canvas
  ctx?: SKRSContext2D

  constructor() {
    this.ram = new RAM()
    this.rom = new ROM()
    this.io = [
      new Empty(),
      new Empty(),
      new RTCCard(),
      new StorageCard(),
      new SerialCard(),
      new GPIOCard(),
      new SoundCard(),
      new VideoCard()
    ]
    this.cpu = new CPU(this.read.bind(this), this.write.bind(this))
  }

  loadROM = async (path: string) => {
    try {
      this.rom.load(Array.from(new Uint8Array(await readFile(path))))
    } catch (error) {
      console.error('Error reading file:', error)
    }
  }

  loadCart = async (path: string) => {
    try {
      const data = Array.from(new Uint8Array(await readFile(path)))
      const cart = new Cart()
      cart.load(data)
      this.cart = cart
    } catch (error) {
      console.error('Error reading file:', error)
    }
  }

  loadSlot(slot: number, description: IODescription): void {
    if (slot < 0 || slot >= 8) { return }
    
    switch (description.className) {
      case 'Empty':
        this.io[slot] = new Empty()
        break
      case 'GPIOCard':
        this.io[slot] = new GPIOCard()
        break
      case 'InputBoard':
        this.io[slot] = new InputBoard()
        break
      case 'LCDCard':
        this.io[slot] = new LCDCard()
        break
      case 'RTCCard':
        this.io[slot] = new RTCCard()
        break
      case 'SerialCard':
        this.io[slot] = new SerialCard()
        break
      case 'SoundCard':
        this.io[slot] = new SoundCard()
        break
      case 'StorageCard':
        this.io[slot] = new StorageCard()
        break
      case 'VGACard':
        this.io[slot] = new VGACard()
        break
      case 'VideoCard':
        this.io[slot] = new VideoCard()
        break
    }
  }

  reset(): void {
    this.cpu.reset()

    this.io.forEach((io) => {
      io.reset()
    })
  }

  loop(): void {
    this.frames += 1

    if (!this.isRunning || this.window?.destroyed) { return }

    const currentTime = Date.now()
    const deltaTime = currentTime - this.previousTime;
    this.previousTime = currentTime
    const fps = 1 / (deltaTime / 1000)

    if (this.frequency >= fps) {
      const cycles = Math.floor(this.frequency / fps)

      for (let i = 0; i < cycles; i++) {
        this.cpu.tick()
        this.io[0].tick()
        this.io[1].tick()
        this.io[2].tick()
        this.io[3].tick()
        this.io[4].tick()
        this.io[5].tick()
        this.io[6].tick()
        this.io[7].tick() // This is faster than looping over IO array!
      }

      this.cycles += cycles
    } else {
      this.frameDelay = Math.floor(fps / this.frequency)

      if (this.frameDelayCount >= this.frameDelay) {
        this.cpu.tick()
        this.io[0].tick()
        this.io[1].tick()
        this.io[2].tick()
        this.io[3].tick()
        this.io[4].tick()
        this.io[5].tick()
        this.io[6].tick()
        this.io[7].tick() // This is faster than looping over IO array!

        this.cycles += 1
        this.frameDelayCount = 0
      } else {
        this.frameDelayCount += 1
      }
    }

    this.render()

    setTimeout(this.loop.bind(this), Math.floor(Machine.FRAME_INTERVAL_MS))
  }

  launch(onComplete: (uptime: number) => void): void {
    this.window = sdl.video.createWindow({ 
      title: "6502 Emulator",
      width: 256 * this.scale,
      height: 192 * this.scale
    })

    this.canvas = createCanvas(256, 192)
    this.ctx = this.canvas.getContext('2d')

    this.window.on('close', () => {
      onComplete(Date.now() - this.startTime)
    })

    this.isRunning = true
    this.startTime = Date.now()

    this.loop()
  }

  render(): void {
    if (!this.window || !this.ctx) { return }

    // this.ctx.imageSmoothingEnabled = false
    // this.ctx.fillStyle = 'black'
    // this.ctx.fillRect(0, 0, 256, 192)
    // this.ctx.fillStyle = 'green'
    // this.ctx.fillRect(160, 16, 1, 1)
    // this.ctx.fillRect(161, 17, 1, 1)
    // // this.ctx.font = "monospace 16px"
    // // this.ctx.fillStyle = 'green'
    // // this.ctx.fillText("00 00 FF 00", 0, 16)

    // this.window.render(
    //   256,
    //   192,
    //   256 * 4,
    //   'rgba32',
    //   Buffer.from(this.ctx.getImageData(0, 0, 256, 192).data)
    // )

    const buffer = Buffer.alloc(256 * 192 * 4)

    let offset = 0
    for (let i = 0; i < 192; i++) {
      for (let j = 0; j < 256; j++) {
        buffer[offset++] = Math.floor(256 * i / 192)    // R
        buffer[offset++] = Math.floor(256 * j / 256)    // G
        buffer[offset++] = 0                            // B
        buffer[offset++] = 255                          // A
      }
    }

    this.window.render(256, 192, 256 * 4, 'rgba32', buffer)
  }

  run(): void {
    this.isRunning = true
  }

  stop(): void {
    this.isRunning = false
  }

  read(address: number): number {
    switch(true) {
      case (address >= RAM.START && address <= RAM.END):
        return this.ram.read(address)
      case (this.cart && address >= Cart.CODE && address <= Cart.END):
        return this.cart.read(address - Cart.START)
      case (address >= ROM.CODE && address <= ROM.END):
        return this.rom.read(address - ROM.START)
      case (address >= 0x8000 && address <= 0x83FF):
        return this.io[0].read(address - 0x8000) || 0
      case (address >= 0x8400 && address <= 0x87FF):
        return this.io[1].read(address - 0x8400) || 0
      case (address >= 0x8800 && address <= 0x8BFF):
        return this.io[2].read(address - 0x8800) || 0
      case (address >= 0x8C00 && address <= 0x8FFF):
        return this.io[3].read(address - 0x8C00) || 0
      case (address >= 0x9000 && address <= 0x93FF):
        return this.io[4].read(address - 0x9000) || 0
      case (address >= 0x9400 && address <= 0x97FF):
        return this.io[5].read(address - 0x9400) || 0
      case (address >= 0x9800 && address <= 0x9BFF):
        return this.io[6].read(address - 0x9800) || 0
      case (address >= 0x9C00 && address <= 0x9FFF):
        return this.io[7].read(address - 0x9C00) || 0
      default:
        return 0
    }
  }

  write(address: number, data: number): void {
    switch(true) {
      case (address >= RAM.START && address <= RAM.END):
        this.ram.write(address, data)
        return
      case (address >= 0x8000 && address <= 0x83FF):
        this.io[0].write(address - 0x8000, data)
        return
      case (address >= 0x8400 && address <= 0x87FF):
        this.io[1].write(address - 0x8400, data)
        return
      case (address >= 0x8800 && address <= 0x8BFF):
        this.io[2].write(address - 0x8800, data)
        return
      case (address >= 0x8C00 && address <= 0x8FFF):
        this.io[3].write(address - 0x8C00, data)
        return
      case (address >= 0x9000 && address <= 0x93FF):
        this.io[4].write(address - 0x9000, data)
        return
      case (address >= 0x9400 && address <= 0x97FF):
        this.io[5].write(address - 0x9400, data)
        return
      case (address >= 0x9800 && address <= 0x9BFF):
        this.io[6].write(address - 0x9800, data)
        return
      case (address >= 0x9C00 && address <= 0x9FFF):
        this.io[7].write(address - 0x9C00, data)
        return
      default:
        return
    }
  }

}