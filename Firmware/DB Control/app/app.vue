<template>
  <UApp>
    <div v-if="connected" class="flex flex-col h-lvh">
      <LayoutHeader />
      <hr>
      <LayoutMain />
      <hr>
      <LayoutFooter />
    </div>
    <div v-else class="flex flex-col h-lvh items-center justify-center gap-4">
      <h1 class="text-4xl">DB CONTROL</h1>
      <h2 class="text-2xl text-primary">Connecting...</h2>
      <UIcon 
        name="i-lucide-refresh-ccw" 
        class="min-w-8 min-h-8"
        :class="connecting ? 'animate-spin' : ''"
      />
      <UButton v-if="!connecting" @click="connect">RETRY</UButton>
    </div>
  </UApp>
</template>

<script setup lang="ts">
  const { error: notification } = useNotifications()
  const connected = useState('connected', () => false)
  const info = useState<Info>('info')
  const connecting = ref(true)

  const connect = async () => {
    try {
      connecting.value = true
      info.value = await $fetch<Info>('http://6502.local/info')
      connected.value = true
    } catch (error) {
      notification(error)
    } finally {
      connecting.value = false
    }
  }

  onNuxtReady(connect)
</script>