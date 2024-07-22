export const formatHex = (dec: number, length: number = 4): string => {
  return '$' + (dec + Math.pow(16, 6)).toString(16).slice(-length) 
}