.setcpu "65C02"

.include "../../../6502.inc"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "PROGRAM"

reset:
  lda #%11111111              ; Set all pins on port B to output
  sta LCD_DDRB
  lda #%11100000              ; Set top 3 pins on port A to output
  sta LCD_DDRA

  lda #%00111000              ; Set 8-bit mode; 2-line display; 5x8 font
  jsr lcd_instruction
  lda #%00001110              ; Display on; cursor on; blink off
  jsr lcd_instruction
  lda #%00000110              ; Increment and shift cursor; don't shift display
  jsr lcd_instruction

clear:
  lda #$00000001              ; Clear display
  jsr lcd_instruction

  ldx #0
print:
  lda message,x
  beq halt
  jsr print_char
  inx
  jmp print

halt:
  jmp halt

message: .asciiz "Hello, World!"

lcd_wait:
  pha
  lda #%00000000              ; Port B is input
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
  lda #%11111111              ; Port B is output
  sta LCD_DDRB
  pla
  rts

lcd_instruction:
  jsr lcd_wait
  sta LCD_PORTB
  lda #0                      ; Clear RS/RW/E bits
  sta LCD_PORTA
  lda #LCD_E                  ; Set E bit to send instruction
  sta LCD_PORTA
  lda #0                      ; Clear RS/RW/E bits
  sta LCD_PORTA
  rts

print_char:
  jsr lcd_wait
  sta LCD_PORTB
  lda #LCD_RS                 ; Set RS; Clear RW/E bits
  sta LCD_PORTA
  lda #(LCD_RS | LCD_E)       ; Set E bit to send instruction
  sta LCD_PORTA
  lda #LCD_RS                 ; Clear E bits
  sta LCD_PORTA
  rts

.segment "KERNAL"
.segment "CART"
.segment "WOZMON"
.segment "VECTORS"