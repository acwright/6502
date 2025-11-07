.setcpu "65C02"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "CODE"

PORTB   = $9400
PORTA   = $9401
DDRB    = $9402
DDRA    = $9403

reset:
  lda #%11111111         ; Set all pins on port B to outputs
  sta DDRB
  lda #%00000000         ; Set all pins on port A to inputs
  sta DDRA

loop:
  lda PORTA              ; Read buttons on port A
  eor #$FF
  sta PORTB              ; Write button state to port B
  jmp loop

.segment "VECTORS"

  .word reset
  .word reset
  .word reset