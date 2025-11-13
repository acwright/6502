.include "../include/6502.inc"

.segment "KERNAL"
.include "Wozmon.asm"
.align 256
.include "Kernal.asm"

.segment "CODE"

.segment "VECTORS"
.word   nmi           ; NMI vector
.word   reset         ; RESET vector
.word   irq           ; IRQ vector