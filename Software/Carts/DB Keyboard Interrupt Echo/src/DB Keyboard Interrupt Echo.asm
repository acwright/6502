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

DB_DATA     = $8400
DB_KB_CMD   = $8402
DB_KB_DATA  = $8402

reset:
  sei            ; Disable interrupts
  ldx #$ff
  txs
  
  lda #$00
  sta READ_PTR    ; Initialize the read pointer
  sta WRITE_PTR   ; Initilize the write pointer

  lda #%10000000  ; Enable KB interrupts
  sta DB_KB_CMD   

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

; Transmit a byte using the DB print data register
; Modifies: None
send_char:
  sta DB_DATA 
  rts

nmi:
irq:
  pha
  phx
  lda DB_KB_DATA
  and #%10000000    ; Check if DB keyboard caused the interrupt
  beq irq_exit      ; If not, exit
  lda DB_KB_DATA    ; Read the data from DB keyboard data register
  jsr write_buffer  ; Store to the input buffer
irq_exit:
  plx
  pla
  rti

.segment "VECTORS"

.word   nmi          ; NMI vector
.word   reset        ; RESET vector
.word   irq          ; IRQ vector