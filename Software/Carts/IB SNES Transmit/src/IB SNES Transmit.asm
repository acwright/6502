; SNES CONTROLLERS
; byte 0:      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
;              | B | Y |SEL|STA|UP |DN |LT |RT |
;
; byte 1:      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
;              | A | X | L | R | 1 | 1 | 1 | 1 |
; byte 2:
;         $00 = controller present
;         $FF = controller not present

.setcpu "65C02"

.segment "ZEROPAGE"
SNES1:    .res 3
SNES2:    .res 3
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "CODE"

VIA_PORTB  = $9400
VIA_PORTA  = $9401
VIA_DDRB   = $9402  
VIA_DDRA   = $9403
VIA_T1L    = $9404
VIA_T1H    = $9405
VIA_ACR    = $940B
VIA_IFR    = $940D
VIA_IER    = $940E

ACIA_DATA   = $9000
ACIA_STATUS = $9001
ACIA_CMD    = $9002
ACIA_CTRL   = $9003

SNES_CLOCK = %00000001  ; PB0 CLOCK (both controllers)
SNES_LATCH = %00000010  ; PB1 LATCH (both controllers)
SNES_DATA1 = %00000100  ; PB2 DATA  (controller #1)
SNES_DATA2 = %00001000  ; PB3 DATA  (controller #2)

reset:
  sei            ; Disable interrupts
  ldx #$ff
  txs
  
  lda #$00
  sta SNES1
  sta SNES1+1
  sta SNES1+2
  sta SNES2
  sta SNES2+1
  sta SNES2+2

  sta VIA_DDRA   ; Set VIA A as input

  lda #$FF-SNES_DATA1-SNES_DATA2
  sta VIA_DDRB   ; Set VIA B as output with PB2 and PB3 as input

  lda #%11000000
  sta VIA_ACR    ; Enable Timer1 continuous interrupts
  sta VIA_IER    ; Enable Timer1 IRQ

  lda #$FF         ; 15.26 Hz, slowest we can go
  sta VIA_T1H      ; Load high byte of countdown value
  sta VIA_T1L      ; Load low byte of countdown value

  lda #$00
  sta ACIA_STATUS ; Soft reset (value not important)
  lda #%00010000  ; N-8-1, 115200 baud
  sta ACIA_CTRL
  lda #%00001011  ; No parity, No echo, No interrupts
  sta ACIA_CMD

  cli            ; Enable interrupts

loop:
  jmp loop          ; Infinite loop

snes_read:
  ; Clear VIA PORTB
	lda #$00
	sta VIA_PORTB

	; Pulse latch
	lda #SNES_LATCH
	sta VIA_PORTB
	lda #$00
	sta VIA_PORTB

	; Read 3x 8 bits
	ldx #$00
snes_read_l2:	
  ldy #$08
snes_read_l1:	
  lda VIA_PORTB
	cmp #SNES_DATA2
	rol SNES2,x
	and #SNES_DATA1
	cmp #SNES_DATA1
	rol SNES1,x
	lda #SNES_CLOCK
	sta VIA_PORTB
	lda #$00
	sta VIA_PORTB
	dey
	bne snes_read_l1
	inx
	cpx #$03
	bne snes_read_l2
	rts

snes_send:
  ; Read 3x 8 bits at SNES1
	ldx #$00
snes1_send:
  lda SNES1,x
  jsr send_char
  inx
	cpx #$03
	bne snes1_send

  ; Read 3x 8 bits at SNES2
	ldx #$00
snes2_send:
  lda SNES2,x
  jsr send_char
  inx
	cpx #$03
	bne snes2_send
  rts

; Transmit a byte using the ACIA
; Modifies: flags
send_char:
  sta ACIA_DATA 
  pha
tx_wait:
  lda ACIA_STATUS
  and #%00010000  ; Check if tx buffer not empty
  beq tx_wait     ; Loop if tx buffer not empty
  pla
  rts

nmi:
irq:
  lda VIA_IFR
  and #%01000000    ; Check if Timer1 caused the interrupt
  beq irq_exit      ; If not, exit
  lda VIA_T1L
  sei               ; Disable interrupts while we read and send data
  jsr snes_read
  jsr snes_send
  cli               ; Re-enable interrupts
irq_exit:
  rti

.segment "VECTORS"

.word   nmi          ; NMI vector
.word   reset        ; RESET vector
.word   irq          ; IRQ vector