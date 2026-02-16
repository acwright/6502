; ***             ***
; ***   DB BIOS   ***
; ***             ***

.setcpu "65C02"

.include "../../6502.inc"

.segment "KERNAL"
.include "src/Kernal.asm"
.segment "CART"
.include "src/Cart.asm"
.segment "WOZMON"
.include "src/Wozmon.asm"
.segment "VECTORS"
.include "src/Vectors.asm"