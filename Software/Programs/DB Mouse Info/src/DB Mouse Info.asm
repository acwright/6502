.setcpu "65C02"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "CODE"

DB_DATA         = $8400
DB_MOUSE_X      = $8403
DB_MOUSE_Y      = $8404
DB_MOUSE_W      = $8405
DB_MOUSE_BTNS   = $8406

reset:
  ldx #$ff
  txs

loop:
  ldx #$0F
loop_inner:
  jsr delay
  dex
  bne loop_inner
  jsr print
  jmp loop

print:
  lda DB_MOUSE_X
  sta DB_DATA
  lda DB_MOUSE_Y
  sta DB_DATA
  lda DB_MOUSE_W
  sta DB_DATA
  lda DB_MOUSE_BTNS
  sta DB_DATA
  rts

delay:
  phx
  ldx #$ff
delay_loop:
  dex
  bne delay_loop
  plx
  rts