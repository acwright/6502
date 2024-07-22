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
      :state="{}"
    >
      <UFormGroup
        label="Slot 1"
        hint="$8000"
        name="slot1"
      >
        <USelectMenu v-model="slots[0]" :options="IODEVICES" />
      </UFormGroup>
      <UFormGroup
        label="Slot 2"
        hint="$8400"
        name="slot2"
      >
        <USelectMenu v-model="slots[1]" :options="IODEVICES" />
      </UFormGroup>
      <UFormGroup
        label="Slot 3"
        hint="$8800"
        name="slot3"
      >
        <USelectMenu v-model="slots[2]" :options="IODEVICES" />
      </UFormGroup>
      <UFormGroup
        label="Slot 4"
        hint="$8C00"
        name="slot4"
      >
        <USelectMenu v-model="slots[3]" :options="IODEVICES" />
      </UFormGroup>
      <UFormGroup
        label="Slot 5"
        hint="$9000"
        name="slot5"
      >
        <USelectMenu v-model="slots[4]" :options="IODEVICES" />
      </UFormGroup>
      <UFormGroup
        label="Slot 6"
        hint="$9400"
        name="slot6"
      >
        <USelectMenu v-model="slots[5]" :options="IODEVICES" />
      </UFormGroup>
      <UFormGroup
        label="Slot 7"
        hint="$9800"
        name="slot3"
      >
        <USelectMenu v-model="slots[6]" :options="IODEVICES" />
      </UFormGroup>
      <UFormGroup
        label="Slot 8"
        hint="$9C00"
        name="slot8"
      >
        <USelectMenu v-model="slots[7]" :options="IODEVICES" />
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

  const emulation = useEmulation()
  const { IODEVICES } = emulation
  const { slots } = storeToRefs(emulation)
</script>