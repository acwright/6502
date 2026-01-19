export const useControlStore = defineStore('controlStore', {
  state: () => ({
    address: 0,
    cartCode: 0,
    cartEnabled: false,
    cartEnd: 0,
    cartFile: 'None',
    cartFilePage: 0,
    cartFiles: [] as string[],
    cartStart: 0,
    cpuAccumulator: 0,
    cpuOpcodeCycle: 0,
    cpuPC: 0,
    cpuStackPointer: 0,
    cpuStatus: 0,
    cpuX: 0,
    cpuY: 0,
    data: 0,
    fileMax: 0,
    freqLabel: 'None',
    freqPeriod: 0,
    inputCtx: 0,
    io: [] as string[],
    ioEnd: 0,
    ioSlots: 0,
    ioSlotSize: 0,
    ioStart: 0,
    ipAddress: '0.0.0.0',
    isRunning: false,
    lastSnapshot: 0,
    programFile: 'None',
    programFilePage: 0,
    ramCode: 0,
    ramEnabled: false,
    ramEnd: 0,
    ramFiles: [] as string[],
    ramStart: 0,
    romEnabled: false,
    romEnd: 0,
    romFile: 'None',
    romFilePage: 0,
    romFiles: [] as string[],
    romStart: 0,
    rtc: 0,
    rw: 0,
    version: '0.0'
  }),
  actions: {
    async doControl(command: string) {
      await $fetch('/api/control', {
        query: {
          command: command
        }
      })
    },
    async fetchInfo () {
      const info = await $fetch('/api/info') as {
        address: number,
        cartCode: number,
        cartEnabled: boolean,
        cartEnd: number,
        cartFile: string,
        cartFilePage: number,
        cartFiles: string[],
        cartStart: number,
        cpuAccumulator: number,
        cpuOpcodeCycle: number,
        cpuPC: number,
        cpuStackPointer: number,
        cpuStatus: number,
        cpuX: number,
        cpuY: number,
        data: number,
        fileMax: number,
        freqLabel: string,
        freqPeriod: number,
        inputCtx: number,
        io: string[],
        ioEnd: number,
        ioSlots: number,
        ioSlotSize: number,
        ioStart: number,
        ipAddress: string,
        isRunning: boolean,
        lastSnapshot: number,
        programFile: string,
        programFilePage: number,
        ramCode: number,
        ramEnabled: boolean,
        ramEnd: number,
        ramFiles: string[],
        ramStart: number,
        romEnabled: boolean,
        romEnd: number,
        romFile: string,
        romFilePage: number,
        romFiles: string[],
        romStart: number,
        rtc: number,
        rw: number,
        version: string
      }

      this.address = info.address
      this.cartCode = info.cartCode
      this.cartEnabled = info.cartEnabled
      this.cartEnd = info.cartEnd
      this.cartFile = info.cartFile
      this.cartFilePage = info.cartFilePage
      this.cartFiles = info.cartFiles
      this.cartStart = info.cartStart
      this.cpuAccumulator = info.cpuAccumulator
      this.cpuOpcodeCycle = info.cpuOpcodeCycle
      this.cpuPC = info.cpuPC
      this.cpuStackPointer = info.cpuStackPointer
      this.cpuStatus = info.cpuStatus
      this.cpuX = info.cpuX
      this.cpuY = info.cpuY
      this.data = info.data
      this.fileMax = info.fileMax
      this.freqLabel = info.freqLabel
      this.freqPeriod = info.freqPeriod
      this.inputCtx = info.inputCtx
      this.io = info.io
      this.ioEnd = info.ioEnd
      this.ioSlots = info.ioSlots
      this.ioSlotSize = info.ioSlotSize
      this.ioStart = info.ioStart
      this.ipAddress = info.ipAddress
      this.isRunning = info.isRunning
      this.lastSnapshot = info.lastSnapshot
      this.programFile = info.programFile
      this.programFilePage = info.programFilePage
      this.ramCode = info.ramCode
      this.ramEnabled = info.ramEnabled
      this.ramEnd = info.ramEnd
      this.ramFiles = info.ramFiles
      this.ramStart = info.ramStart
      this.romEnabled = info.romEnabled
      this.romEnd = info.romEnd
      this.romFile = info.romFile
      this.romFilePage = info.romFilePage
      this.romFiles = info.romFiles
      this.romStart = info.romStart
      this.rtc = info.rtc
      this.rw = info.rw
      this.version = info.version
    }
  },
})