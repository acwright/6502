<template>
  <div class="flex flex-col items-center p-4 gap-4">
    <div 
      class="flex flex-row items-center gap-2"
      :class="menuWidth"
    >
      <UIcon name="i-heroicons-tv" />
      <USelectMenu 
        size="sm"
        v-model="index"
        :options="slots"
        value-attribute="index"
        option-attribute="name"
        class="w-full"
      />
    </div>
    <div class="flex flex-col w-full h-full items-center justify-center">
      <IoDevBoard v-if="machine.io[index].description().className === 'DevBoard'" />
      <IoEmpty v-if="machine.io[index].description().className === 'Empty'" />
      <IoGPIOCard v-if="machine.io[index].description().className === 'GPIOCard'" />
      <IoInputBoard v-if="machine.io[index].description().className === 'InputBoard'" />
      <IoLCDCard v-if="machine.io[index].description().className === 'LCDCard'" />
      <IoPiCard v-if="machine.io[index].description().className === 'PiCard'" />
      <IoSerialCard v-if="machine.io[index].description().className === 'SerialCard'" />
      <IoSoundCard v-if="machine.io[index].description().className === 'SoundCard'" />
      <IoStorageCard v-if="machine.io[index].description().className === 'StorageCard'" />
      <IoTeensyCard v-if="machine.io[index].description().className === 'TeensyCard'" />
      <IoVGACard v-if="machine.io[index].description().className === 'VGACard'" />
      <IoVideoCard v-if="machine.io[index].description().className === 'VideoCard'" />
    </div>
  </div>
</template>

<script setup lang="ts">
  const props = defineProps<{
    modelValue: number,
    gridIndex: number,
    gridSelection: number
  }>()
  const emit = defineEmits<{
    (e: 'update:modelValue', value: number): void
  }>()

  const machine = useMachine()
  const slots = computed(() => {
    return machine.io.map((io, index) => {
      const description = io.description()
      return {
        name: `${description.title} - Slot ${index + 1} (${formatHex(0x8000 + (index * 0x400))})`,
        index: index
      }
    })
  })
  const index = ref(props.modelValue)

  const menuWidth = computed(() => {
    if (props.gridSelection > 4) {
      return 'w-full'
    } else if (props.gridSelection > 1) {
      return 'w-1/2'
    } else {
      return 'w-1/4'
    }
  })

  watch(index, () => {
    emit('update:modelValue', index.value)
  })
</script>