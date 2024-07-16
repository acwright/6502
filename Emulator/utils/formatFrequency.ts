export const formatFrequency = (frequency: number): string => {
  switch (true) {
    case (frequency < 1000):
      return `${frequency} Hz`
    case (frequency >= 1000 && frequency < 1000000):
      return `${frequency / 1000} kHz`
    case (frequency >= 1000000):
      return `${frequency / 1000000} MHz`
    default:
      return ''
  }
}