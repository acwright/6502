<template>
  <UCard>
    <template #header>
      <div class="flex flex-row items-center gap-2">
        <UIcon name="i-heroicons-question-mark-circle" />
        <h1 class="text-lg">Help</h1>
      </div>
    </template>

    <div class="flex flex-col gap-2 items-center">
      <Logo class="w-20 h-20" />
      <h2 class="text-2xl">6502 Emulator</h2>
      <p class="text-xs">&copy; 2024 A.C. Wright Design</p>
      <UButton 
        icon="i-heroicons-arrow-top-right-on-square"
        to="https://github.com/acwright/6502" 
        target="_blank"
        size="lg"
        class="mt-2 text-lg"
      >View Docs on Github</UButton>
    </div>
    
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
</script>