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
  sta SND_DDRA
  sta SND_DDRB
  lda #$00
  sta SND_PORTA
  
  jsr ay_inactive

  jsr ay_latch
  lda #$07
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #%00111110
  sta SND_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$08
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #%00001111
  sta SND_PORTA
  jsr ay_inactive

loop:
  jsr ay_latch
  lda #$00
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #223
  sta SND_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta SND_PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #170
  sta SND_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta SND_PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #123
  sta SND_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta SND_PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #102
  sta SND_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta SND_PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #63
  sta SND_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta SND_PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #28
  sta SND_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #1
  sta SND_PORTA
  jsr ay_inactive

  jsr delay
  jsr delay
  jsr delay
  jsr delay

  jsr ay_latch
  lda #$00
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #253
  sta SND_PORTA
  jsr ay_inactive

  jsr ay_latch
  lda #$01
  sta SND_PORTA
  jsr ay_inactive
  jsr ay_write
  lda #0
  sta SND_PORTA
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
  ldx #(SND_BDIRL | SND_BC1L)   ; BDIR LOW, BC1 LOW
  stx SND_PCR
  plx
  rts

ay_latch:
  phx
  ldx #(SND_BDIRH | SND_BC1H)   ; BDIR HIGH, BC1 HIGH
  stx SND_PCR
  plx
  rts

ay_write:
  phx
  ldx #(SND_BDIRH | SND_BC1L)   ; BDIR HIGH, BC1 LOW
  stx SND_PCR
  plx
  rts
  
.segment "KERNAL"
.segment "CART"
.segment "WOZMON"
.segment "VECTORS"