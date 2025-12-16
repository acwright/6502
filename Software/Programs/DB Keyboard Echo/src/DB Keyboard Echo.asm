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
  sta EMU_SER_CTRL    ; Use DB USB Serial output

loop:
  bit EMU_KB_DATA
  bpl loop        ; No data availabe if bit 7 is not set
  lda EMU_KB_DATA
  and #%01111111  ; Mask out the ASCII value
  sta EMU_SER_DATA
  jmp loop

.segment "KERNAL"
.segment "CART"
.segment "WOZMON"
.segment "VECTORS"