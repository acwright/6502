.setcpu "65C02"
.segment "CODE"

PORTB   = $9400
PORTA   = $9401
DDRB    = $9402
DDRA    = $9403

reset:
  lda #%11111111         ; Set all pins on port B to outputs
  sta DDRB
  lda #%00000000         ; Set all pins on port A to inputS
  sta DDRA

loop:
  lda PORTA              ; Read buttons on port A
  sta PORTB              ; Write button state to port B
  jmp loop

.segment "VECTORS"

  .word $0000
  .word reset
  .word $0000