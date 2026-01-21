declare global {
  
  interface Info {
    address: number,
    cartEnabled: boolean,
    cartFile: string,
    cpuAccumulator: number,
    cpuOpcodeCycle: number,
    cpuPC: number,
    cpuStackPointer: number,
    cpuStatus: number,
    cpuX: number,
    cpuY: number,
    data: number,
    freqLabel: string,
    freqPeriod: number,
    inputCtx: number,
    ioEnabled: boolean,
    ipAddress: string,
    isRunning: boolean,
    lastSnapshot: number,
    programFile: string,
    ramEnabled: boolean,
    romEnabled: boolean,
    romFile: string,
    rtc: number,
    rw: number,
    version: string
  }

}

export {}