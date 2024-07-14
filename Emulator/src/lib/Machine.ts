import { Bus } from './Bus'
import { CPU } from './CPU'
import { RAM } from './RAM'
import { ROM } from './ROM'

export class Machine {

  bus: Bus
  frequency: number

  isRunning: boolean = false
  cycles: number = 0
  frameDelay: number = 0
  frameDelayCount: number = 0

  constructor(frequency: number = 8000000, bus?: Bus) {
    this.frequency = frequency

    if (bus) {
      this.bus = bus
    } else {
      this.bus = new Bus([
        new CPU(),
        new RAM(),
        new ROM()
      ])
    }
  }

  start() {
    this.isRunning = true
  }

  stop() {
    this.isRunning = false
  }

  toggle() {
    this.isRunning = !this.isRunning
  }

  sync(refreshRate: number = 60) {
    // TODO: Step the clock forward n cycles based on the frequency and display refresh rate (60Hz)
    // https://www.reddit.com/r/EmuDev/comments/ksbvgx/how_to_set_clock_speed_in_c/

    if (this.frequency >= refreshRate) {
      this.step(Math.floor(this.frequency / refreshRate))
    } else {
      this.frameDelay = Math.floor(refreshRate / this.frequency)

      if (this.frameDelayCount >= this.frameDelay) {
        this.step()
        this.frameDelayCount = 0
      } else {
        this.frameDelayCount += 1
      }
    }
  }

  step(cycles: number = 1) {
    this.bus.clock(cycles, this.frequency)
    this.cycles += cycles
  }

  reset() {
    this.bus.reset()
  }

}