import { defineStore } from 'pinia'

export const useEmulation = defineStore('emulation', () => {
  const machine = new Machine()
  
  const notifications = useNotifications()

  const isRunning = ref(false)
  const frequency = ref(1000000)

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

  const load = async (rom?: File, cart?: File) => {
    if (!rom) { return }

    const romData = Array.from(new Uint8Array(await rom.arrayBuffer()))

    try {
      machine.rom.load(romData)
    } catch(error) {
      notifications.error(error)
    }

    if (cart) {
      const cartData = Array.from(new Uint8Array(await cart.arrayBuffer()))

      const _cart = new Cart()
      _cart.load(cartData)

      machine.cart = _cart
    }
  }

  const configure = () => {
    
  }

  return { 
    isRunning,
    frequency,
    run,
    stop,
    reset,
    step,
    load,
    configure
  }
})