export class RAM {
  
  static firstAddress: number = 0x0000
  static lastAddress: number = 0x7FFF
  static RAMSize: number = RAM.lastAddress - RAM.firstAddress + 1

  data: number[] = [...Array(RAM.RAMSize)].fill(0x00)

  read(address: number): number {
    return this.data[address]
  }

  write(address: number, data: number): void {
    this.data[address] = data
  }

}