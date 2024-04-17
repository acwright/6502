;-----------------------------------------------------------------------------
; text_input.s
; Copyright (C) 2020 Frank van den Hoef
;-----------------------------------------------------------------------------

        .include "text_input.inc"
	.include "lib.inc"
	.include "text_display.inc"
	.include "ps2.inc"

;-----------------------------------------------------------------------------
; Variables
;-----------------------------------------------------------------------------
	.bss
line_len:
	.byte 0
line_buf:
	.res MAX_LINE_LEN + 1
line_start:
	.byte 0

	.code

;-----------------------------------------------------------------------------
; getchar
;-----------------------------------------------------------------------------
.proc getchar
	jsr ps2_getkey
	cmp #0
	beq getchar
	rts
.endproc

;-----------------------------------------------------------------------------
; getline
;-----------------------------------------------------------------------------
.proc getline
	stz line_len
	stz line_start

next_char:
	jsr getchar

	cmp #10
	beq enter
	cmp #8
	beq backspace

	ldx line_len
	cpx #MAX_LINE_LEN
	beq next_char

	sta line_buf, x
	inc line_len

	jsr putchar

	bra next_char

backspace:
	ldx line_len
	beq next_char
	dec line_len
	jsr putchar
	bra next_char

enter:	jsr putchar

	; Zero terminate line_buf
	ldx line_len
	stz line_buf, x
	rts
.endproc

;-----------------------------------------------------------------------------
; skip_spaces
;-----------------------------------------------------------------------------
.proc skip_spaces
	ldx line_start
	dex
:	inx
	lda line_buf, x
	cmp #' '
	beq :-

	stx line_start
	rts
.endproc

;-----------------------------------------------------------------------------
; to_upper
;-----------------------------------------------------------------------------
.proc to_upper
	; Lower case character?
	cmp #'a'
	bcc done
	cmp #'z'+1
	bcs done

	; Make uppercase
	and #$DF
done:
	rts
.endproc

;-----------------------------------------------------------------------------
; to_lower
;-----------------------------------------------------------------------------
.proc to_lower
	; Lower case character?
	cmp #'A'
	bcc done
	cmp #'Z'+1
	bcs done

	; Make lowercase
	ora #$20
done:
	rts
.endproc

;-----------------------------------------------------------------------------
; first_word_to_upper
;-----------------------------------------------------------------------------
.proc first_word_to_upper
	ldx line_start
	dex
next:	inx
	lda line_buf, x
	beq done
	cmp #' '
	beq done
	jsr to_upper
	sta line_buf, x
	bra next
done:
	rts
.endproc

;-----------------------------------------------------------------------------
; terminate_and_skip_to_next_word
;-----------------------------------------------------------------------------
.proc terminate_and_skip_to_next_word
	; Find next space
	ldx line_start
	dex
next:	inx
	lda line_buf, x
	beq no_word
	cmp #' '
	bne next

	; Zero-terminate
	stz line_buf, x
	inx
	stx line_start

	; Skip spaces
	jsr skip_spaces

	; End of string?
	ldx line_start
	lda line_buf, x
	beq no_word

	sec
	rts

no_word:
	clc
	rts
.endproc
