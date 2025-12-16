.setcpu "65C02"

.include "../../../6502.inc"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "PROGRAM"

reset:
  lda #%11111111         ; Set all pins on port B to output
  sta GPIO_DDRB
  
loop:
  lda #$55
  sta GPIO_PORTB
  lda #$AA
  sta GPIO_PORTB
  jmp loop

.segment "KERNAL"
.segment "CART"
.segment "WOZMON"
.segment "VECTORS"