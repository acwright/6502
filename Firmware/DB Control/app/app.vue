<template>
  <UApp>
    <div class="flex flex-col h-lvh">
      <LayoutHeader />
      <hr>
      <LayoutMain />
      <hr>
      <LayoutFooter />
    </div>
  </UApp>
</template>

<script setup lang="ts">
  const store = useControlStore()

  await callOnce(store.fetchInfo)
</script>