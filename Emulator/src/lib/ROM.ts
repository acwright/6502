import { Device } from './Device'
import { v4 as uuidv4 } from 'uuid'

export class ROM implements Device {

  id: string = uuidv4()
  title: string = "ROM"
  description: string = "Read-Only Memory"
  
  firstAddress: number
  lastAddress: number
  data: number[]

  constructor(
    firstAddress: number = 0xA000, 
    lastAddress: number = 0xFFFF,
    data: number[] = [...Array(lastAddress - firstAddress + 1)].fill(0xEA)
  ) {
    if (data.length != lastAddress - firstAddress + 1) { 
      throw new RangeError('Data length must equal address range') 
    }

    this.firstAddress = firstAddress
    this.lastAddress = lastAddress
    this.data = data
  }

  read(address: number): number {
    return this.data[address]
  }

  load(data: number[]) {
    if (data.length != this.lastAddress - this.firstAddress + 1) { 
      throw new RangeError('Data length must equal address range') 
    }

    this.data = data
  }

}