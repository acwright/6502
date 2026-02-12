; ***             ***
; ***   DB BIOS   ***
; ***             ***

.setcpu "65C02"

.include "../../../6502.inc"

.segment "KERNAL"
.include "Kernal.asm"
.segment "CART"
.include "Cart.asm"
.segment "WOZMON"
.include "Wozmon.asm"
.segment "VECTORS"
.include "Vectors.asm"