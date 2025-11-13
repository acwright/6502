.setcpu "65C02"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "CODE"

DB_PRAM_DATA   = $8412
DB_PRAM_ADDR   = $8413

reset:
  ldx #$ff
  txs

  lda #$00
loop:
  sta DB_PRAM_ADDR
  sta DB_PRAM_DATA
  inc
  bne loop

halt:
  jmp halt
