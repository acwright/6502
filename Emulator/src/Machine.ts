import { CPU } from './CPU'
import { RAM } from './RAM'
import { ROM } from './ROM'
import { Cart } from './Cart'
import { type IO, type IODescription } from './IO'
import { LCDCard } from './io/LCDCard'
import { Empty } from './io/Empty'

export class Machine {

  static IO_DESCRIPTIONS: IODescription[] = [
    { token: 'empty', title: 'Empty' },
    { token: 'dev_board', title: 'Dev Board' },
    { token: 'gpio_card', title: 'GPIO Card' },
    { token: 'input_board', title: 'Input Board' },
    { token: 'lcd_card', title: 'LCD Card' },
    { token: 'pi_card', title: 'Pi Card' },
    { token: 'serial_card', title: 'Serial Card' },
    { token: 'sound_card', title: 'Sound Card' },
    { token: 'storage_card', title: 'Storage Card' },
    { token: 'teensy_card', title: 'Teensy Card' },
    { token: 'vga_card', title: 'VGA Card' },
    { token: 'video_card', title: 'Video Card' }
  ]

  cpu: CPU = new CPU()
  ram: RAM = new RAM()
  rom: ROM = new ROM()

  // IODEVICES[4], IODEVICES[8], IODEVICES[7], IODEVICES[11],
  // IODEVICES[6], IODEVICES[0], IODEVICES[0], IODEVICES[1]
  io: IO[] = [
    new LCDCard(),
    new Empty(),
    new Empty(),
    new Empty(),
    new Empty(),
    new Empty(),
    new Empty(),
    new Empty()
  ]

  cart?: Cart

  romFile?: File
  cartFile?: File

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

  assignSlots(slots: IODescription[]): void {

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