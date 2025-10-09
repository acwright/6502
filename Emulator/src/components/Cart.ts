export class Cart {
  
  static firstAddress: number = 0xC000
  static lastAddress: number = 0xFFFF

  data: number[] = []

  read(address: number): number {
    return this.data[address]
  }

  write(address: number, data: number): void {
    this.data[address] = data
  }

  load(data: number[]): void {
    this.data = data
  }

  reset(): void {}

}