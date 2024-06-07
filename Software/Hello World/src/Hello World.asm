.setcpu "65C02"
.segment "CODE"

DSP   = $9C03

reset:
  ldx #$ff
  txs

  ldx #0
print:
  lda message,x
  beq end
  sta DSP
  inx
  jmp print

end:
  lda #$0D        ; Carriage Return
  sta DSP
  lda #$0A        ; Line feed
  sta DSP

loop:
  jmp loop

message: .asciiz "Hello, World!"

.segment "VECTORS"

  .word $0000
  .word reset
  .word $0000