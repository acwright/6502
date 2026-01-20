<template>
  <div class="flex flex-col p-4">
    <h2>MEMORY</h2>
    <div class="flex flex-col justify-between grow border-2 pl-4 pr-4 pt-8 pb-8">
      <UFieldGroup orientation="horizontal" size="sm" class="flex flex-row justify-center">
        <UButton color="primary" variant="solid" label="RAM" />
        <UButton color="neutral" variant="outline" label="ROM" />
      </UFieldGroup>
      <br>
      <div class="flex flex-row ml-auto mr-auto gap-3 text-xs">
        <div class="grid grid-rows-16 w-3/32 gap-1 text-right font-bold">
          <div v-for="i in 16">${{ ((i - 1) * 16).toString(16).padStart(4, '0').toUpperCase() }}:</div>
        </div>
        <div class="grid grid-cols-16 w-21/32 gap-1 text-center">
          <div v-for="i in 256">{{ (i-1).toString(16).padStart(2,'0').toUpperCase() }}</div>
        </div>
        <div class="grid grid-cols-16 w-8/32 gap-x-0 gap-y-1 text-center text-gray-400">
          <div v-for="i in 256">{{ String.fromCharCode(i - 1) }}</div>
        </div>
      </div>
      <br>
      <UPagination v-model:page="page" :items-per-page="256" :total="65535" size="sm" class="flex flex-row justify-center"/>
    </div>
  </div>
</template>

<script setup lang="ts">
  const page = ref(1)
</script>