.setcpu "65C02"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "CODE"

DB_DATA     = $8400
DB_KB_DATA  = $8404

reset:
  ldx #$ff
  txs

loop:
  bit DB_KB_DATA
  bpl loop        ; Bit 7 is not set
  lda DB_KB_DATA
  and #%01111111  ; Mask out the ASCII value
  sta DB_DATA
  jmp loop
