import { v4 as uuidv4 } from 'uuid'
import { IO, IODescription } from '../IO'

export class TeensyCard implements IO {

  static DESCRIPTION: IODescription = { className: 'TeensyCard', title: 'Teensy Card' }

  id: string = uuidv4()

  raiseIRQ = () => {}
  raiseNMI = () => {}

  read(address: number): number { return 0 }
  write(address: number, data: number): void {}
  clock(cycles: number, frequency: number): void {}
  reset(): void {}
  
  description(): IODescription {
    return TeensyCard.DESCRIPTION
  }

}