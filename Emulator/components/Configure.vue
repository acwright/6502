<template>
  <UCard>
    <template #header>
      <div class="flex flex-row items-center gap-2">
        <UIcon name="i-heroicons-tv" />
        <h1 class="text-lg">Configure IO</h1>
      </div>
    </template>

    <UForm 
      id="configure"
      class="space-y-4"
    >
      <UFormGroup
        label="Slot 1"
        hint="$8000"
        name="slot1"
      >
        <USelectMenu v-model="slot1" :options="ios" />
      </UFormGroup>
      <UFormGroup
        label="Slot 2"
        hint="$8400"
        name="slot2"
      >
        <USelectMenu v-model="slot2" :options="ios" />
      </UFormGroup>
      <UFormGroup
        label="Slot 3"
        hint="$8800"
        name="slot3"
      >
        <USelectMenu v-model="slot3" :options="ios" />
      </UFormGroup>
      <UFormGroup
        label="Slot 4"
        hint="$8C00"
        name="slot4"
      >
        <USelectMenu v-model="slot4" :options="ios" />
      </UFormGroup>
      <UFormGroup
        label="Slot 5"
        hint="$9000"
        name="slot5"
      >
        <USelectMenu v-model="slot5" :options="ios" />
      </UFormGroup>
      <UFormGroup
        label="Slot 6"
        hint="$9400"
        name="slot6"
      >
        <USelectMenu v-model="slot6" :options="ios" />
      </UFormGroup>
      <UFormGroup
        label="Slot 7"
        hint="$9800"
        name="slot3"
      >
        <USelectMenu v-model="slot7" :options="ios" />
      </UFormGroup>
      <UFormGroup
        label="Slot 8"
        hint="$9C00"
        name="slot8"
      >
        <USelectMenu v-model="slot8" :options="ios" />
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
    (e: 'close'): void
  }>()

  const ios = [
    'Empty',
    'Dev Board',
    'GPIO Card',
    'Input Board',
    'LCD Card',
    'Pi Card',
    'Serial Card',
    'Sound Card',
    'Storage Card',
    'Teensy Card',
    'VGA Card',
    'Video Card'
  ]
  const slot1 = ref(ios[4])
  const slot2 = ref(ios[8])
  const slot3 = ref(ios[7])
  const slot4 = ref(ios[11])
  const slot5 = ref(ios[6])
  const slot6 = ref(ios[0])
  const slot7 = ref(ios[0])
  const slot8 = ref(ios[1])

  const emulation = useEmulation()
  const { configure } = emulation

  const cancel = () => {
    emit('close')
  }

  const save = () => {
    configure()
    emit('close')
  }
</script>