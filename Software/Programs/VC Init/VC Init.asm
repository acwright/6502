.setcpu "65C02"

.include "../../6502.inc"

.segment "CODE"

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

loop:
  jmp loop