<template>
  <div class="flex flex-col p-4">
    <h2>EMULATOR</h2>
    <div class="flex flex-col justify-evenly grow border-2 p-2">
      <br>
      <p class="text-center text-xl font-bold" :class="info.isRunning ? 'text-primary' : 'text-error'">
        {{ info.isRunning ? '-- RUNNING --' : '-- STOPPED --' }}
      </p>
      <br>
      <div class="flex flex-col ml-auto mr-auto gap-1 text-sm">
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">Version:</span>
          <span>{{ info.version }}</span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">RAM:</span>
          <span :class="info.programFile != 'None' ? 'text-primary' : 'text-gray-400'">
            {{ info.programFile.length ? info.programFile : 'None' }}
          </span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">ROM:</span>
          <span :class="info.romFile != 'None' ? 'text-primary' : 'text-gray-400'">
            {{ info.romFile.length ? info.romFile : 'None' }}
          </span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">Cart:</span>
          <span :class="info.cartFile != 'None' ? 'text-primary' : 'text-gray-400'">
            {{ info.cartFile.length ? info.cartFile : 'None' }}
          </span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">Frequency:</span>
          <span>{{ info.freqLabel }} ({{ info.freqPeriod }} µS)</span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">IP Address:</span>
          <span>{{ info.ipAddress }}</span>
        </div>
        <div class="flex flex-row gap-3">
          <span class="text-right font-bold w-28">Date / Time:</span>
          <span>{{ new Date(info.rtc * 1000).toLocaleString() }}</span>
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
      await $fetch('/api/control', {
        query: {
          ipAddress: info.value.ipAddress,
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
      await $fetch('/api/control', {
        query: {
          ipAddress: info.value.ipAddress,
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
      await $fetch('/api/control', {
        query: {
          ipAddress: info.value.ipAddress,
          command: 'L'
        }
      })
    } catch (error) {
      notification(error)
    }

    await fetchInfo()
  }

  const fetchInfo = async () => {
    try {
      info.value = await $fetch<Info>('/api/info', {
        query: {
          ipAddress: info.value.ipAddress
        }
      })
    } catch (error) {
      notification(error)
    }
  }
</script>