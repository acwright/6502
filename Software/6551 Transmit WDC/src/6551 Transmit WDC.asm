.setcpu "65C02"
.segment "CODE"

ACIA_DATA   = $9000
ACIA_STATUS = $9001
ACIA_CMD    = $9002
ACIA_CTRL   = $9003

reset:
  ldx #$ff
  txs

  lda #$00
  sta ACIA_STATUS ; Soft reset (value not important)

  lda #%00011111  ; N-8-1, 19200 baud
  sta ACIA_CTRL

  lda #%00001011  ; No parity, No echo, No interrupts
  sta ACIA_CMD

  ldx #0
send_msg:
  lda message,x
  beq halt
  jsr send_char
  inx
  jmp send_msg

send_char:
  sta ACIA_DATA
  phx
  ldx #100
tx_wait:
  dex
  bne tx_wait
  plx
  rts

message: .asciiz "Hello, World!"

halt:
  jmp halt

.segment "VECTORS"

  .word $0000
  .word reset
  .word $0000