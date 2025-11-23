.setcpu "65C02"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "CODE"

DB_DATA   = $8400

reset:
  ldx #$ff
  txs

  ldx #0
print:
  lda message,x
  beq end
  sta DB_DATA     ; Write char to DB serial data register
  inx
  jmp print

end:
  lda #$0D        ; Carriage Return
  sta DB_DATA
  lda #$0A        ; Line feed
  sta DB_DATA

halt:
  jmp halt

message: .asciiz "Hello, World!"
