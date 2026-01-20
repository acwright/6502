; ***             ***
; ***   DB BIOS   ***
; ***             ***

.setcpu "65C02"

.include "../../../6502.inc"

.segment "ZEROPAGE"
ZP:     .res $100
.segment "STACK"
STACK: .res $100
.segment "INPUT_BUFFER"
INPUT_BUFFER: .res $100
.segment "KERNAL_VARS"
KERNAL_VARS: .res $100
.segment "USER_VARS"
USER_VARS: .res $400
.segment "PROGRAM"
.segment "KERNAL"
.include "Kernal.asm"
.segment "CART"
.segment "WOZMON"
.include "Wozmon.asm"
.segment "VECTORS"

.word   NMI_VEC           ; NMI vector
.word   RESET_VEC         ; RESET vector
.word   IRQ_VEC           ; IRQ vector