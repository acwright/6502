<template>
  <UCard>
    <template #header>
      <div class="flex flex-row items-center gap-2">
        <UIcon name="i-heroicons-folder" />
        <h1 class="text-lg">Load ROM</h1>
      </div>
    </template>

    <UForm 
      id="load"
      :state="{}"
      :validate="validate"
      :validateOn="['input']"
      class="space-y-4"
    >
      <UFormGroup
        label="ROM"
        name="rom"
        :hint="rom?.name || 'Empty'"
      >
        <UInput
          type="file" 
          icon="i-heroicons-folder"
          accept="application/octet-stream"
          @change="onRom"
        />
      </UFormGroup>

      <UFormGroup
        label="Cartridge"
        name="cart"
        :hint="cart?.name || 'Empty'"
      >
        <UInput
          type="file" 
          icon="i-heroicons-folder"
          accept="application/octet-stream"
          @change="onCart"
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
  import type { FormError } from '#ui/types'

  const emulation = useEmulation()
  const { loadROM, loadCart } = emulation
  const { rom, cart } = storeToRefs(emulation)
  
  const emit = defineEmits<{
    (e: 'close'): void
  }>()

  const onRom = (files: FileList) => {
    loadROM(files[0])
  }

  const onCart = (files: FileList) => {
    loadCart(files[0])
  }

  const validate = (): FormError[] => {
    const errors = []
    if (rom.value && rom.value.size != 32768) errors.push({ path: 'rom', message: 'ROM size must be exactly 32768 bytes' })
    return errors
  }
</script>