<template>
  <div class="flex flex-col p-4">
    <h2>MEMORY</h2>
    <div class="flex flex-col justify-between grow border-2 pl-4 pr-4 pt-8 pb-8">
      <UFieldGroup orientation="horizontal" size="sm" class="flex flex-row justify-center">
        <UButton 
          @click="onTarget('ram')"
          :color="target == 'ram' ? 'primary' : 'neutral'"
          :variant="target == 'ram' ? 'solid' : 'outline'"
          label="RAM"
        />
        <UButton 
          @click="onTarget('rom')"
          :color="target == 'rom' ? 'primary' : 'neutral'"
          :variant="target == 'rom' ? 'solid' : 'outline'"
          label="ROM"
        />
      </UFieldGroup>
      <br>
      <div class="flex flex-row ml-auto mr-auto gap-3 text-xs">
        <div class="grid grid-rows-16 min-w-3/32 gap-1 text-right font-bold">
          <div v-for="address in addresses">${{ address }}:</div>
        </div>
        <div class="grid grid-cols-16 min-w-21/32 gap-1 text-center">
          <div v-for="byte in data">{{ byte }}</div>
        </div>
        <div class="grid grid-cols-16 min-w-8/32 gap-x-0 gap-y-1 text-center text-gray-400">
          <div v-for="_char in ascii" class="w-1">{{ _char }}</div>
        </div>
      </div>
      <br>
      <UPagination v-model:page="page" :items-per-page="256" :total="32768" size="sm" class="flex flex-row justify-center"/>
    </div>
  </div>
</template>

<script setup lang="ts">
  const { error: notification } = useNotifications()
  const info = useState<Info>('info')
  const addresses = ref<string[]>([])
  const data = ref<string[]>([])
  const ascii = ref<string[]>([])
  const target = ref<'ram' | 'rom'>('ram')
  const page = ref(1)
  
  const onTarget = (_target: 'ram' | 'rom') => {
    target.value = _target
  }

  watch([info, target, page], async () => {
    try {
      const blob = await $fetch<Blob>(`http://${info.value.ipAddress}/memory`, {
        headers: {
          'Accept': 'application/octet-stream',
        },
        responseType: 'blob',
        query: {
          target: target.value,
          page: page.value - 1
        }
      })
      const buffer = await blob.arrayBuffer()
      const bytes: Uint8Array = new Uint8Array(buffer)

      for (let i = 0; i < 16; i++) {
        switch (target.value) {
          case 'ram':
            addresses.value[i] = (((page.value - 1) * 0x100) + (i * 16)).toString(16).padStart(4, '0').toUpperCase()
            break
          case 'rom':
            addresses.value[i] = (((page.value - 1) * 0x100) + 32768 + (i * 16)).toString(16).padStart(4, '0').toUpperCase()
            break
        }
      }
      for (let i = 0; i < 256; i++) {
        data.value[i] = bytes[i]?.toString(16).padStart(2,'0').toUpperCase() || '00'
      }
      for (let i = 0; i < 256; i++) {
        let value = String.fromCharCode(bytes[i] || 0)
        ascii.value[i] = value !== "\u0000" ? value : '.'
      }
    } catch (error) {
      notification(error)
    }
  }, { immediate: true })
</script>