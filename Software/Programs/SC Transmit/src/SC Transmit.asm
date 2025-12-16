.setcpu "65C02"

.include "../../../6502.inc"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "PROGRAM"

reset:
  lda #$00
  sta SC_STATUS         ; Soft reset (value not important)

  lda #%00010000        ; N-8-1, 115200 baud
  sta SC_CTRL

  lda #%00001011        ; No parity, No echo, No interrupts
  sta SC_CMD

  ldx #0
send_msg:
  lda message,x
  beq halt
  jsr send_char
  inx
  jmp send_msg

send_char:
  sta SC_DATA 
  pha
tx_wait:
  lda SC_STATUS
  and #%00010000        ; Check if tx buffer not empty
  beq tx_wait           ; Loop if tx buffer not empty
  pla
  rts

message: .asciiz "Hello from 6502!"

halt:
  jmp halt

.segment "KERNAL"
.segment "CART"
.segment "WOZMON"
.segment "VECTORS"