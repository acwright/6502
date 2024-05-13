.setcpu "65C02"
.segment "CODE"

PORTB     = $8000
PORTA     = $8001
DDRB      = $8002
DDRA      = $8003

KBD_DATA  = $9C00
KBD_CTRL  = $9C01

E         = %10000000
RW        = %01000000
RS        = %00100000

reset:
  ldx #$ff
  txs

  lda #00000000  ; Disable keyboard interrupts
  sta KBD_CTRL

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

loop:
  lda KBD_DATA
  bit #%10000000  ; Is data ready?
  beq loop
  and #%01111111  ; Clear the data ready bit
  sta KBD_DATA    ; Store back to data register to ack handling
  jsr print_char
  jmp loop
  
lcd_wait:
  pha
  lda #%00000000  ; Port B is input
  sta DDRB
lcdbusy:
  lda #RW
  sta PORTA
  lda #(RW | E)
  sta PORTA
  lda PORTB
  and #%10000000
  bne lcdbusy

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

  .word $0000
  .word reset
  .word $0000