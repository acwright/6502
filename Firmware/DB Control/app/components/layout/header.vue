<template>
  <header class="flex flex-col sm:flex-row items-center justify-center sm:justify-between p-4 gap-2">
    <h1 class="text-4xl">DB CONTROL</h1>
    <div class="flex flex-row gap-2">
      <UButton 
        icon="i-lucide-refresh-ccw"
        size="md"
        color="primary"
        variant="solid"
        title="Snapshot"
        @click="refresh"
      />
      <span class="text-xl">|</span>
      <UButton 
        icon="i-lucide-camera"
        size="md"
        color="primary"
        variant="solid"
        title="Snapshot"
        :disabled="store.isRunning"
        @click="snapshot"
      />
      <span class="text-xl">|</span>
      <UButton 
        icon="i-lucide-clock-arrow-down"
        size="md"
        color="secondary"
        variant="solid"
        title="Decrease Frequency"
        @click="decreaseFrequency"
      />
      <UButton 
        icon="i-lucide-clock-arrow-up"
        size="md"
        color="secondary"
        variant="solid"
        title="Increase Frequency"
        @click="increaseFrequency"
      />
      <span class="text-xl">|</span>
      <UButton 
        icon="i-lucide-play"
        size="md"
        color="primary"
        variant="solid"
        title="Run"
        :disabled="store.isRunning"
        @click="run"
      />
      <UButton 
        icon="i-lucide-square"
        size="md"
        color="error"
        variant="solid" 
        title="Stop"
        :disabled="!store.isRunning"
        @click="stop"
      />
      <UButton 
        icon="i-lucide-step-forward"
        size="md"
        color="secondary"
        variant="solid"
        title="Step"
        :disabled="store.isRunning"
        @click="step"
      />
      <UButton 
        icon="i-lucide-clock"
        size="md"
        color="secondary"
        variant="solid"
        title="Tick"
        :disabled="store.isRunning"
        @click="tick"
      />
      <UButton 
        icon="i-lucide-rotate-ccw"
        size="md"
        color="error"
        variant="solid"
        title="Reset"
        @click="reset"
      />
    </div>
  </header>
</template>

<script setup lang="ts">
  const store = useControlStore()

  const refresh = async () => {
    await store.fetchInfo()
  }
  const snapshot = async () => {
    await store.doControl('P')
    await store.fetchInfo()
  }
  const decreaseFrequency = async () => {
    await store.doControl('-')
    await store.fetchInfo()
  }
  const increaseFrequency = async () => {
    await store.doControl('+')
    await store.fetchInfo()
  }
  const run = async () => {
    store.isRunning = true
    await store.doControl('R')
  }
  const stop = async () => {
    await store.doControl('R')
    await store.fetchInfo()
  }
  const step = async () => {
    await store.doControl('S')
    await store.fetchInfo()
  }
  const tick = async () => {
    await store.doControl('K')
    await store.fetchInfo()
  }
  const reset = async () => {
    await store.doControl('T')
    await store.fetchInfo()
  }
</script>