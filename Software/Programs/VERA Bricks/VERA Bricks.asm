.setcpu "65C02"

.include "../../6502.inc"

.segment "CODE"

;*******************************************************************************
; Build a 16x16 256 color tile in VRAM location $12000
;*******************************************************************************
    stz VERA_CTRL                       ; Use Data Register 0
    lda #$11
    sta VERA_ADDR_HIGH                  ; Set Increment to 1, High Byte to 1
    lda #$20
    sta VERA_ADDR_MID                   ; Set Middle Byte to $20
    stz VERA_ADDR_LOW                   ; Set Low Byte to $00

    ldx #0                              
:   lda brick,x                         ; read from Brick Data
    sta VERA_DATA0                      ; Write to VRAM with +1 Autoincrement
    inx
    bne :-

;*******************************************************************************
; Configure Layer 0
;*******************************************************************************
    lda #%00000011                      ; 32 x 32 tiles, 8 bits per pixel
    sta VERA_L0_CONFIG
    lda #$20                            ; $20 points to $4000 in VRAM
    sta VERA_L0_MAPBASE                  ; Store to Map Base Pointer

    lda #$93                            ; $93 points to $12000, Width and Height 16 pixel
    sta VERA_L0_TILEBASE                ; Store to Tile Base Pointer

;*******************************************************************************
; Fill the Layer 0 with all bricks
;*******************************************************************************
    stz VERA_CTRL                       ; Use Data Register 0
    lda #$10
    sta VERA_ADDR_HIGH                  ; Set Increment to 1, High Byte to 0
    lda #$40
    sta VERA_ADDR_MID                   ; Set Middle Byte to $40
    stz VERA_ADDR_LOW                   ; Set Low Byte to $00

    ldy #32
    lda #0                              
:   ldx #32
:   sta VERA_DATA0                      ; Write to VRAM with +1 Autoincrement
    sta VERA_DATA0                      ; Write Attribute
    dex     
    bne :-
    dey
    bne :--

;*******************************************************************************
; Turn on Layer 0
;*******************************************************************************
    lda #%00010001                      ; Turn on layer 0, VGA Mode
    sta VERA_DC_VIDEO                   

;*******************************************************************************
; Scale Display x2 for resolution of 320 x 240 pixels
;*******************************************************************************
    lda #$40
    sta VERA_DC_HSCALE
    sta VERA_DC_VSCALE

loop:
  jmp loop                             ; Halt

brick:
    .byte $08,$08,$08,$08,$08,$08,$08,$E5,$08,$08,$08,$08,$08,$08,$08,$08
    .byte $2A,$2A,$2A,$2A,$2A,$2A,$29,$E5,$08,$2A,$2A,$2A,$2A,$2A,$2A,$2A
    .byte $2A,$2A,$2A,$2A,$2A,$2A,$29,$E5,$08,$2A,$2C,$2A,$2A,$2A,$2A,$2A
    .byte $2A,$2A,$2C,$2C,$2A,$2A,$29,$E5,$08,$2A,$2A,$2A,$2A,$2A,$2A,$2A
    .byte $2A,$2A,$2A,$2A,$2A,$2A,$29,$E5,$08,$2A,$2A,$2A,$2A,$2A,$2A,$2A
    .byte $2A,$2A,$2A,$2A,$2A,$2A,$29,$E5,$08,$2A,$2A,$2A,$2A,$29,$29,$2A
    .byte $29,$29,$29,$29,$29,$29,$29,$E5,$08,$29,$29,$29,$29,$29,$29,$29
    .byte $E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5
    .byte $E5,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08
    .byte $E5,$08,$08,$2A,$2C,$2C,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$29
    .byte $E5,$08,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$29
    .byte $E5,$08,$2A,$2A,$2A,$2A,$29,$29,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$29
    .byte $E5,$08,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$29,$2A,$29
    .byte $E5,$08,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$29
    .byte $E5,$08,$29,$29,$29,$29,$29,$29,$29,$29,$29,$29,$29,$29,$29,$29
    .byte $E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5,$E5