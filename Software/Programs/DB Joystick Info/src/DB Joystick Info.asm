.setcpu "65C02"

.include "../../../6502.inc"

.segment "CODE"

reset:
  lda #$00
  sta EMU_SER_CTRL    ; Use DB USB Serial output

loop:
  ldx #$0F
loop_inner:
  jsr delay
  dex
  bne loop_inner
  jsr print
  jmp loop

print:
  lda EMU_JOY_BTNS
  sta EMU_SER_DATA
  rts

delay:
  phx
  ldx #$ff
delay_loop:
  dex
  bne delay_loop
  plx
  rts