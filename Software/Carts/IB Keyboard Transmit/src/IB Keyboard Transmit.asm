.setcpu "65C02"

.segment "ZEROPAGE"
READ_PTR:     .res 1
WRITE_PTR:    .res 1
.segment "STACK"
.segment "INPUT_BUFFER"
INPUT_BUFFER: .res $100
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "CODE"

VIA_PORTB  = $9400
VIA_PORTA  = $9401
VIA_DDRB   = $9402  
VIA_DDRA   = $9403
VIA_PCR    = $940C
VIA_IFR    = $940D
VIA_IER    = $940E

ACIA_DATA   = $9000
ACIA_STATUS = $9001
ACIA_CMD    = $9002
ACIA_CTRL   = $9003

reset:
  sei            ; Disable interrupts
  ldx #$ff
  txs
  
  lda #$00
  sta READ_PTR   ; Initialize the read pointer
  sta WRITE_PTR  ; Initilize the write pointer

  sta VIA_DDRA   ; Set VIA A as input
  sta VIA_DDRB   ; Set VIA B as input
  lda #%10000010 ; Enable IRQ on CA1
  sta VIA_IER
  lda #%00001101 ; Set CA2 to LOW output, CA1 to positive edge
  sta VIA_PCR

  lda #$00
  sta ACIA_STATUS ; Soft reset (value not important)
  lda #%00011111  ; N-8-1, 19200 baud
  sta ACIA_CTRL
  lda #%00001011  ; No parity, No echo, No interrupts
  sta ACIA_CMD

  lda VIA_PORTA   ; Clear any inital interrupts

  cli             ; Enable interrupts

loop:
  jsr buffer_size   ; Is there a new input?
  beq loop
  jsr read_buffer
  jsr send_char
  jmp loop          ; Infinite loop

; Write a byte from the A register to the input buffer
; Modifies: flags, X
write_buffer:
  ldx WRITE_PTR
  sta INPUT_BUFFER,x
  inc WRITE_PTR
  rts

; Read a byte from the input buffer and load into A register
; Modifies: flags, X, A
read_buffer:
  ldx READ_PTR
  lda INPUT_BUFFER,x
  inc READ_PTR
  rts

; Return (in A) the number of bytes in the input buffer
; Modifies: flags, A
buffer_size:
  lda WRITE_PTR
  sec
  sbc READ_PTR
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
  pha
  phx
  lda VIA_IFR
  and #%00000010    ; Check if CA1 caused the interrupt
  beq irq_exit      ; If not, exit
  lda VIA_PORTA     ; Read the data from VIA A
  jsr write_buffer  ; Store to the input buffer
irq_exit:
  plx
  pla
  rti

.segment "VECTORS"

  .word nmi
  .word reset
  .word irq