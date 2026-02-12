.setcpu "65C02"

.include "../../../6502.inc"

.segment "CODE"

reset:
  lda #$00
  sta EMU_SER_CTRL  ; Use DB USB Serial output

  ldx #0
print:
  lda message,x
  beq end
  sta EMU_SER_DATA  ; Write char to DB serial data register
  inx
  jmp print

end:
  lda #$0D          ; Carriage Return
  sta EMU_SER_DATA
  lda #$0A          ; Line feed
  sta EMU_SER_DATA

halt:
  rts

message: .asciiz "Hello, World!"
