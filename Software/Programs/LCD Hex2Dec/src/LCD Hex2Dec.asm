.setcpu "65C02"

.include "../../../6502.inc"

.segment "CODE"

VALUE       := $00
MOD10       := $02
RESULT      := $04

reset:
  ldx #$ff
  txs

  lda #%11111111 ; Set all pins on port B to output
  sta LCD_DDRB
  lda #%11100000 ; Set top 3 pins on port A to output
  sta LCD_DDRA

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
  sta LCD_DDRB
lcd_busy:
  lda #LCD_RW
  sta LCD_PORTA
  lda #(LCD_RW | LCD_E)
  sta LCD_PORTA
  lda LCD_PORTB
  and #%10000000
  bne lcd_busy

  lda #LCD_RW
  sta LCD_PORTA
  lda #%11111111  ; Port B is output
  sta LCD_DDRB
  pla
  rts

lcd_instruction:
  jsr lcd_wait
  sta LCD_PORTB
  lda #0         ; Clear RS/RW/E bits
  sta LCD_PORTA
  lda #LCD_E         ; Set E bit to send instruction
  sta LCD_PORTA
  lda #0         ; Clear RS/RW/E bits
  sta LCD_PORTA
  rts

print_char:
  jsr lcd_wait
  sta LCD_PORTB
  lda #LCD_RS         ; Set RS; Clear RW/E bits
  sta LCD_PORTA
  lda #(LCD_RS | LCD_E)   ; Set E bit to send instruction
  sta LCD_PORTA
  lda #LCD_RS         ; Clear E bits
  sta LCD_PORTA
  rts