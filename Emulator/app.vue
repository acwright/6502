<template>
  <header class="flex flex-row justify-between items-center p-2 border-b-2 border-gray-800">
    <div class="flex flex-row items-center gap-2">
      <Logo class="w-6 h-6" />
      <h1 class="text-3xl">
        6502 Emulator
      </h1>
    </div>
    <Controls 
      :isRunning="machine.isRunning" 
      @run="machine.run()"
      @stop="machine.stop()"
      @step="machine.step()"
      @reset="machine.reset()"
      @clock="showClock = true"
      @load="showLoad = true"
      @configure="showConfigure = true"
      @debug="showDebug = !showDebug"
      @help="showHelp = true"
    />
  </header>
  <main class="flex flex-row grow">
    <Devices 
      :class="showDebug ? 'w-2/3' : 'w-full'" 
    />
    <Debug 
      v-if="showDebug"
      class="w-1/3"
    />
  </main>
  <div>
    <UModal v-model="showClock">
      <Clock 
        @close="showClock = false"
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
  const machine = useMachine()
  const showClock = ref(false)
  const showLoad = ref(false)
  const showConfigure = ref(false)
  const showDebug = ref(false)
  const showHelp = ref(false)
</script>
