.setcpu "65C02"
.segment "CODE"

TERM_DATA   = $9C00

reset:
  ldx #$ff
  txs

  ldx #0
print:
  lda message,x
  beq end
  sta TERM_DATA   ; Write char to Terminal data register
  inx
  jmp print

end:
  lda #$0D        ; Carriage Return
  sta TERM_DATA
  lda #$0A        ; Line feed
  sta TERM_DATA

halt:
  jmp halt

message: .asciiz "Hello, World!"

.segment "VECTORS"

  .word $0000
  .word reset
  .word $0000