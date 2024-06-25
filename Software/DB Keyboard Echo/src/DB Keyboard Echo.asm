.setcpu "65C02"
.segment "CODE"

TERM_DATA   = $9C00
KBD_DATA    = $9D00

reset:
  ldx #$ff
  txs

loop:
  lda KBD_DATA
  beq loop          ; Is data ready (not zero)?
  sta TERM_DATA     ; Output char to terminal
  jmp loop

.segment "VECTORS"

  .word $0000
  .word reset
  .word $0000