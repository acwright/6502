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
  lda #80
  sta VDP_REG
  lda #$D0            ; 16K, Enable Display, Disable Interrupts
  sta VDP_REG         
  lda #81
  sta VDP_REG
  lda #$02            ; Address of name table in VRAM = Hex $0800
  sta VDP_REG
  lda #$82
  sta VDP_REG
  lda #$00            ; Address of pattern table in VRAM = Hex $0000
  sta VDP_REG
  lda #$84
  sta VDP_REG
  lda #$00            ; White text on light blue background
  sta VDP_REG
  lda #$87
  sta VDP_REG

  ldx $00
loop:
  inx
  txa
  sta VDP_REG
  lda #$87
  sta VDP_REG
  jsr tmswait
  jmp loop

tmswait:
  txa
  pha
  ldy #$FF            ; Outer loop counter
tmswait_inner:
  ldx #$FF            ; Inner loop counter
tmswait_loop:
  dex
  bne tmswait_loop
  dey                 ; Decrement outer loop counter
  bne tmswait_inner   ; Branch if outer loop counter not zero
  pla
  tax
  rts

.segment "VECTORS"

  .word reset
  .word reset
  .word reset