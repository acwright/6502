.setcpu "65C02"
.segment "CODE"

TERM_DATA   = $8800
KBD_DATA    = $8801
KBD_ISR     = $8802

loop:
  ; lda KBD_DATA
  ; beq loop          ; Is data ready (not zero)?
  ; sta TERM_DATA     ; Output char to terminal
  jmp loop

.segment "VECTORS"

  .word loop
  .word loop
  .word loop