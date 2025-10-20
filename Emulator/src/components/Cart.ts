export class Cart {
  
  static START: number = 0x8000
  static END: number = 0xFFFF
  static CODE: number = 0xC000
  static SIZE: number = Cart.END - Cart.START + 1

  data: number[] = []

  read(address: number): number {
    return this.data[address]
  }

  write(address: number, data: number): void {
    this.data[address] = data
  }

  load(data: number[]): void {
    if (data.length != Cart.SIZE) { return }

    this.data = data
  }

  reset(): void {}

}