.setcpu "65C02"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "CODE"

PORTB   = $9800
PORTA   = $9801
DDRB    = $9802
DDRA    = $9803
PCR     = $980C

AY_BC1H  = %11100000       ; VIA PCR CA2
AY_BC1L  = %11000000       ; VIA PCR CA2
AY_BDIRH = %00001110       ; VIA PCR CB2
AY_BDIRL = %00001100       ; VIA PCR CB2

reset:
  lda #%11111111          ; Set all pins on port A and B to outputs
  sta DDRA
  sta DDRB
  lda #$00
  sta PORTA
  
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

ay_inactive:
  phx
  ldx #(AY_BDIRL | AY_BC1L)   ; BDIR LOW, BC1 LOW
  stx PCR
  plx
  rts

ay_latch:
  phx
  ldx #(AY_BDIRH | AY_BC1H)   ; BDIR HIGH, BC1 HIGH
  stx PCR
  plx
  rts

ay_write:
  phx
  ldx #(AY_BDIRH | AY_BC1L)   ; BDIR HIGH, BC1 LOW
  stx PCR
  plx
  rts
  