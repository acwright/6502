import { type Device } from './Device'

export class Bus {

  private devices: Device[] = []

  constructor(devices: Device[] = []) {
    devices.forEach((device) => {
      this.attachDevice(device)
    })
  }

  attachDevice(device: Device) {
    device.bus = this

    this.devices.push(device)
  }

  detachDevice(id: string) {
    this.devices = this.devices.filter((device) => {
      return device.id === id
    })
  }

  read(address: number): number {
    let data = 0

    this.devices.forEach((device) => {
      if (device.read) {
        data |= device.read(address)
      }
    })

    return data
  }

  write(address: number, data: number) {
    this.devices.forEach((device) => {
      if (device.write) {
        device.write(address, data)
      }
    })
  }

  clock(cycles: number, frequency: number) {
    this.devices.forEach((device) => {
      if (device.clock) {
        device.clock(cycles, frequency)
      }
    })
  }

  reset() {
    this.devices.forEach((device) => {
      if (device.reset) { device.reset() }
    })
  }

  irq() {
    this.devices.forEach((device) => {
      if (device.irq) { device.irq() }
    })
  }

  nmi() {
    this.devices.forEach((device) => {
      if (device.nmi) { device.nmi() }
    })
  }

}