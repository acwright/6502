.setcpu "65C02"
.segment "CODE"

PORTB   = $9800
PORTA   = $9801
DDRB    = $9802
DDRA    = $9803

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

  jsr ay_latch
  lda #$07
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #%00111110
  sta PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$08
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #%00001111
  sta PORTA
  jsr ay_inactive

loop:
  jsr ay_latch
  lda #$00
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #223
  sta PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #170
  sta PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #123
  sta PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #102
  sta PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #63
  sta PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #28
  sta PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #253
  sta PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta PORTA
  jsr ay_inactive
  jsr ay_write
  lda #0
  sta PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jmp loop

delay:
  phx
  ldx #$FF
delay_loop:
  dex
  bne delay_loop
  plx
  rts

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

ay_latch:
  phx
  ldx #(AY_BC1 | AY_BDIR | AY_RESB)   ; BDIR HIGH, BC1 HIGH, RESB HIGH
  stx PORTB
  plx
  rts

ay_write:
  phx
  ldx #(AY_BDIR | AY_RESB)            ; BDIR HIGH, BC1 LOW, RESB HIGH
  stx PORTB
  plx
  rts

.segment "VECTORS"

  .word reset
  .word reset
  .word reset