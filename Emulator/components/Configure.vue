<template>
  <UCard>
    <template #header>
      <h1 class="text-lg">
        Configure
      </h1>
    </template>

    <UForm 
      id="configure"
      class="space-y-4 max-h-96 p-2 overflow-y-auto"
    >
      <UFormGroup
        label="Frequency"
        name="frequency"
      >
        <URange 
          size="sm"
          :min="1"
          :max="2000000"
          v-model="frequency"
          class="mt-2 mb-2"
        />
        <span class="flex flex-row items-center justify-between">
          <UButton 
            variant="ghost"
            icon="i-heroicons-chevron-left"
            size="xs"
            @click="prevFreq"
          />
          {{ formatFrequency(frequency) }}
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
      <span class="flex flex-row justify-between">
        <UButton
          variant="ghost"
          @click="cancel"
        >Cancel</UButton>
        <UButton 
          @click="save"
        >Save</UButton>
      </span>
    </template>
  </UCard>
</template>

<script setup lang="ts">
  const emit = defineEmits<{
    (e: 'cancel'): void,
    (e: 'save', frequency: number): void
  }>()

  const frequencies: number[] = [
    1, 2, 4, 8, 16, 32, 64, 122, 244, 488, 976, 
    1953, 3906, 7812, 15625, 31250, 62500, 125000, 
    250000, 500000, 1000000, 2000000
  ]
  const frequency = ref(1000000)

  const cancel = () => {
    emit('cancel')
  }

  const save = () => {
    emit('save', frequency.value)
  }

  const prevFreq = () => {
    let index = frequencies.findLastIndex((freq) => {
      return freq < frequency.value
    })
    if (index === -1) {
      frequency.value = frequencies[frequencies.length - 1]
    } else {
      frequency.value = frequencies[index]
    }
  }

  const nextFreq = () => {
    let index = frequencies.findIndex((freq) => {
      return freq > frequency.value
    })
    if (index === -1) {
      frequency.value = frequencies[0]
    } else {
      frequency.value = frequencies[index]
    }
  }
</script>