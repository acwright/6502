<template>
  <UCard>
    <template #header>
      <h1 class="text-lg">
        Load
      </h1>
    </template>

    <UForm 
      id="load"
      :state="state" 
      :validate="validate" 
      validateOn="submit"
      class="space-y-4 max-h-96 p-2 overflow-y-auto"
      @submit="save"
    >
      <UFormGroup
        label="ROM"
        name="rom"
      >
        <UInput
          type="file" 
          icon="i-heroicons-folder"
          accept="application/octet-stream"
          v-model="state.rom"
          @change="onRom"
        />
      </UFormGroup>

      <UFormGroup
        label="Cartridge"
        name="cart"
      >
        <UInput
          type="file" 
          icon="i-heroicons-folder"
          accept="application/octet-stream"
          v-model="state.cart"
          @change="onCart"
        />
      </UFormGroup>
    </UForm>
    
    <template #footer>
      <span class="flex flex-row justify-between">
        <UButton
          variant="ghost"
          @click="cancel"
        >Cancel</UButton>
        <UButton 
          type="submit"
          form="load"
        >Save</UButton>
      </span>
    </template>
  </UCard>
</template>

<script setup lang="ts">
  import type { FormError } from '#ui/types'

  const emit = defineEmits<{
    (e: 'cancel'): void,
    (e: 'save', rom?: File, cart?: File): void
  }>()

  const state = reactive<{
    rom: string | undefined,
    cart: string | undefined
  }>({
    rom: undefined,
    cart: undefined
  })

  const rom = ref<File>()
  const cart = ref<File>()

  const cancel = () => {
    emit('cancel')
  }

  const save = () => {
    emit('save', rom.value, cart.value)
  }

  const onRom = (files: FileList) => {
    rom.value = files[0]
  }

  const onCart = (files: FileList) => {
    cart.value = files[0]
  }

  const validate = (state: any): FormError[] => {
    const errors = []
    if (!state.rom) errors.push({ path: 'rom', message: 'Required' })
    return errors
  }
</script>