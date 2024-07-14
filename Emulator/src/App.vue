<template>
  <h1 class="text-2xl">6502</h1>
</template>

<script setup lang="ts">
  import { Machine } from './lib/Machine.ts'

  const machine = new Machine()

  console.log(machine)
</script>
