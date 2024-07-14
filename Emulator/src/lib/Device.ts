import { Bus } from './Bus'

export interface Device {

  id: string
  title: string
  description: string
  bus?: Bus

  read?(address: number): number
  write?(address: number, data: number): void
  clock?(cycles: number, frequency: number): void
  reset?(): void
  irq?(): void
  nmi?(): void
}