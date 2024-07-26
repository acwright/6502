<template>
  <UCard>
    <template #header>
      <div class="flex flex-row items-center gap-2">
        <UIcon name="i-heroicons-clock" />
        <h1 class="text-lg">Clock Frequency</h1>
      </div>
    </template>

    <UForm 
      id="configure"
      class="space-y-4"
      :state="{}"
    >
      <UFormGroup
        label="Frequency"
        name="frequency"
      >
        <URange 
          size="sm"
          :min="1"
          :max="2000000"
          v-model="machine.frequency"
          class="mt-2 mb-2"
        />
        <span class="flex flex-row items-center justify-between">
          <UButton 
            variant="ghost"
            icon="i-heroicons-chevron-left"
            size="xs"
            @click="prevFreq"
          />
          {{ formatFrequency(machine.frequency) }}
          <UButton 
            variant="ghost"
            icon="i-heroicons-chevron-right"
            size="xs"
            @click="nextFreq"
          />
        </span>
      </UFormGroup>
    </UForm>
    
    <template #footer>
      <span class="flex flex-row justify-end">
        <UButton 
          @click="emit('close')"
        >Close</UButton>
      </span>
    </template>
  </UCard>
</template>

<script setup lang="ts">
  const emit = defineEmits<{
    (e: 'close'): void
  }>()

  const machine = useMachine()
  
  const frequencies: number[] = [
    1, 2, 4, 8, 16, 32, 64, 122, 244, 488, 976, 
    1953, 3906, 7812, 15625, 31250, 62500, 125000, 
    250000, 500000, 1000000, 2000000
  ]

  const prevFreq = () => {
    let index = frequencies.findLastIndex((freq) => {
      return freq < machine.frequency
    })
    if (index === -1) {
      machine.frequency = frequencies[frequencies.length - 1]
    } else {
      machine.frequency = frequencies[index]
    }
  }

  const nextFreq = () => {
    let index = frequencies.findIndex((freq) => {
      return freq > machine.frequency
    })
    if (index === -1) {
      machine.frequency = frequencies[0]
    } else {
      machine.frequency = frequencies[index]
    }
  }
</script>