<template>
  <div class="flex flex-col p-4">
    <h2>CPU</h2>
    <div class="flex flex-col justify-center grow border-2 p-2">
      <br>
      <div class="grid grid-cols-2 gap-x-8 ml-auto mr-auto">
        <div class="flex flex-row gap-3">
          <div class="font-bold">A:</div>
          <div>${{ store.cpuAccumulator.toString(16).padStart(2,'0').toUpperCase() }}</div>
          <div class="text-gray-400">{{ store.cpuAccumulator }}</div>
        </div>
        <div class="flex flex-row gap-3">
          <div class="font-bold">PC:</div>
          <div>${{ store.cpuPC.toString(16).padStart(4,'0').toUpperCase() }}</div>
          <div class="text-gray-400">{{ store.cpuPC }}</div>
        </div>
        <div class="flex flex-row gap-3">
          <div class="font-bold">X:</div>
          <div>${{ store.cpuX.toString(16).padStart(2,'0').toUpperCase() }}</div>
          <div class="text-gray-400">{{ store.cpuX }}</div>
        </div>
        <div class="flex flex-row gap-3">
          <div class="font-bold">SP:</div>
          <div>${{ store.cpuStackPointer.toString(16).padStart(2,'0').toUpperCase() }}</div>
          <div class="text-gray-400">{{ store.cpuStackPointer }}</div>
        </div>
        <div class="flex flex-row gap-3">
          <div class="font-bold">Y:</div>
          <div>${{ store.cpuY.toString(16).padStart(2,'0').toUpperCase() }}</div>
          <div class="text-gray-400">{{ store.cpuY }}</div>
        </div>
        <div class="flex flex-row gap-3">
          <div class="font-bold">ST:</div>
          <div>${{ store.cpuStatus.toString(16).padStart(2,'0').toUpperCase() }}</div>
          <div class="text-gray-400">{{ store.cpuStatus }}</div>
        </div>
      </div>
      <br>
      <div class="text-center">
        <div>FLAGS:</div>
        <div class="tracking-widest text-gray-400">
          <span :class="flag(store.cpuStatus, 7) ? 'text-primary' : 'text-gray-400'">N</span>
          <span :class="flag(store.cpuStatus, 6) ? 'text-primary' : 'text-gray-400'">V</span>
          <span>-</span>
          <span :class="flag(store.cpuStatus, 4) ? 'text-primary' : 'text-gray-400'">B</span>
          <span :class="flag(store.cpuStatus, 3) ? 'text-primary' : 'text-gray-400'">D</span>
          <span :class="flag(store.cpuStatus, 2) ? 'text-primary' : 'text-gray-400'">I</span>
          <span :class="flag(store.cpuStatus, 1) ? 'text-primary' : 'text-gray-400'">Z</span>
          <span :class="flag(store.cpuStatus, 0) ? 'text-primary' : 'text-gray-400'">C</span>
        </div>
      </div>
      <br>
      <div class="flex flex-col items-center">
        <div class="flex flex-row gap-3">
          <div class="font-bold">ADDRESS BUS:</div>
          <div>${{ store.address.toString(16).padStart(4,'0').toUpperCase() }}</div>
          <div class="text-gray-400">{{ store.address }}</div>
        </div>
        <div>
          <UChip v-for="i in 16" :color="address(store.address, i) ? 'primary' : 'neutral'" standalone inset class="pl-1 pr-1"/>
        </div>
      </div>
      <br>
      <div class="flex flex-col items-center">
        <div class="flex flex-row gap-3">
          <div class="font-bold">DATA BUS:</div>
          <div>${{ store.data.toString(16).padStart(2,'0').toUpperCase() }}</div>
          <div class="text-gray-400">{{ store.data }}</div>
        </div>
        <div>
          <UChip v-for="i in 8" :color="data(store.data, i) ? 'primary' : 'neutral'" standalone inset class="pl-1 pr-1"/>
        </div>
      </div>
      <br>
      <div class="flex flex-col items-center">
        <div class="flex flex-row gap-3">
          <div class="font-bold">R/W:</div>
          <div>{{ store.rw == 0 ? 'WRITE' : 'READ' }}</div>
          <div class="text-gray-400">{{ store.rw }}</div>
        </div>
      </div>
      <br>
    </div>
  </div>
</template>

<script setup lang="ts">
  const store = useControlStore()

  const data = (data: number, i: number) => {
    return (1 << (7 - (i - 1))) & data
  }

  const address = (address: number, i: number) => {
    return (1 << (15 - (i - 1))) & address
  }

  const flag = (status: number, i: number) => {
    return (1 << i) & status
  }
</script>