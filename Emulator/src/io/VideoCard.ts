import { v4 as uuidv4 } from 'uuid'

export class VideoCard implements IO {

  static DESCRIPTION: IODescription = { className: 'VideoCard', title: 'Video Card' }

  id: string = uuidv4()

  raiseIRQ = () => {}
  raiseNMI = () => {}

  read(address: number): number { return 0 }
  write(address: number, data: number): void {}
  clock(cycles: number, frequency: number): void {}
  reset(): void {}
  
  description(): IODescription {
    return VideoCard.DESCRIPTION
  }

}