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
  
rx_wait:
  lda ACIA_STATUS
  and #%00001000  ; Check if rx buffer empty
  beq rx_wait     ; Loop if rx buffer empty

  lda ACIA_DATA   ; Otherwise load the received char
  jsr send_char   ; Echo char
  jmp rx_wait

send_char:
  sta ACIA_DATA 
  pha
tx_wait:
  lda ACIA_STATUS
  and #%00010000  ; Check if tx buffer not empty
  beq tx_wait     ; Loop if tx buffer not empty
  pla
  rts

.segment "VECTORS"

  .word reset
  .word reset
  .word reset