import { v4 as uuidv4 } from 'uuid'

export class Empty implements IO {

  id: string = uuidv4()
  token: string = 'empty'
  title: string = 'Empty'

  raiseIRQ = () => {}
  raiseNMI = () => {}

  read(address: number): number { return 0 }
  write(address: number, data: number): void {}
  clock(cycles: number, frequency: number): void {}
  reset(): void {}
  
}