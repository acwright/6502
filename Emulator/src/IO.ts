export interface IODescription {

  className: string
  title: string
  
}

export interface IO {

  raiseIRQ: () => void
  raiseNMI: () => void

  read(address: number): number
  write(address: number, data: number): void
  clock(cycles: number, frequency: number): void
  reset(): void
  
  description(): IODescription
  
}