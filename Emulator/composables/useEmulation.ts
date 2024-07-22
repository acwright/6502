import { defineStore } from 'pinia'

export const useEmulation = defineStore('emulation', () => {
  const IODEVICES = [
    'Empty',
    'Dev Board',
    'GPIO Card',
    'Input Board',
    'LCD Card',
    'Pi Card',
    'Serial Card',
    'Sound Card',
    'Storage Card',
    'Teensy Card',
    'VGA Card',
    'Video Card'
  ]

  const machine = new Machine()

  const isRunning = ref(false)
  const frequency = ref(1000000)
  const rom = ref<File>()
  const cart = ref<File>()

  const slots = reactive<string[]>([
    IODEVICES[4], IODEVICES[8], IODEVICES[7], IODEVICES[11],
    IODEVICES[6], IODEVICES[0], IODEVICES[0], IODEVICES[1]
  ])
  const devices = reactive<string[]>([
    IODEVICES[4], IODEVICES[8], IODEVICES[7], IODEVICES[11],
    IODEVICES[6], IODEVICES[0], IODEVICES[0], IODEVICES[1]
  ])

  const run = () => {
    isRunning.value = true
  }

  const stop = () => {
    isRunning.value = false
  }

  const reset = () => {
    machine.reset()
  }

  const step = () => {
    machine.step()
  }

  const loadROM = async (_rom: File) => {
    rom.value = _rom

    const romData = Array.from(new Uint8Array(await _rom.arrayBuffer()))

    machine.loadROM(romData)
  }

  const loadCart = async (_cart: File) => {
    cart.value = _cart
    
    const cartData = Array.from(new Uint8Array(await _cart.arrayBuffer()))

    machine.loadCart(cartData)
  }

  return { 
    IODEVICES,
    isRunning,
    frequency,
    slots,
    devices,
    rom,
    cart,
    run,
    stop,
    reset,
    step,
    loadROM,
    loadCart
  }
})