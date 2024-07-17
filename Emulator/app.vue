<template>
  <header class="flex flex-row justify-between items-center p-2 border-b-2 border-gray-800">
    <div class="flex flex-row items-center gap-2">
      <Logo class="w-6 h-6" />
      <h1 class="text-3xl">
        6502 Emulator
      </h1>
    </div>
    <Controls 
      :isRunning="isRunning" 
      @run="run"
      @stop="stop"
      @step="step"
      @reset="reset"
      @frequency="showFrequency = true"
      @load="showLoad = true"
      @configure="showConfigure = true"
      @debug="showDebug = !showDebug"
      @help="showHelp = true"
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
    <UModal v-model="showFrequency">
      <Frequency 
        @close="showFrequency = false"
      />
    </UModal>
    <UModal v-model="showLoad">
      <Load 
        @close="showLoad = false"
      />
    </UModal>
    <UModal v-model="showConfigure">
      <Configure 
        @close="showConfigure = false"
      />
    </UModal>
    <UModal v-model="showHelp">
      <Help 
        @close="showHelp = false"
      />
    </UModal>
  </div>
  <UNotifications />
</template>

<script setup lang="ts">
  const emulation = useEmulation()
  const showFrequency = ref(false)
  const showLoad = ref(false)
  const showConfigure = ref(false)
  const showDebug = ref(false)
  const showHelp = ref(false)

  const { isRunning } = storeToRefs(emulation)
  const { run, stop, reset, step } = emulation
</script>
