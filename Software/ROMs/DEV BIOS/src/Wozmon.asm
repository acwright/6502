; ***             ***
; ***   WOZMON    ***
; ***             ***

WOZ_XAML  = $24               ; Last "opened" location Low
WOZ_XAMH  = $25               ; Last "opened" location High
WOZ_STL   = $26               ; Store address Low
WOZ_STH   = $27               ; Store address High
WOZ_L     = $28               ; Hex value parsing Low
WOZ_H     = $29               ; Hex value parsing High
WOZ_YSAV  = $2A               ; Used to see if hex value is given
WOZ_MODE  = $2B               ; $00=XAM, $7F=STOR, $AE=BLOCK XAM

WOZ_MON:
  lda     #$1B                ; Begin with escape

WOZ_NOTCR:
  cmp     #$08                ; Backspace key?
  beq     WOZ_BACKSPACE       ; Yes
  cmp     #$1B                ; ESC?
  beq     WOZ_ESCAPE          ; Yes
  iny                         ; Advance text index
  bpl     WOZ_NEXTCHAR        ; Auto ESC if line longer than 127

WOZ_ESCAPE:
  lda     #$5C                ; "\"
  jsr     WOZ_ECHO            ; Output it

WOZ_GETLINE:
  lda     #$0D                ; Send CR
  jsr     WOZ_ECHO
  lda     #$0A                ; Send LF
  jsr     WOZ_ECHO

  ldy     #$01                ; Initialize text index
WOZ_BACKSPACE:      
  dey                         ; Back up text index
  bmi     WOZ_GETLINE         ; Beyond start of line, reinitialize

WOZ_NEXTCHAR:
  jsr     CHRIN               ; Get next character
  bcc     WOZ_NEXTCHAR        ; No character found
  sta     INPUT_BUFFER,y      ; Add to text buffer
  cmp     #$0D                ; CR?
  bne     WOZ_NOTCR           ; No

  ldy     #$FF                ; Reset text index
  lda     #$00                ; For XAM mode
  tax                         ; X=0
WOZ_SETBLOCK:
  asl
WOZ_SETSTOR:
  asl                         ; Leaves $7B if setting STOR mode
  sta     WOZ_MODE            ; $00 = XAM, $74 = STOR, $B8 = BLOK XAM
WOZ_BLSKIP:
  iny                         ; Advance text index
WOZ_NEXTITEM:
  lda     INPUT_BUFFER,y      ; Get character
  cmp     #$0D                ; CR?
  beq     WOZ_GETLINE         ; Yes, done this line
  cmp     #$2E                ; "."?
  bcc     WOZ_BLSKIP          ; Skip delimiter
  beq     WOZ_SETBLOCK        ; Set BLOCK XAM mode
  cmp     #$3A                ; ":"?
  beq     WOZ_SETSTOR         ; Yes, set STOR mode
  cmp     #$52                ; "R"?
  beq     WOZ_RUN             ; Yes, run user program
  stx     WOZ_L               ; $00 -> L
  stx     WOZ_H               ;    and H
  sty     WOZ_YSAV            ; Save Y for comparison

WOZ_NEXTHEX:
  lda     INPUT_BUFFER,y      ; Get character for hex test
  eor     #$30                ; Map digits to $0-9
  cmp     #$0A                ; Digit?
  bcc     WOZ_DIG             ; Yes
  adc     #$88                ; Map letter "A"-"F" to $FA-FF
  cmp     #$FA                ; Hex letter?
  bcc     WOZ_NOTHEX          ; No, character not hex
WOZ_DIG:
  asl
  asl                         ; Hex digit to MSD of A
  asl
  asl

  ldx     #$04                ; Shift count
WOZ_HEXSHIFT:
  asl                         ; Hex digit left, MSB to carry
  rol     WOZ_L               ; Rotate into LSD
  rol     WOZ_H               ; Rotate into MSD's
  dex                         ; Done 4 shifts?
  bne     WOZ_HEXSHIFT        ; No, loop
  iny                         ; Advance text index
  bne     WOZ_NEXTHEX         ; Always taken. Check next character for hex

