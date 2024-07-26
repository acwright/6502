import { CPU } from './CPU'
import { RAM } from './RAM'
import { ROM } from './ROM'
import { Cart } from './Cart'
import { type IO, type IODescription } from './IO'
import { Empty } from './io/Empty'
import { DevBoard } from './io/DevBoard'
import { GPIOCard } from './io/GPIOCard'
import { InputBoard } from './io/InputBoard'
import { LCDCard } from './io/LCDCard'
import { PiCard } from './io/PiCard'
import { SerialCard } from './io/SerialCard'
import { SoundCard } from './io/SoundCard'
import { StorageCard } from './io/StorageCard'
import { TeensyCard } from './io/TeensyCard'
import { VGACard } from './io/VGACard'
import { VideoCard } from './io/VideoCard'

export class Machine {

  static IO_DESCRIPTIONS: IODescription[] = [
    Empty.DESCRIPTION,
    DevBoard.DESCRIPTION,
    GPIOCard.DESCRIPTION,
    InputBoard.DESCRIPTION,
    LCDCard.DESCRIPTION,
    PiCard.DESCRIPTION,
    SerialCard.DESCRIPTION,
    SoundCard.DESCRIPTION,
    StorageCard.DESCRIPTION,
    TeensyCard.DESCRIPTION,
    VGACard.DESCRIPTION,
    VideoCard.DESCRIPTION
  ]

  cpu: CPU = new CPU()
  ram: RAM = new RAM()
  rom: ROM = new ROM()

  io: IO[] = [
    new LCDCard(),
    new StorageCard(),
    new SoundCard(),
    new VideoCard(),
    new SerialCard(),
    new GPIOCard(),
    new InputBoard(),
    new Empty()
  ]

  cart?: Cart

  romFile?: File
  cartFile?: File

  isRunning: boolean = false
  frequency: number = 1000000 // 1 MHz
  cycles: number = 0
  frameDelay: number = 0
  frameDelayCount: number = 0

  async loadROM(romFile: File): Promise<void> {
    this.romFile = romFile
    this.rom.load(Array.from(new Uint8Array(await romFile.arrayBuffer())))
  }

  async loadCart(cartFile: File): Promise<void> {
    this.cartFile = cartFile
    
    const cart = new Cart()
    cart.load(Array.from(new Uint8Array(await cartFile.arrayBuffer())))

    this.cart = cart
  }

  loadSlot(slot: number, description: IODescription): void {
    if (slot < 0 || slot >= 8) { return }
    
    switch (description.className) {
      case 'DevBoard':
        this.io[slot] = new DevBoard()
        break
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
      case 'PiCard':
        this.io[slot] = new PiCard()
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
      case 'TeensyCard':
        this.io[slot] = new TeensyCard()
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

  sync(refreshRate: number = 60): void {
    // TODO: Step the clock forward n cycles based on the frequency and display refresh rate (60Hz)
    // https://www.reddit.com/r/EmuDev/comments/ksbvgx/how_to_set_clock_speed_in_c/

    // This is missing IO stepping at IO freq rate...
    
    if (this.frequency >= refreshRate) {
      let cpuCycles = Math.floor(this.frequency / refreshRate)
      this.cpu.step(
        cpuCycles, 
        this.read, 
        this.write
      )
      this.cycles += cpuCycles
    } else {
      this.frameDelay = Math.floor(refreshRate / this.frequency)

      if (this.frameDelayCount >= this.frameDelay) {
        this.cpu.step(
          1, 
          this.read, 
          this.write
        )
        this.cycles += 1
        this.frameDelayCount = 0
      } else {
        this.frameDelayCount += 1
      }
    }
  }

  step(): void {
    // This is missing IO stepping at IO freq rate...

    this.cpu.step(
      1, 
      this.read, 
      this.write
    )
    this.cycles += 1
  }

  run(): void {
    this.isRunning = true
  }

  stop(): void {
    this.isRunning = false
  }

  read(address: number): number {
    switch(true) {
      case (address >= RAM.firstAddress && address <= RAM.lastAddress):
        return this.ram.read(address)
      case (this.cart && address >= Cart.firstAddress && address <= Cart.lastAddress):
        return this.cart.read(address - Cart.firstAddress)
      case (address >= ROM.codeAddress && address <= ROM.lastAddress):
        return this.rom.read(address - ROM.firstAddress)
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
      case (address >= RAM.firstAddress && address <= RAM.lastAddress):
        this.ram.write(address, data)
        return
      case (this.cart && address >= Cart.firstAddress && address <= Cart.lastAddress):
        this.cart.write(address - Cart.firstAddress, data)
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