export interface IO {

  id: string
  title: string
  description: string

  raiseIRQ: () => void
  raiseNMI: () => void

  read(address: number): number
  write(address: number, data: number): void
  clock(cycles: number, frequency: number): void
  reset(): void

}