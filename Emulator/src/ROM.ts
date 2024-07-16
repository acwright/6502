export class ROM {
  
  static firstAddress: number = 0x8000
  static lastAddress: number = 0xFFFF
  static codeAddress: number = 0xA000
  static ROMSize: number = ROM.lastAddress - ROM.firstAddress + 1

  data: number[] = [...Array(ROM.ROMSize)].fill(0xEA)

  read(address: number): number {
    return this.data[address]
  }

  load(data: number[]): void {
    if (data.length != ROM.ROMSize) { 
      throw new RangeError('Data length must equal ROM address range') 
    }

    this.data = data
  }

}