WOZ_NOTHEX:
  cpy     WOZ_YSAV            ; Check if L, H empty (no hex digits)
  beq     WOZ_ESCAPE          ; Yes, generate ESC sequence

  bit     WOZ_MODE            ; Test MODE byte
  bvc     WOZ_NOTSTOR         ; B6=0 is STOR, 1 is XAM and BLOCK XAM

  lda     WOZ_L               ; LSD's of hex data
  sta     (WOZ_STL,x)         ; Store current 'store index'
  inc     WOZ_STL             ; Increment store index
  bne     WOZ_NEXTITEM        ; Get next item (no carry)
  inc     WOZ_STH             ; Add carry to 'store index' high order
WOZ_TONEXTITEM:     
  jmp     WOZ_NEXTITEM        ; Get next command item

WOZ_RUN:
  jsr     WOZ_JUMP            ; Begin executing at XAM index
  jmp     WOZ_MON             ; After returning we reset
WOZ_JUMP:
  jmp     (WOZ_XAML)          ; Run at current XAM index

WOZ_NOTSTOR:
  bmi     WOZ_XAMNEXT         ; B7 = 0 for XAM, 1 for BLOCK XAM

  ldx     #$02                ; Byte count
WOZ_SETADR:         
  lda     WOZ_L-1,x           ; Copy hex data to
  sta     WOZ_STL-1,x         ;  'store index'
  sta     WOZ_XAML-1,x        ; And to 'XAM index'
  dex                         ; Next of 2 bytes
  bne     WOZ_SETADR          ; Loop unless X = 0

WOZ_NXTPRNT:
  bne     WOZ_PRDATA          ; NE means no address to print
  lda     #$0D                ; CR
  jsr     WOZ_ECHO            ; Output it
  lda     #$0A                ; LF
  jsr     WOZ_ECHO            ; Output it
  lda     WOZ_XAMH            ; 'Examine index' high-order byte
  jsr     WOZ_PRBYTE          ; Output it in hex format
  lda     WOZ_XAML            ; Low-order 'examine index' byte
  jsr     WOZ_PRBYTE          ; Output it in hex format
  lda     #$3A                ; ":"
  jsr     WOZ_ECHO            ; Output it

WOZ_PRDATA:
  lda     #$20                ; Blank
  jsr     WOZ_ECHO            ; Output it
  lda     (WOZ_XAML,x)        ; Get data byte at 'examine index'
  jsr     WOZ_PRBYTE          ; Output it in hex format
WOZ_XAMNEXT:        
  stx     WOZ_MODE            ; 0 -> MODE (XAM mode)
  lda     WOZ_XAML
  cmp     WOZ_L               ; Compare 'examine index' to hex data
  lda     WOZ_XAMH
  sbc     WOZ_H
  bcs     WOZ_TONEXTITEM      ; Not less, so no more data to output

  inc     WOZ_XAML
  bne     WOZ_MOD8CHK         ; Increment 'examine index'
  inc     WOZ_XAMH

WOZ_MOD8CHK:
  lda     WOZ_XAML            ; Check low-order 'examine index' byte
  and     #$07                ; For MOD 8 = 0
  bpl     WOZ_NXTPRNT         ; Always taken

WOZ_PRBYTE:
  pha                         ; Save A for LSD
  lsr
  lsr
  lsr                         ; MSD to LSD position
  lsr
  jsr     WOZ_PRHEX           ; Output hex digit
  pla                         ; Restore A

WOZ_PRHEX:
  and     #$0F                ; Mask LSD for hex print
  ora     #$30                ; Add "0"
  cmp     #$3A                ; Digit?
  bcc     WOZ_ECHO            ; Yes, output it
  adc     #$06                ; Add offset for letter

WOZ_ECHO:
  jsr CHROUT                  ; Output the character
  rts                         ; Return