.setcpu "65C02"

.include "../../../6502.inc"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "PROGRAM"

reset:
  lda #$00            ; Initialize VC in text mode
  sta VC_REG         
  lda #$80
  sta VC_REG
  lda #$D0            ; 16K, Enable Display, Disable Interrupts
  sta VC_REG         
  lda #$81
  sta VC_REG
  lda #$02            ; Address of name table in VRAM = Hex $0800
  sta VC_REG
  lda #$82
  sta VC_REG
  lda #$00            ; Address of pattern table in VRAM = Hex $0000
  sta VC_REG
  lda #$84
  sta VC_REG
  lda #$F5            ; White text on light blue background
  sta VC_REG
  lda #$87
  sta VC_REG

                      ; Load pattern data into VRAM
  lda #$00            ; Set VRAM write address to $0000
  sta VC_REG
  lda #$40
  sta VC_REG

  ldx #$00
write_smile:
  lda smile,x
  sta VC_DATA
  inx
  cpx #$08
  bne write_smile

  ; Write character to name table
  lda #$00            ; Set VRAM write address to $0800
  sta VC_REG
  lda #$48
  sta VC_REG

  ldx #$00
  ; Fill some of the screen with smiles
fill:
  lda #$00            ; Pattern table code for smile
  sta VC_DATA
  inx
  cpx #$FF            ; Fill 256 characters
  bne fill

loop:
  jmp loop

smile:
  .byte %00111100
  .byte %01000010
  .byte %10100101
  .byte %10000001
  .byte %10100101
  .byte %10011001
  .byte %01000010
  .byte %00111100

.segment "KERNAL"
.segment "CART"
.segment "WOZMON"
.segment "VECTORS"