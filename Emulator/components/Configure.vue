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
        <USelectMenu 
          v-model="slots[0]" 
          :options="Machine.IO_DESCRIPTIONS"
          option-attribute="title"
          @change="onChange(0, $event)"
        />
      </UFormGroup>
      <UFormGroup
        label="Slot 2"
        hint="$8400"
        name="slot2"
      >
        <USelectMenu 
          v-model="slots[1]" 
          :options="Machine.IO_DESCRIPTIONS"
          option-attribute="title"
          @change="onChange(1, $event)"
        />
      </UFormGroup>
      <UFormGroup
        label="Slot 3"
        hint="$8800"
        name="slot3"
      >
        <USelectMenu 
          v-model="slots[2]" 
          :options="Machine.IO_DESCRIPTIONS"
          option-attribute="title"
          @change="onChange(2, $event)"
        />
      </UFormGroup>
      <UFormGroup
        label="Slot 4"
        hint="$8C00"
        name="slot4"
      >
        <USelectMenu 
          v-model="slots[3]" 
          :options="Machine.IO_DESCRIPTIONS"
          option-attribute="title"
          @change="onChange(3, $event)"
        />
      </UFormGroup>
      <UFormGroup
        label="Slot 5"
        hint="$9000"
        name="slot5"
      >
        <USelectMenu 
          v-model="slots[4]" 
          :options="Machine.IO_DESCRIPTIONS"
          option-attribute="title"
          @change="onChange(4, $event)"
        />
      </UFormGroup>
      <UFormGroup
        label="Slot 6"
        hint="$9400"
        name="slot6"
      >
        <USelectMenu 
          v-model="slots[5]" 
          :options="Machine.IO_DESCRIPTIONS"
          option-attribute="title"
          @change="onChange(5, $event)"
        />
      </UFormGroup>
      <UFormGroup
        label="Slot 7"
        hint="$9800"
        name="slot3"
      >
        <USelectMenu 
          v-model="slots[6]" 
          :options="Machine.IO_DESCRIPTIONS"
          option-attribute="title"
          @change="onChange(6, $event)"
        />
      </UFormGroup>
      <UFormGroup
        label="Slot 8"
        hint="$9C00"
        name="slot8"
      >
        <USelectMenu 
          v-model="slots[7]" 
          :options="Machine.IO_DESCRIPTIONS"
          option-attribute="title"
          @change="onChange(7, $event)"
        />
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
  const slots = computed(() => {
    return machine.io.map((io) => {
      return io.description()
    })
  })

  const onChange = (slot: number, description: IODescription) => {
    machine.loadSlot(slot, description)
  }
</script>