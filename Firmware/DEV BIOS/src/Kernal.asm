; ***             ***
; ***   KERNAL    ***
; ***             ***

; Main entry point
RESET:
  cld                           ; Clear decimal mode
  sei                           ; Disable interrupts

  ldx #$ff                      
  txs                           ; Reset the stack pointer

  lda #<IRQ                     ; Initialize the IRQ pointer
  sta IRQ_PTR
  lda #>IRQ
  sta IRQ_PTR + 1

  lda #<NMI                     ; Initialize the NMI pointer
  sta NMI_PTR
  lda #>NMI
  sta NMI_PTR + 1

  jsr INIT_BUFFER               ; Initialize the input buffer
  jsr INIT_SC                   ; Initialize the serial output

  cli                           ; Enable interrupts

  jmp WOZ_MON                   ; Jump to Wozmon

; Initialize the serial output
; Modifies: Flags, A
INIT_SC:
  lda     #$10              ; 8-N-1, 115200 baud
  sta     SC_CTRL
  lda     #$09              ; No parity, no echo, transmit interrupts disabled, receive interrupts enabled
  sta     SC_CMD
  rts

; Initialize the INPUT_BUFFER
; Modifies: Flags, A
INIT_BUFFER:
  lda #$00
  sta READ_PTR                  ; Init read and write pointers
  sta WRITE_PTR
  rts

; Write a character from the A register to the INPUT_BUFFER
; Modifies: Flags, X
WRITE_BUFFER:
  ldx WRITE_PTR
  sta INPUT_BUFFER,x
  inc WRITE_PTR
  rts

; Read a character from the INPUT_BUFFER and store it in A register
; Modifies: Flags, X, A
READ_BUFFER:
  ldx READ_PTR
  lda INPUT_BUFFER,x
  inc READ_PTR
  rts

; Return in A register the number of unread bytes in the INPUT_BUFFER
; Modifies: Flags, A
BUFFER_SIZE:  
  lda WRITE_PTR
  sec
  sbc READ_PTR
  rts

; Get a character from the INPUT_BUFFER if available
; On return, carry flag indicates whether a character was available
; If character available the character will be in the A register
; Modifies: Flags, A
CHRIN:
  phx
  jsr BUFFER_SIZE               ; Check for character available
  beq @CHRIN_NO_CHAR            ; Branch if no character available
  jsr READ_BUFFER               ; Read the character from the buffer
  jsr CHROUT                    ; Echo
  plx
  sec
  rts
@CHRIN_NO_CHAR:
  plx
  clc
  rts

; Output a character from the A register to the Serial Card
; Modifies: Flags
CHROUT:
  sta SC_DATA
  pha
@CHROUT_WAIT:
  lda SC_STATUS
  and #%00010000                ; Check if tx buffer not empty
  beq @CHROUT_WAIT              ; Loop if tx buffer not empty
  pla
  rts

; NMI Handler
NMI:
  rti

; IRQ Handler
IRQ:
  pha
  phx
  lda SC_STATUS
  and #%10000000                ; Check if serial data caused the interrupt
  beq @IRQ_EXIT                 ; If not, exit
  lda SC_DATA                   ; Read the data from serial register
  jsr WRITE_BUFFER              ; Store to the input buffer
@IRQ_EXIT:
  plx
  pla
  rti

; NMI Vector
NMI_VEC:
  jmp (NMI_PTR)                 ; Indirect jump through NMI pointer to the NMI handler

; Reset Vector
RESET_VEC:
  jmp RESET                     ; Initialize the system

; IRQ Vector
IRQ_VEC:
  jmp (IRQ_PTR)                 ; Indirect jump through IRQ pointer to the IRQ handler