.setcpu "65C02"
.segment "CODE"

VDP_DATA   = $9C00
VDP_REG    = $9C01

reset:
  ldx #$ff
  txs
  
  ; Initialize VDP in text mode
  lda #$00            ; Set mode to text
  sta VDP_REG         
  lda #$80
  sta VDP_REG
  lda #$D0            ; 16K, Enable Display, Disable Interrupts
  sta VDP_REG         
  lda #$81
  sta VDP_REG
  lda #$02            ; Address of name table in VRAM = Hex $0800
  sta VDP_REG
  lda #$82
  sta VDP_REG
  lda #$00            ; Address of pattern table in VRAM = Hex $0000
  sta VDP_REG
  lda #$84
  sta VDP_REG
  lda #$F5            ; White text on light blue background
  sta VDP_REG
  lda #$87
  sta VDP_REG

  ; Load pattern data into VRAM
  lda #$00            ; Set VRAM write address to $0000
  sta VDP_REG
  lda #$40
  sta VDP_REG

  ldx #$00
write_smile:
  lda smile,x
  sta VDP_DATA
  inx
  cpx #$08
  bne write_smile

  ; Write character to name table
  lda #$00            ; Set VRAM write address to $0800
  sta VDP_REG
  lda #$48
  sta VDP_REG

  ldx #$00
  ; Fill some of the screen with smiles
fill:
  lda #$00            ; Pattern table code for smile
  sta VDP_DATA
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

.segment "VECTORS"

  .word reset
  .word reset
  .word reset