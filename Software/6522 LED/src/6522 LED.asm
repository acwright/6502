.setcpu "65C02"
.segment "CODE"

PORTB   = $9400
DDRB    = $9402

reset:
  lda #%11111111         ; Set all pins on port B to output
  sta DDRB
loop:
  lda #$55
  sta PORTB
  lda #$aa
  sta PORTB
  jmp loop

.segment "VECTORS"

  .word $0000
  .word reset
  .word $0000