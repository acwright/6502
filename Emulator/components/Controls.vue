<template>
  <div class="flex flex-row justify-center items-center gap-2">
    <UTooltip text="Run" :shortcuts="[metaSymbol, 'R']">
      <UButton 
        icon="i-heroicons-play"
        size="xs"
        title="Start"
        :loading="isRunning"
        @click="emit('run')"
      />
    </UTooltip>
    <UTooltip text="Pause" :shortcuts="[metaSymbol, 'P']">
      <UButton 
        icon="i-heroicons-pause"
        color="gray"
        size="xs"
        title="Pause"
        @click="emit('stop')"
      />
    </UTooltip>
    <UTooltip text="Step" :shortcuts="[metaSymbol, 'S']">
      <UButton 
        icon="i-heroicons-forward"
        color="gray"
        size="xs"
        title="Step"
        :disabled="isRunning"
        @click="emit('step')"
      />
    </UTooltip>
    <UTooltip text="Reset" :shortcuts="[metaSymbol, 'T']">
      <UButton 
        icon="i-heroicons-arrow-path"
        color="gray"
        size="xs"
        title="Reset"
        @click="emit('reset')"
      />
    </UTooltip>
    <UTooltip text="Load ROM" :shortcuts="[metaSymbol, 'L']">
      <UButton 
        icon="i-heroicons-folder"
        color="gray"
        size="xs"
        title="Load ROM"
        :disabled="isRunning"
        @click="emit('load')"
      />
    </UTooltip>
    <UTooltip text="Configure" :shortcuts="[metaSymbol, 'C']">
      <UButton 
        icon="i-heroicons-cog-6-tooth"
        color="gray"
        size="xs"
        title="Configure"
        :disabled="isRunning"
        @click="emit('configure')"
      />
    </UTooltip>
    <UTooltip text="Toggle Debug" :shortcuts="[metaSymbol, 'D']">
      <UButton 
        icon="i-heroicons-bug-ant"
        color="gray"
        size="xs"
        title="Debug"
        @click="emit('debug')"
      />
    </UTooltip>
  </div>
</template>

<script setup lang="ts">
  const props = defineProps<{
    isRunning: boolean
  }>()

  const emit = defineEmits([
    'run', 
    'stop', 
    'step', 
    'reset', 
    'load',
    'configure',
    'debug'
  ])

  const { metaSymbol } = useShortcuts()

  defineShortcuts({
    meta_r: {
      handler: () => {
        emit('run')
      }
    },
    meta_p: {
      handler: () => {
        emit('stop')
      }
    },
    meta_s: {
      handler: () => {
        emit('step')
      }
    },
    meta_t: {
      handler: () => {
        emit('reset')
      }
    },
    meta_l: {
      handler: () => {
        emit('load')
      }
    },
    meta_c: {
      handler: () => {
        emit('configure')
      }
    },
    meta_d: {
      handler: () => {
        emit('debug')
      }
    }
  })
</script>