.setcpu "65C02"

.include "../../6502.inc"

.segment "CODE"

reset:
  lda #%11111111         ; Set all pins on port B to outputs
  sta GPIO_DDRB
  lda #%00000000         ; Set all pins on port A to inputs
  sta GPIO_DDRA

loop:
  lda GPIO_PORTA              ; Read buttons on port A
  eor #$FF
  sta GPIO_PORTB              ; Write button state to port B
  jmp loop
