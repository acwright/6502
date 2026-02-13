<template>
  <div class="flex flex-col p-4">
    <h2>STORAGE</h2>
    <div class="flex flex-col justify-between grow border-2 pl-4 pr-4 pt-8 pb-8">
      <UFieldGroup orientation="horizontal" size="sm" class="flex flex-row justify-center">
        <UButton 
          @click="onTarget('rom')"
          :color="target == 'rom' ? 'primary' : 'neutral'"
          :variant="target == 'rom' ? 'solid' : 'outline'"
          label="ROMs"
        />
        <UButton 
          @click="onTarget('cart')"
          :color="target == 'cart' ? 'primary' : 'neutral'"
          :variant="target == 'cart' ? 'solid' : 'outline'"
          label="Carts"
        />
        <UButton 
          @click="onTarget('program')"
          :color="target == 'program' ? 'primary' : 'neutral'"
          :variant="target == 'program' ? 'solid' : 'outline'"
          label="Programs"
        />
      </UFieldGroup>
      <br>
      <div class="flex flex-col w-full gap-1">
        <div v-for="(file, i) in files" :key="i" class="flex flex-row gap-2">
          <span class="flex flex-row justify-end w-1/4 gap-1">
            <UButton @click="onLoad(file)" label="LOAD" size="xs"/>
            <span class="text-right w-8">
              {{ i }}:
            </span>
          </span>
          <span class="text-left w-3/4">{{ file }}</span>
        </div>
      </div>
      <br>
      <UPagination v-model:page="page" :items-per-page="8" :total="256" size="sm" class="flex flex-row justify-center"/>
    </div>
  </div>
</template>

<script setup lang="ts">
  const { error: notification } = useNotifications()
  const info = useState<Info>('info')
  const files = ref<string[]>([])
  const target = ref<'rom' | 'cart' | 'program'>('rom')
  const page = ref(1)

  const onTarget = (_target: 'rom' | 'cart' | 'program') => {
    page.value = 1
    target.value = _target
  }

  const onLoad = async (filename: string) => {
    try {
      await $fetch(`http://${info.value.ipAddress}/load`, {
        query: {
          target: target.value,
          filename: filename
        }
      })
      await fetchInfo()
    } catch (error) {
      notification(error)
    }
  }

  const fetchInfo = async () => {
    try {
      info.value = await $fetch<Info>(`http://${info.value.ipAddress}/info`)
    } catch (error) {
      notification(error)
    }
  }

  watch([info, target, page], async () => {
    try {
      files.value = await $fetch<string[]>(`http://${info.value.ipAddress}/storage`, {
        query: {
          target: target.value,
          page: page.value - 1
        }
      })
    } catch (error) {
      notification(error)
    }
  }, { immediate: true })
</script>