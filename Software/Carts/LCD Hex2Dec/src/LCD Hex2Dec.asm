.setcpu "65C02"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "CODE"

PORTB   = $8000
PORTA   = $8001
DDRB    = $8002
DDRA    = $8003

VALUE   = $0200
MOD10   = $0202
RESULT  = $0204

E       = %10000000
RW      = %01000000
RS      = %00100000

reset:
  ldx #$ff
  txs

  lda #%11111111 ; Set all pins on port B to output
  sta DDRB
  lda #%11100000 ; Set top 3 pins on port A to output
  sta DDRA

  lda #%00111000 ; Set 8-bit mode; 2-line display; 5x8 font
  jsr lcd_instruction
  lda #%00001110 ; Display on; cursor on; blink off
  jsr lcd_instruction
  lda #%00000110 ; Increment and shift cursor; don't shift display
  jsr lcd_instruction

clear:
  lda #$00000001 ; Clear display
  jsr lcd_instruction

main:
  lda #0
  sta RESULT        ; Intialize the result

  lda number        ; Initialize the number
  sta VALUE
  lda number + 1
  sta VALUE + 1

divide:
  lda #0            ; Initialize the remainder to zero
  sta MOD10
  sta MOD10 + 1
  clc

  ldx #16
loop:
  rol VALUE         ; Rotate quotient and remainder
  rol VALUE + 1
  rol MOD10
  rol MOD10 + 1

  sec               ; A,Y = dividend - divisor
  lda MOD10
  sbc #10
  tay               ; Save low byte in Y
  lda MOD10 + 1
  sbc #0
  bcc ignore_result ; Branch if dividend < divisor
  sty MOD10
  sta MOD10 + 1

ignore_result:
  dex
  bne loop
  rol VALUE
  rol VALUE + 1

  lda MOD10
  clc
  adc #48           ; ASCII "0"
  jsr push_char

  lda VALUE         ; If value != 0, then continue dividing
  ora VALUE + 1
  bne divide        ; Branch if value != 0

  ldx #0
print:
  lda RESULT,x
  beq halt
  jsr print_char
  inx
  jmp print

halt:
  jmp halt

number: .word 1729

push_char:
  pha               ; Push new first char onto stack
  ldy #0  
char_loop:
  lda RESULT,y      ; Get char on string and put in X
  tax
  pla
  sta RESULT,y      ; Pull char off stack and add it to the string
  iny
  txa
  pha               ; Push char from string onto stack
  bne char_loop
  pla
  sta RESULT,y      ; Pull the null off the stack and add to the end of the string
  rts

lcd_wait:
  pha
  lda #%00000000  ; Port B is input
  sta DDRB
lcd_busy:
  lda #RW
  sta PORTA
  lda #(RW | E)
  sta PORTA
  lda PORTB
  and #%10000000
  bne lcd_busy

  lda #RW
  sta PORTA
  lda #%11111111  ; Port B is output
  sta DDRB
  pla
  rts

lcd_instruction:
  jsr lcd_wait
  sta PORTB
  lda #0         ; Clear RS/RW/E bits
  sta PORTA
  lda #E         ; Set E bit to send instruction
  sta PORTA
  lda #0         ; Clear RS/RW/E bits
  sta PORTA
  rts

print_char:
  jsr lcd_wait
  sta PORTB
  lda #RS         ; Set RS; Clear RW/E bits
  sta PORTA
  lda #(RS | E)   ; Set E bit to send instruction
  sta PORTA
  lda #RS         ; Clear E bits
  sta PORTA
  rts

.segment "VECTORS"

  .word reset
  .word reset
  .word reset