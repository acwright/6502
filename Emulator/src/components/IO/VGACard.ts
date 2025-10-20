import { v4 as uuidv4 } from 'uuid'
import { IO, IODescription } from '../IO'

export class VGACard implements IO {

  static DESCRIPTION: IODescription = { className: 'VGACard', title: 'VGA Card' }

  id: string = uuidv4()

  raiseIRQ = () => {}
  raiseNMI = () => {}

  read(address: number): number { return 0 }
  write(address: number, data: number): void {}
  step(cycles: number, frequency: number): void {}
  reset(): void {}
  
  description(): IODescription {
    return VGACard.DESCRIPTION
  }

}