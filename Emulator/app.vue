<template>
  <header class="flex flex-row justify-between items-center p-2 border-b-2 border-gray-800">
    <h1 class="text-3xl">
      6502 Emulator
    </h1>
    <Controls 
      :isRunning="isRunning" 
      @run="run()"
      @stop="stop()"
      @step="step()"
      @reset="reset()"
      @load="load"
      @configure="configure"
      @debug="debug"
    />
  </header>
  <main class="flex flex-row grow">
    <IO 
      :class="showDebug ? 'w-2/3' : 'w-full'" 
    />
    <Debug 
      v-if="showDebug"
      class="w-1/3"
    />
  </main>
  <div>
    <UModal v-model="showLoad">
      <Load 
        @save="onLoad"
        @cancel="showLoad = false"
      />
    </UModal>
    <UModal v-model="showConfigure">
      <Configure 
        @save="onConfigure"
        @cancel="showConfigure = false"
      />
    </UModal>
  </div>
  <UNotifications />
</template>

<script setup lang="ts">
  const notifications = useNotifications()
  const machine = useMachine()
  const isRunning = ref(false)
  const showDebug = ref(false)
  const showLoad = ref(false)
  const showConfigure = ref(false)

  const run = () => {
    isRunning.value = true
  }

  const stop = () => {
    isRunning.value = false
  }

  const step = () => {
    machine.step()
  }

  const reset = () => {
    machine.reset()
  }
  
  const load = () => {
    showLoad.value = true
  }

  const configure = () => {
    showConfigure.value = true
  }

  const debug = () => {
    showDebug.value = !showDebug.value
  }

  const onLoad = async (rom?: File, cart?: File) => {
    showLoad.value = false

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

  const onConfigure = (frequency: number) => {
    
  }
</script>
