.setcpu "65C02"

.include "../../../6502.inc"

.segment "CODE"

reset:
  lda #%11111111         ; Set all pins on port B to output
  sta GPIO_DDRB
  
loop:
  lda #$55
  sta GPIO_PORTB
  lda #$AA
  sta GPIO_PORTB
  jmp loop