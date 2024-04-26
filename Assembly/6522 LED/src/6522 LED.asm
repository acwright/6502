.setcpu "65C02"
.segment "CODE"

reset:
  lda #$ff
  sta $9802
loop:
  lda #$55
  sta $9800
  lda #$aa
  sta $9800
  jmp loop

.segment "VECTORS"

  .word $0000
  .word reset
  .word $0000