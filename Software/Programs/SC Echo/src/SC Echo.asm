.setcpu "65C02"

.include "../../../6502.inc"

.segment "CODE"

reset:
  lda #$00
  sta SC_STATUS ; Soft reset (value not important)

  lda #%00010000  ; N-8-1, 115200 baud
  sta SC_CTRL

  lda #%00001011  ; No parity, No echo, No interrupts
  sta SC_CMD
  
rx_wait:
  lda SC_STATUS
  and #%00001000  ; Check if rx buffer empty
  beq rx_wait     ; Loop if rx buffer empty

  lda SC_DATA   ; Otherwise load the received char
  jsr send_char   ; Echo char
  jmp rx_wait

send_char:
  sta SC_DATA 
  pha
tx_wait:
  lda SC_STATUS
  and #%00010000  ; Check if tx buffer not empty
  beq tx_wait     ; Loop if tx buffer not empty
  pla
  rts