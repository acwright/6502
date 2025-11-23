.setcpu "65C02"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "CODE"

DB_DATA           = $8400
DB_JOYSTICK_BTNS  = $8407

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
  lda DB_JOYSTICK_BTNS
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