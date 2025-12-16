.setcpu "65C02"

.include "../../../6502.inc"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "PROGRAM"

reset:
  lda #%00000000            ; Set all pins on port B to inputs
  sta GPIO_DDRB
  lda #%00000000            ; Set all pins on port A to inputs
  sta GPIO_DDRA

  lda #$00
  sta SC_STATUS             ; Soft reset (value not important)

  lda #%00010000            ; N-8-1, 115200 baud
  sta SC_CTRL

  lda #%00001011            ; No parity, No echo, No interrupts
  sta SC_CMD

loop:
  ldx #$0F
loop_inner:
  jsr delay
  dex
  bne loop_inner
  jsr scan
  jmp loop

scan:
  ldx #$80
scan_inner:
  stx GPIO_DDRB             ; Set one of PORTB's pins as output
  txa
  eor #$FF                  ; Flip all the bits
  sta GPIO_PORTB            ; Set one of PORTB's pins low to enable column
  lda GPIO_PORTA            ; Read rows from PORTA
  cmp #$FF        
  bne send                  ; Output the value of PORTA if any key is pressed
  lda #%00000000            ; Set all pins on port B to inputs
  sta GPIO_DDRB
  txa
  lsr                       ; Shift X to the right to set column for next loop
  beq scan_exit             ; If zero exit scanning
  tax
  jmp scan_inner
scan_exit:
  rts

send:
  jsr send_value            ; Send value in A register (X = ROW)
  txa
  jsr send_value            ; Send value of X register (Y = COL)
  rts

delay:
  phx
  ldx #$ff
delay_loop:
  dex
  bne delay_loop
  plx
  rts

send_value:
  sta SC_DATA 
  pha
send_value_wait:
  lda SC_STATUS
  and #%00010000            ; Check if tx buffer not empty
  beq send_value_wait       ; Loop if tx buffer not empty
  pla
  rts

.segment "KERNAL"
.segment "CART"
.segment "WOZMON"
.segment "VECTORS"