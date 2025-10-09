import { v4 as uuidv4 } from 'uuid'
import { IO, IODescription } from '../IO'

export class SoundCard implements IO {

  static DESCRIPTION: IODescription = { className: 'SoundCard', title: 'Sound Card' }

  id: string = uuidv4()

  raiseIRQ = () => {}
  raiseNMI = () => {}

  read(address: number): number { return 0 }
  write(address: number, data: number): void {}
  clock(cycles: number, frequency: number): void {}
  reset(): void {}
  
  description(): IODescription {
    return SoundCard.DESCRIPTION
  }

}