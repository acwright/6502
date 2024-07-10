.setcpu "65C02"
.segment "CODE"

PORTB   = $8000
PORTA   = $8001
DDRB    = $8002
DDRA    = $8003

MOUSE_X     = $9C02
MOUSE_Y     = $9C03
MOUSE_W     = $9C04
MOUSE_BTN   = $9C05

VALUE   = $0300
MOD10   = $0302
RESULT  = $0304

E       = %10000000
RW      = %01000000
RS      = %00100000

reset:
  ldx #$ff
  txs

  lda #%11111111                ; Set all pins on port B to output
  sta DDRB
  lda #%11100000                ; Set top 3 pins on port A to output
  sta DDRA

  lda #%00111000                ; Set 8-bit mode; 2-line display; 5x8 font
  jsr lcd_instruction
  lda #%00001100                ; Display on; cursor off; blink off
  jsr lcd_instruction
  lda #%00000110                ; Increment and shift cursor; don't shift display
  jsr lcd_instruction

clear:
  lda #$00000001                ; Clear display
  jsr lcd_instruction

  ldx #0
line_1:                         ; Print the line 1 template
  lda line_1_template,x
  beq line_2_start
  jsr lcd_write
  inx
  jmp line_1

line_2_start:
  lda #($80 | $40)              ; Move to DDRAM address $40 (line 2)
  jsr lcd_instruction

  ldx #0
line_2:                         ; Print the line 2 template
  lda line_2_template,x
  beq load_mouse
  jsr lcd_write
  inx
  jmp line_2

load_mouse:
  lda #%01001000                 ; Set CGRAM address 1
  jsr lcd_instruction
  
  lda mouse
  jsr lcd_write
  lda mouse+1
  jsr lcd_write
  lda mouse+2
  jsr lcd_write
  lda mouse+3
  jsr lcd_write
  lda mouse+4
  jsr lcd_write
  lda mouse+5
  jsr lcd_write
  lda mouse+6
  jsr lcd_write
  lda mouse+7
  jsr lcd_write
  lda mouse+8
  jsr lcd_write
  lda mouse+9
  jsr lcd_write
  lda mouse+10
  jsr lcd_write
  lda mouse+11
  jsr lcd_write
  lda mouse+12
  jsr lcd_write
  lda mouse+13
  jsr lcd_write
  lda mouse+14
  jsr lcd_write
  lda mouse+15
  jsr lcd_write
  lda mouse+16
  jsr lcd_write
  lda mouse+17
  jsr lcd_write
  lda mouse+18
  jsr lcd_write
  lda mouse+19
  jsr lcd_write
  lda mouse+20
  jsr lcd_write
  lda mouse+21
  jsr lcd_write
  lda mouse+22
  jsr lcd_write
  lda mouse+23
  jsr lcd_write
  lda mouse+24
  jsr lcd_write
  lda mouse+25
  jsr lcd_write
  lda mouse+26
  jsr lcd_write
  lda mouse+27
  jsr lcd_write
  lda mouse+28
  jsr lcd_write
  lda mouse+29
  jsr lcd_write
  lda mouse+30
  jsr lcd_write
  lda mouse+31
  jsr lcd_write

draw_mouse:
  lda #($80 | $07)              ; Move to DDRAM address $07 (line 1, pos 7)
  jsr lcd_instruction
  lda #1
  jsr lcd_write
  lda #($80 | $08)              ; Move to DDRAM address $08 (line 1, pos 8)
  jsr lcd_instruction
  lda #2
  jsr lcd_write
  lda #($80 | $47)              ; Move to DDRAM address $47 (line 2, pos 7)
  jsr lcd_instruction
  lda #3
  jsr lcd_write
  lda #($80 | $48)              ; Move to DDRAM address $48 (line 2, pos 8)
  jsr lcd_instruction
  lda #4
  jsr lcd_write

