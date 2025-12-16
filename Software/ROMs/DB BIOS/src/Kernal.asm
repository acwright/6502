; ***             ***
; ***   KERNAL    ***
; ***             ***

; Initialization main entry point
; Modifies: Flags, A
INIT:
  cld                           ; Clear decimal mode
  rts

; Initialize the Serial Card
; Modifies: Flags, A
INIT_SC:
  sei                           ; Disable interrupts
  lda #$10                      ; 8-N-1, 115200 baud.
  sta SC_CTRL
  lda #$09                      ; No parity, no echo, interrupts enabled.
  sta SC_CMD
  cli                           ; Enable interrupts
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
; If character available the character will be in the A regsiter
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

; Reset Handler
RESET:
  jsr INIT                      ; Initialize the system
  jsr INIT_BUFFER               ; Initialize the INPUT_BUFFER
  jsr INIT_SC                   ; Initialize the Serial Card
  jmp WOZ_MON                   ; Jump to Wozmon

; NMI Handler
NMI:
  rti

; IRQ Handler
IRQ:
  pha
  phx
  lda SC_STATUS
  and #%10000000                ; Check if Serial Card caused the interrupt
  beq @IRQ_EXIT                 ; If not, exit
  lda SC_DATA                   ; Read the data from ACIA
  jsr WRITE_BUFFER              ; Store to the input buffer
@IRQ_EXIT:
  plx
  pla
  rti