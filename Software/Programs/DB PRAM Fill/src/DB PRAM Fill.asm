.setcpu "65C02"

.include "../../../6502.inc"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "PROGRAM"

reset:
  lda #$00

loop:
  sta EMU_PRAM_ADDR
  sta EMU_PRAM_DATA
  inc
  bne loop

halt:
  rts

.segment "KERNAL"
.segment "CART"
.segment "WOZMON"
.segment "VECTORS"