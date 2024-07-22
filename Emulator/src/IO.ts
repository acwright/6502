export interface IO {

  id: string
  token: string
  title: string

  raiseIRQ: () => void
  raiseNMI: () => void

  read(address: number): number
  write(address: number, data: number): void
  clock(cycles: number, frequency: number): void
  reset(): void

}

export interface IODescription {

  token: string
  title: string
  
}