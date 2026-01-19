<template>
  <div class="flex flex-col p-4">
    <h2>EMULATOR</h2>
    <div class="flex flex-col justify-evenly grow border-2 p-2">
      <br>
      <p class="text-center text-xl font-bold" :class="store.isRunning ? 'text-primary' : 'text-error'">
        {{ store.isRunning ? '-- RUNNING --' : '-- STOPPED --' }}
      </p>
      <br>
      <div class="flex flex-col ml-auto mr-auto gap-1 text-sm">
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">Version:</span>
          <span>{{ store.version }}</span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">RAM:</span>
          <span :class="store.programFile != 'None' ? 'text-primary' : 'text-gray-400'">
            {{ store.programFile.length ? store.programFile : 'None' }}
          </span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">ROM:</span>
          <span :class="store.romFile != 'None' ? 'text-primary' : 'text-gray-400'">
            {{ store.romFile.length ? store.romFile : 'None' }}
          </span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">Cart:</span>
          <span :class="store.cartFile != 'None' ? 'text-primary' : 'text-gray-400'">
            {{ store.cartFile.length ? store.cartFile : 'None' }}
          </span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">Frequency:</span>
          <span>{{ store.freqLabel }} ({{ store.freqPeriod }} µS)</span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">IP Address:</span>
          <span>{{ store.ipAddress }}</span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">Date / Time:</span>
          <span>{{ new Date(store.rtc * 1000).toLocaleString() }}</span>
        </div>
      </div>
      <br>
      <div class="flex flex-row justify-center gap-2">
        <UButton 
          @click="toggleRAM" 
          :color="store.ramEnabled ? 'primary' : 'error'"
        >
          {{ store.ramEnabled ? 'DISABLE' : 'ENABLE' }} RAM
        </UButton>
        <UButton 
          @click="toggleROM" 
          :color="store.romEnabled ? 'primary' : 'error'"
        >
          {{ store.romEnabled ? 'DISABLE' : 'ENABLE' }} ROM
        </UButton>
        <UButton
          @click="toggleCart" 
          :color="store.cartEnabled ? 'primary' : 'error'"
        >
          {{ store.cartEnabled ? 'DISABLE' : 'ENABLE' }} CART
        </UButton>
      </div>
      <br>
      <p class="text-center text-sm">
        <span class="font-bold pr-3">Last Snapshot:</span>
        <span :class="store.lastSnapshot > 0 ? 'text-primary' : 'text-gray-400'">
          {{ store.lastSnapshot > 0 ? store.lastSnapshot : 'None' }}
        </span>
      </p>
      <br>
    </div>
  </div>
</template>

<script setup lang="ts">
  const store = useControlStore()

  const toggleRAM = async () => {
    console.log('toggle RAM')
    store.ramEnabled = !store.ramEnabled
  }
  const toggleROM = async () => {
    console.log('toggle ROM')
    store.romEnabled = !store.romEnabled
  }
  const toggleCart = async () => {
    console.log('toggle Cart')
    store.cartEnabled = !store.cartEnabled
  }
</script>