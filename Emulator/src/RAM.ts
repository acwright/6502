import { type Device } from './Device'
import { v4 as uuidv4 } from 'uuid'

export class RAM implements Device {

  id: string = uuidv4()
  title: string = "RAM"
  description: string = "Random Access Memory"
  
  firstAddress: number
  lastAddress: number
  isResettable: boolean
  data: number[]

  constructor(
    firstAddress: number = 0x0000, 
    lastAddress: number = 0x7FFF,
    data: number[] = [...Array(lastAddress - firstAddress + 1)].fill(0x00),
    isResettable: boolean = true
  ) {
    if (data.length != lastAddress - firstAddress + 1) { 
      throw new RangeError('Data length must equal address range') 
    }
    
    this.firstAddress = firstAddress
    this.lastAddress = lastAddress
    this.data = data
    this.isResettable = isResettable
  }

  read(address: number): number {
    return this.data[address]
  }

  write(address: number, data: number): void {
    this.data[address] = data
  }

  reset(): void {
    if (!this.isResettable) { return }

    this.data = [...Array(this.lastAddress - this.firstAddress + 1)].fill(0x00)
  }

}