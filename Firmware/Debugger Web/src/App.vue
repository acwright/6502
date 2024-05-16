<template>
  <header>
    <img alt="Vue logo" class="logo" src="./assets/logo.svg" width="125" height="125" />
  </header>
  <main>
    <p class="text-xl">Hello, World!</p>
    <br />
    <p @click="useCounterStore().increment()">{{ useCounterStore().count }}</p>
  </main>
</template>

<script setup lang="ts">
  import { useCounterStore } from './stores/counter'
  
</script>
