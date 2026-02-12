.setcpu "65C02"

.include "../../../6502.inc"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "PROGRAM"

reset:
  lda #%11111111          ; Set all pins on port A and B to outputs
  sta AY_DDRA
  sta AY_DDRB
  lda #$00
  sta AY_PORTA
  
  jsr ay_inactive

  jsr ay_latch
  lda #$07
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #%00111110
  sta AY_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$08
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #%00001111
  sta AY_PORTA
  jsr ay_inactive

loop:
  jsr ay_latch
  lda #$00
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #223
  sta AY_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta AY_PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #170
  sta AY_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta AY_PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #123
  sta AY_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta AY_PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #102
  sta AY_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta AY_PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #63
  sta AY_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta AY_PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #28
  sta AY_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta AY_PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #253
  sta AY_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta AY_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #0
  sta AY_PORTA
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

ay_inactive:
  phx
  ldx #(AY_BDIRL | AY_BC1L)   ; BDIR LOW, BC1 LOW
  stx AY_PCR
  plx
  rts

ay_latch:
  phx
  ldx #(AY_BDIRH | AY_BC1H)   ; BDIR HIGH, BC1 HIGH
  stx AY_PCR
  plx
  rts

ay_write:
  phx
  ldx #(AY_BDIRH | AY_BC1L)   ; BDIR HIGH, BC1 LOW
  stx AY_PCR
  plx
  rts
  
.segment "KERNAL"
.segment "CART"
.segment "WOZMON"
.segment "VECTORS"