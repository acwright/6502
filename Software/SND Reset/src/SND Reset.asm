.setcpu "65C02"
.segment "CODE"

PORTB   = $8800
PORTA   = $8801
DDRB    = $8802
DDRA    = $8803

AY_BC1  = %00000001
AY_BDIR = %00000010
AY_RESB = %00000100

reset:
  lda #%11111111          ; Set all pins on port A and B to outputs
  sta DDRA
  sta DDRB
  lda #$00
  sta PORTA
  
  jsr ay_inactive
  jsr ay_reset
  jsr ay_inactive

loop:
  jmp loop

ay_reset:
  phx
  ldx #$00
  stx PORTB
  ldx #(AY_RESB)                      ; BDIR LOW, BC1 LOW, RESB LOW
  stx PORTB
  plx
  rts

ay_inactive:
  phx
  ldx #(AY_RESB)                      ; BDIR LOW, BC1 LOW, RESB HIGH
  stx PORTB
  plx
  rts

.segment "VECTORS"

  .word $0000
  .word reset
  .word $0000