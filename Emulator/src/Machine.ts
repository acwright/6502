// import { Bus } from './Bus'
import { CPU } from './CPU'
import { RAM } from './RAM'
import { ROM } from './ROM'
import { Cart } from './Cart'
import { type IO } from './IO'

export class Machine {

  cpu: CPU = new CPU()
  ram: RAM = new RAM()
  rom: ROM = new ROM()

  io: {
    slot1?: IO,
    slot2?: IO,
    slot3?: IO,
    slot4?: IO,
    slot5?: IO,
    slot6?: IO,
    slot7?: IO,
    slot8?: IO
  } = {}

  cart?: Cart

  cycles: number = 0
  frameDelay: number = 0
  frameDelayCount: number = 0

  loadROM(data: number[]): void {
    this.rom.load(data)
  }

  loadCart(data: number[]): void {
    const cart = new Cart()
    cart.load(data)

    this.cart = cart
  }

  reset(): void {
    this.cpu.reset()

    if (this.io.slot1) {
      this.io.slot1.reset()
    }
    if (this.io.slot2) {
      this.io.slot2.reset()
    }
    if (this.io.slot3) {
      this.io.slot3.reset()
    }
    if (this.io.slot4) {
      this.io.slot4.reset()
    }
    if (this.io.slot5) {
      this.io.slot5.reset()
    }
    if (this.io.slot6) {
      this.io.slot6.reset()
    }
    if (this.io.slot7) {
      this.io.slot7.reset()
    }
    if (this.io.slot8) {
      this.io.slot8.reset()
    }
  }

  sync(frequency: number, refreshRate: number = 60): void {
    // TODO: Step the clock forward n cycles based on the frequency and display refresh rate (60Hz)
    // https://www.reddit.com/r/EmuDev/comments/ksbvgx/how_to_set_clock_speed_in_c/

    // This is missing IO stepping at IO freq rate...
    
    if (frequency >= refreshRate) {
      let cpuCycles = Math.floor(frequency / refreshRate)
      this.cpu.step(
        cpuCycles, 
        this.read, 
        this.write
      )
      this.cycles += cpuCycles
    } else {
      this.frameDelay = Math.floor(refreshRate / frequency)

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
        return this.io.slot1?.read(address - 0x8000) || 0
      case (address >= 0x8400 && address <= 0x87FF):
        return this.io.slot2?.read(address - 0x8400) || 0
      case (address >= 0x8800 && address <= 0x8BFF):
        return this.io.slot3?.read(address - 0x8800) || 0
      case (address >= 0x8C00 && address <= 0x8FFF):
        return this.io.slot4?.read(address - 0x8C00) || 0
      case (address >= 0x9000 && address <= 0x93FF):
        return this.io.slot5?.read(address - 0x9000) || 0
      case (address >= 0x9400 && address <= 0x97FF):
        return this.io.slot6?.read(address - 0x9400) || 0
      case (address >= 0x9800 && address <= 0x9BFF):
        return this.io.slot7?.read(address - 0x9800) || 0
      case (address >= 0x9C00 && address <= 0x9FFF):
        return this.io.slot8?.read(address - 0x9C00) || 0
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
        this.io.slot1?.write(address - 0x8000, data) || 0
        return
      case (address >= 0x8400 && address <= 0x87FF):
        this.io.slot2?.write(address - 0x8400, data) || 0
        return
      case (address >= 0x8800 && address <= 0x8BFF):
        this.io.slot3?.write(address - 0x8800, data) || 0
        return
      case (address >= 0x8C00 && address <= 0x8FFF):
        this.io.slot4?.write(address - 0x8C00, data) || 0
        return
      case (address >= 0x9000 && address <= 0x93FF):
        this.io.slot5?.write(address - 0x9000, data) || 0
        return
      case (address >= 0x9400 && address <= 0x97FF):
        this.io.slot6?.write(address - 0x9400, data) || 0
        return
      case (address >= 0x9800 && address <= 0x9BFF):
        this.io.slot7?.write(address - 0x9800, data) || 0
        return
      case (address >= 0x9C00 && address <= 0x9FFF):
        this.io.slot8?.write(address - 0x9C00, data) || 0
        return
      default:
        return
    }
  }

}