<template>
  <header class="flex flex-col sm:flex-row items-center justify-center sm:justify-between p-4 gap-2">
    <h1 class="text-4xl">DB CONTROL</h1>
    <div class="flex flex-row gap-2">
      <UButton 
        icon="i-lucide-refresh-ccw"
        size="md"
        color="primary"
        variant="solid"
        title="Refresh"
        @click="refresh"
      />
      <span class="text-xl">|</span>
      <UButton 
        icon="i-lucide-camera"
        size="md"
        color="primary"
        variant="solid"
        title="Snapshot"
        :disabled="info.isRunning"
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
        :disabled="info.isRunning"
        @click="run"
      />
      <UButton 
        icon="i-lucide-square"
        size="md"
        color="error"
        variant="solid" 
        title="Stop"
        :disabled="!info.isRunning"
        @click="stop"
      />
      <UButton 
        icon="i-lucide-step-forward"
        size="md"
        color="secondary"
        variant="solid"
        title="Step"
        :disabled="info.isRunning"
        @click="step"
      />
      <UButton 
        icon="i-lucide-clock"
        size="md"
        color="secondary"
        variant="solid"
        title="Tick"
        :disabled="info.isRunning"
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
  const { error: notification } = useNotifications()
  const info = useState<Info>('info')

  const refresh = async () => {
    await fetchInfo()
  }
  const snapshot = async () => {
    try {
      await $fetch(`http://${info.value.ipAddress}/control`, {
        query: {
          command: 'P'
        }
      })
    } catch (error) {
      notification(error)
    }

    await fetchInfo()
  }
  const decreaseFrequency = async () => {
    try {
      await $fetch(`http://${info.value.ipAddress}/control`, {
        query: {
          command: '-'
        }
      })
    } catch (error) {
      notification(error)
    }

    await fetchInfo()
  }
  const increaseFrequency = async () => {
    try {
      await $fetch(`http://${info.value.ipAddress}/control`, {
        query: {
          command: '+'
        }
      })
    } catch (error) {
      notification(error)
    }

    await fetchInfo()
  }
  const run = async () => {
    info.value.isRunning = true
    
    try {
      await $fetch(`http://${info.value.ipAddress}/control`, {
        query: {
          command: 'R'
        }
      })
    } catch (error) {
      notification(error)
    }
  }
  const stop = async () => {
    try {
      await $fetch(`http://${info.value.ipAddress}/control`, {
        query: {
          command: 'R'
        }
      })
    } catch (error) {
      notification(error)
    }

    await fetchInfo()
  }
  const step = async () => {
    try {
      await $fetch(`http://${info.value.ipAddress}/control`, {
        query: {
          command: 'S'
        }
      })
    } catch (error) {
      notification(error)
    }

    await fetchInfo()
  }
  const tick = async () => {
    try {
      await $fetch(`http://${info.value.ipAddress}/control`, {
        query: {
          command: 'K'
        }
      })
    } catch (error) {
      notification(error)
    }

    await fetchInfo()
  }
  const reset = async () => {
    try {
      await $fetch(`http://${info.value.ipAddress}/control`, {
        query: {
          command: 'T'
        }
      })
    } catch (error) {
      notification(error)
    }

    await fetchInfo()
  }

  const fetchInfo = async () => {
    try {
      info.value = await $fetch<Info>(`http://${info.value.ipAddress}/info`)
    } catch (error) {
      notification(error)
    }
  }
</script>