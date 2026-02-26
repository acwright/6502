<template>
  <div class="flex flex-col p-4">
    <h2>EMULATOR</h2>
    <div class="flex flex-col justify-evenly grow border-2 p-2">
      <br>
      <p class="text-center text-xl font-bold" :class="info.isRunning ? 'text-primary' : 'text-error'">
        {{ info.isRunning ? '-- RUNNING --' : '-- STOPPED --' }}
      </p>
      <br>
      <div class="flex flex-col w-full gap-1 text-sm">
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-1/4">Version:</span>
          <span class="text-left w-3/4">{{ info.version }}</span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-1/4">RAM:</span>
          <span class="text-left w-3/4">
            <span :class="info.ramEnabled ? 'text-primary' : 'text-gray-400'">
              {{ info.ramEnabled ? 'ENABLED' : 'DISABLED' }}
            </span>
            <span>
              ({{ info.programFile.length ? info.programFile : 'None' }})
            </span>
          </span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-1/4">ROM:</span>
          <span class="text-left w-3/4">
            <span :class="info.romEnabled ? 'text-primary' : 'text-gray-400'">
              {{ info.romEnabled ? 'ENABLED' : 'DISABLED' }}
            </span>
            <span>
              ({{ info.romFile.length ? info.romFile : 'None' }})
            </span>
          </span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-1/4">Cart:</span>
          <span class="text-left w-3/4">
            <span :class="info.cartEnabled ? 'text-primary' : 'text-gray-400'">
              {{ info.cartEnabled ? 'ENABLED' : 'DISABLED' }}
            </span>
            <span>
              ({{ info.cartFile.length ? info.cartFile : 'None' }})
            </span>
          </span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-1/4">Frequency:</span>
          <span class="text-left w-3/4">{{ info.freqLabel }} ({{ info.freqPeriod }} µS)</span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-1/4">IP Address:</span>
          <span class="text-left w-3/4">{{ info.ipAddress }}</span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-1/4">Date / Time:</span>
          <span class="text-left w-3/4">{{ new Date(info.rtc * 1000).toLocaleString() }}</span>
        </div>
      </div>
      <br>
      <div class="flex flex-row justify-center gap-2">
        <UButton 
          @click="toggleRAM" 
          :color="info.ramEnabled ? 'primary' : 'error'"
        >
          TOGGLE RAM
        </UButton>
        <UButton 
          @click="toggleROM" 
          :color="info.romEnabled ? 'primary' : 'error'"
        >
          TOGGLE ROM
        </UButton>
        <UButton
          @click="toggleCart" 
          :color="info.cartEnabled ? 'primary' : 'error'"
        >
          TOGGLE CART
        </UButton>
      </div>
      <br>
      <p class="text-center text-sm">
        <span class="font-bold pr-3">Last Snapshot:</span>
        <span :class="info.lastSnapshot > 0 ? 'text-primary' : 'text-gray-400'">
          {{ info.lastSnapshot > 0 ? info.lastSnapshot : 'None' }}
        </span>
      </p>
      <br>
    </div>
  </div>
</template>

<script setup lang="ts">
  const { error: notification } = useNotifications()
  const info = useState<Info>('info')

  const toggleRAM = async () => {
    try {
      await $fetch(`http://${info.value.ipAddress}/control`, {
        query: {
          command: 'A'
        }
      })
    } catch (error) {
      notification(error)
    }

    await fetchInfo()
  }
  const toggleROM = async () => {
    try {
      await $fetch(`http://${info.value.ipAddress}/control`, {
        query: {
          command: 'O'
        }
      })
    } catch (error) {
      notification(error)
    }

    await fetchInfo()
  }
  const toggleCart = async () => {
    try {
      await $fetch(`http://${info.value.ipAddress}/control`, {
        query: {
          command: 'L'
        }
      })
    } catch (error) {
      notification(error)
    }

    await fetchInfo()
  }
  const toggleIO = async () => {
    try {
      await $fetch(`http://${info.value.ipAddress}/control`, {
        query: {
          command: 'I'
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