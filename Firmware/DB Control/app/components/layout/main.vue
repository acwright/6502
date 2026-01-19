<template>
  <main class="grid grid-cols-1 lg:grid-cols-2 h-full overflow-y-auto">
    <Emulator/>
    <Cpu/>
    <Memory/>
    <Storage/>
  </main>
</template>

<script setup lang="ts">
  const { data } = await useFetch('/api/info')
</script>