.setcpu "65C02"

.include "../../6502.inc"

.segment "CODE"

reset:
  lda #$00

loop:
  sta EMU_PRAM_ADDR
  sta EMU_PRAM_DATA
  inc
  bne loop

halt:
  rts