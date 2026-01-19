<template>
  <div class="flex flex-col p-4">
    <h2>STORAGE</h2>
    <div class="flex flex-col justify-between grow border-2 pl-4 pr-4 pt-8 pb-8">
      <UFieldGroup orientation="horizontal" size="sm" class="flex flex-row justify-center">
        <UButton color="primary" variant="solid" label="ROMs" />
        <UButton color="neutral" variant="outline" label="Carts" />
        <UButton color="neutral" variant="outline" label="Programs" />
      </UFieldGroup>
      <br>
      <div class="flex flex-col text-center ml-auto mr-auto gap-2">
        <div v-for="i in 8" class="flex flex-row gap-2">
          <UButton label="LOAD" size="xs"/>
          <p class="text-right w-4">
            {{ i - 1 }}:
          </p>
          <p>{{ i * 16 }} File Name.bin</p>
        </div>
      </div>
      <br>
      <UPagination v-model:page="page" :items-per-page="8" :total="256" size="sm" class="flex flex-row justify-center"/>
    </div>
  </div>
</template>

<script setup lang="ts">
  const page = ref(1)
</script>