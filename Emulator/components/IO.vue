<template>
  <div class="flex flex-col grow">
    <div class="flex flex-row justify-between items-center p-2 border-b-2 border-gray-800">
      <h2 class="text-lg">IO</h2>
      <div class="flex flex-row items-center gap-2">
        <UIcon name="i-heroicons-squares-2x2" />
        <USelectMenu size="xs" v-model="gridSelection" :options="grids" class="w-16" />
      </div>
    </div>
    <div 
      class="grid grow"
      :class="gridCols + (gridSelection > 1 ? ' divide-x divide-y divide-gray-900' : '')"
    >
      <IOSlot />
      <IOSlot v-if="gridSelection > 1" />
      <IOSlot v-if="gridSelection > 2" />
      <IOSlot v-if="gridSelection > 2" />
      <IOSlot v-if="gridSelection > 4" />
      <IOSlot v-if="gridSelection > 4" />
      <IOSlot v-if="gridSelection > 4" />
      <IOSlot v-if="gridSelection > 4" />
    </div>
  </div>
</template>

<script setup lang="ts">
  const grids = [1, 2, 4, 8]
  const gridSelection = ref(grids[0])
  
  const gridCols = computed(() => {
    switch(gridSelection.value) {
      case 1:
        return 'grid-cols-1'
      case 2:
        return 'grid-cols-2'
      case 4:
        return 'grid-cols-2'
      case 8:
        return 'grid-cols-4'
    }
  })
</script>