<template>
  <div class="flex flex-col items-center p-4 gap-4">
    <div 
      class="flex flex-row items-center gap-2"
      :class="menuWidth"
    >
      <UIcon name="i-heroicons-tv" />
      <USelectMenu 
        size="sm" 
        v-model="devices[props.index]" 
        :options="formattedSlots" 
        value-attribute="value"
        option-attribute="name"
        class="w-full"
      />
    </div>
    <div class="w-full h-full">

    </div>
  </div>
</template>

<script setup lang="ts">
  const props = defineProps<{
    index: number,
    gridSelection: number
  }>()

  const machine = useMachine()
  const slots = ref<IODescription[]>(machine.io.map((io) => {
    return Machine.IO_DESCRIPTIONS.find((io_desc) => {
      return io_desc.token === io.token
    }) || Machine.IO_DESCRIPTIONS[0]
  }))
  const devices = ref<IODescription[]>(machine.io.map((io) => {
    return Machine.IO_DESCRIPTIONS.find((io_desc) => {
      return io_desc.token === io.token
    }) || Machine.IO_DESCRIPTIONS[0]
  }))

  const formattedSlots = computed(() => {
    return slots.value.map((slot, index) => {
      return {
        name: `${slot.title} - Slot ${index + 1} (${formatHex(0x8000 + (index * 0x400))})`,
        value: slot
      }
    })
  })

  const menuWidth = computed(() => {
    if (props.gridSelection > 4) {
      return 'w-full'
    } else if (props.gridSelection > 1) {
      return 'w-1/2'
    } else {
      return 'w-1/4'
    }
  })
</script>