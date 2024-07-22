import { defineStore } from 'pinia'

export const useMachine = defineStore('machine', () => {
  return new Machine()
})