<template>
  <div class="flex flex-col items-center p-4 gap-4">
    <div class="flex flex-row items-center gap-2">
      <UIcon name="i-heroicons-tv" />
      <USelectMenu 
        size="sm" 
        v-model="devices[props.index]" 
        :options="formattedSlots" 
        value-attribute="value"
        option-attribute="name"
      />
    </div>
    <div class="w-full h-full"></div>
  </div>
</template>

<script setup lang="ts">
import { formatHex } from '~/utils/formatHex';

  const props = defineProps<{
    index: number
  }>()

  const emulation = useEmulation()
  const { slots, devices } = emulation

  const formattedSlots = computed(() => {
    return slots.map((slot, index) => {
      return {
        name: `${slot} - Slot ${ index + 1} (${formatHex(0x8000 + (index * 0x400))})`,
        value: slot
      }
    })
  })
</script>