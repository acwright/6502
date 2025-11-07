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

ACIA_DATA   = $9000
ACIA_STATUS = $9001
ACIA_CMD    = $9002
ACIA_CTRL   = $9003

reset:
  sei            ; Disable interrupts
  ldx #$ff
  txs

  lda #$00
  sta READ_PTR
  sta WRITE_PTR

  sta ACIA_STATUS ; Soft reset (value not important)

  lda #%00011111  ; N-8-1, 19200 baud
  sta ACIA_CTRL

  lda #%00001001  ; No parity, No echo, RTS low, IRQ Enabled, DTR Ready
  sta ACIA_CMD

  cli             ; Enable interrupts

loop:
  jsr buffer_size   ; Is there a new input?
  beq loop
  jsr read_buffer
  jsr send_char
  jsr buffer_size   ; Is the buffer still mostly full?
  cmp #$B0
  bcs loop_mostly_full
  lda #%00001001    ; Re-enable RTS - No parity, No echo, RTS low, IRQ Enabled, DTR Ready
  sta ACIA_CMD
loop_mostly_full:
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
  lda ACIA_STATUS
  and #%10000000    ; Check if ACIA caused the interrupt
  beq irq_exit      ; If not, exit
  lda ACIA_DATA     ; Read the data from ACIA
  jsr write_buffer  ; Store to the input buffer
  jsr buffer_size   ; Check if buffer is almost full
  cmp #$f0          
  bcc irq_exit
  lda #%00000001    ; Disable RTS - No parity, No echo, RTS high, IRQ Enabled, DTR Ready
  sta ACIA_CMD
irq_exit:
  plx
  pla
  rti

.segment "VECTORS"

  .word nmi
  .word reset
  .word irq