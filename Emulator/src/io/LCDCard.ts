import { v4 as uuidv4 } from 'uuid'

export class LCDCard implements IO {

  id: string = uuidv4()
  token: string = 'lcd_card'
  title: string = 'LCD Card'

  raiseIRQ = () => {}
  raiseNMI = () => {}

  read(address: number): number {
    return 0
  }

  write(address: number, data: number): void {
      
  }

  clock(cycles: number, frequency: number): void {
      
  }

  reset(): void {
      
  }
  
}