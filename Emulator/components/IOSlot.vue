<template>
  <div class="flex flex-col items-center p-4 gap-4">
    <div class="flex flex-row items-center gap-2">
      <UIcon name="i-heroicons-tv" />
      <USelectMenu size="sm" v-model="ioSelection" :options="ios" />
    </div>
    <div class="w-full h-full"></div>
  </div>
</template>

<script setup lang="ts">
  const ios = [
    'Slot 1 - LCD Card ($8000)',
    'Slot 2 - Storage Card ($8400)',
    'Slot 3 - Sound Card ($8800)',
    'Slot 4 - VGA Card ($8C00)',
    'Slot 5 - Serial Card ($9000)',
    'Slot 6 - Empty ($9400)',
    'Slot 7 - Input Board ($9800)',
    'Slot 8 - Dev Board ($9C00)'
  ]
  const ioSelection = ref(ios[0])
</script>