loop:
  lda #($80 | $03)              ; Move to DDRAM address $03 (line 1, pos 3)
  jsr lcd_instruction
  lda MOUSE_X
  jsr hex2dec
  jsr print_result
  lda #($80 | $0D)              ; Move to DDRAM address $0D (line 1, pos 14)
  jsr lcd_instruction
  lda MOUSE_Y
  jsr hex2dec
  jsr print_result
  lda #($80 | $43)              ; Move to DDRAM address $43 (line 2, pos 3)
  jsr lcd_instruction
  lda MOUSE_W
  jsr hex2dec
  jsr print_result
  lda #($80 | $4D)              ; Move to DDRAM address $4D (line 2, pos 14)
  jsr lcd_instruction
  jsr button_test_left
  jsr button_test_right
  jsr button_test_middle
  jmp loop

button_test_left:
  lda MOUSE_BTN                 
  and #%00000001
  bne button_test_left_pressed
  lda #$2D                      ; Dash
  jsr lcd_write
  jmp button_test_left_end
button_test_left_pressed:
  lda #$4C                      ; L
  jsr lcd_write
button_test_left_end:
  rts

button_test_right:
  lda MOUSE_BTN                 
  and #%00000010
  bne button_test_right_pressed
  lda #$2D                      ; Dash
  jsr lcd_write
  jmp button_test_right_end
button_test_right_pressed:
  lda #$52                      ; R
  jsr lcd_write
button_test_right_end:
  rts

button_test_middle:
  lda MOUSE_BTN                 
  and #%00000100
  bne button_test_middle_pressed
  lda #$2D                      ; Dash
  jsr lcd_write
  jmp button_test_middle_end
button_test_middle_pressed:
  lda #$4D                      ; M
  jsr lcd_write
button_test_middle_end:
  rts

print_result:
  ldx #0
print_loop:
  lda RESULT,x
  beq print_end
  jsr lcd_write
  inx
  jmp print_loop
print_end:
  rts

lcd_wait:
  pha
  lda #%00000000                ; Port B is input
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
  lda #%11111111                ; Port B is output
  sta DDRB
  pla
  rts

lcd_instruction:
  jsr lcd_wait
  sta PORTB
  lda #0                        ; Clear RS/RW/E bits
  sta PORTA
  lda #E                        ; Set E bit to send instruction
  sta PORTA
  lda #0                        ; Clear RS/RW/E bits
  sta PORTA
  rts

lcd_write:
  jsr lcd_wait
  sta PORTB
  lda #RS                       ; Set RS; Clear RW/E bits
  sta PORTA
  lda #(RS | E)                 ; Set E bit to send instruction
  sta PORTA
  lda #RS                       ; Clear E bits
  sta PORTA
  rts

hex2dec:
  sta VALUE
  lda #0
  sta VALUE + 1
  lda #0
  sta RESULT        ; Intialize the result
  sta RESULT + 1
  sta RESULT + 2
  
hex2dec_divide:
  lda #0            ; Initialize the remainder to zero
  sta MOD10
  sta MOD10 + 1
  clc

  ldx #16
hex2dec_loop:
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
  bcc hex2dec_ignore ; Branch if dividend < divisor
  sty MOD10
  sta MOD10 + 1

hex2dec_ignore:
  dex
  bne hex2dec_loop
  rol VALUE
  rol VALUE + 1

  lda MOD10
  clc
  adc #48           ; ASCII "0"
  jsr push_char

  lda VALUE         ; If value != 0, then continue dividing
  ora VALUE + 1
  bne hex2dec_divide        ; Branch if value != 0
  rts

push_char:
  pha               ; Push new first char onto stack
  ldy #0  
push_char_loop:
  lda RESULT,y      ; Get char on string and put in X
  tax
  pla
  sta RESULT,y      ; Pull char off stack and add it to the string
  iny
  txa
  pha               ; Push char from string onto stack
  bne push_char_loop
  pla
  sta RESULT,y      ; Pull the null off the stack and add to the end of the string
  rts

line_1_template: .asciiz "X:+000 <> Y:+000"
line_2_template: .asciiz "W:+000 <> B: ---"

mouse:
  .byte $00, $00, $02, $05, $03, $01, $03, $03
  .byte $00, $00, $08, $14, $18, $10, $18, $18
  .byte $07, $07, $07, $0B, $12, $08, $04, $08
  .byte $1C, $1C, $1C, $18, $08, $00, $00, $00

.segment "VECTORS"

  .word $0000
  .word reset
  .word $0000