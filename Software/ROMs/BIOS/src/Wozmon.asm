XAML  = $24              ; Last "opened" location Low
XAMH  = $25              ; Last "opened" location High
STL   = $26              ; Store address Low
STH   = $27              ; Store address High
L     = $28              ; Hex value parsing Low
H     = $29              ; Hex value parsing High
YSAV  = $2A              ; Used to see if hex value is given
MODE  = $2B              ; $00=XAM, $7F=STOR, $AE=BLOCK XAM

IN    = INPUT_BUFFER     ; Input buffer

woz:
  lda     #$10           ; 8-N-1, 115200 baud.
  sta     SC_CTRL
  ldy     #$8B           ; No parity, no echo, no interrupts.
  sty     SC_CMD
  lda     #$1B           ; Begin with escape.

notcr:
  cmp     #$08           ; Backspace key?
  beq     backspace      ; Yes.
  cmp     #$1B           ; ESC?
  beq     escape         ; Yes.
  iny                    ; Advance text index.
  bpl     nextchar       ; Auto ESC if line longer than 127.

escape:
  lda     #$5C           ; "\".
  jsr     echo           ; Output it.

getline:
  lda     #$0D           ; Send CR
  jsr     echo

  ldy     #$01           ; Initialize text index.
backspace:      
  dey                    ; Back up text index.
  bmi     getline        ; Beyond start of line, reinitialize.

nextchar:
  lda     SC_STATUS      ; Check status.
  and     #$08           ; Key ready?
  beq     nextchar       ; Loop until ready.
  lda     SC_DATA        ; Load character. B7 will be '0'.
  sta     IN,Y           ; Add to text buffer.
  jsr     echo           ; Display character.
  cmp     #$0D           ; CR?
  bne     notcr          ; No.

  ldy     #$FF           ; Reset text index.
  lda     #$00           ; For XAM mode.
  tax                    ; X=0.
setblock:
  asl
setstor:
  asl                    ; Leaves $7B if setting STOR mode.
  sta     MODE           ; $00 = XAM, $74 = STOR, $B8 = BLOK XAM.
blskip:
  iny                    ; Advance text index.
nextitem:
  lda     IN,Y           ; Get character.
  cmp     #$0D           ; CR?
  beq     getline        ; Yes, done this line.
  cmp     #$2E           ; "."?
  bcc     blskip         ; Skip delimiter.
  beq     setblock       ; Set BLOCK XAM mode.
  cmp     #$3A           ; ":"?
  beq     setstor        ; Yes, set STOR mode.
  cmp     #$52           ; "R"?
  beq     run            ; Yes, run user program.
  stx     L              ; $00 -> L.
  stx     H              ;    and H.
  sty     YSAV           ; Save Y for comparison

nexthex:
  lda     IN,Y           ; Get character for hex test.
  eor     #$30           ; Map digits to $0-9.
  cmp     #$0A           ; Digit?
  bcc     dig            ; Yes.
  ADC     #$88           ; Map letter "A"-"F" to $FA-FF.
  cmp     #$FA           ; Hex letter?
  bcc     nothex         ; No, character not hex.
dig:
  asl
  asl                    ; Hex digit to MSD of A.
  asl
  asl

  ldx     #$04           ; Shift count.
hexshift:
  asl                    ; Hex digit left, MSB to carry.
  rol     L              ; Rotate into LSD.
  rol     H              ; Rotate into MSD's.
  dex                    ; Done 4 shifts?
  bne     hexshift       ; No, loop.
  iny                    ; Advance text index.
  bne     nexthex        ; Always taken. Check next character for hex.

nothex:
  cpy     YSAV           ; Check if L, H empty (no hex digits).
  beq     escape         ; Yes, generate ESC sequence.

  bit     MODE           ; Test MODE byte.
  bvc     notstor        ; B6=0 is STOR, 1 is XAM and BLOCK XAM.

  lda     L              ; LSD's of hex data.
  sta     (STL,X)        ; Store current 'store index'.
  inc     STL            ; Increment store index.
  bne     nextitem       ; Get next item (no carry).
  inc     STH            ; Add carry to 'store index' high order.
tonextitem:     
  jmp     nextitem       ; Get next command item.

run:
  jmp     (XAML)         ; Run at current XAM index.

notstor:
  bmi     xamnext        ; B7 = 0 for XAM, 1 for BLOCK XAM.

  ldx     #$02           ; Byte count.
setadr:         
  lda     L-1,X          ; Copy hex data to
  sta     STL-1,X        ;  'store index'.
  sta     XAML-1,X       ; And to 'XAM index'.
  dex                    ; Next of 2 bytes.
  bne     setadr         ; Loop unless X = 0.

nxtprnt:
  bne     prdata         ; NE means no address to print.
  lda     #$0D           ; CR.
  jsr     echo           ; Output it.
  lda     XAMH           ; 'Examine index' high-order byte.
  jsr     prbyte         ; Output it in hex format.
  lda     XAML           ; Low-order 'examine index' byte.
  jsr     prbyte         ; Output it in hex format.
  lda     #$3A           ; ":".
  jsr     echo           ; Output it.

prdata:
  lda     #$20           ; Blank.
  jsr     echo           ; Output it.
  lda     (XAML,X)       ; Get data byte at 'examine index'.
  jsr     prbyte         ; Output it in hex format.
xamnext:        
  stx     MODE           ; 0 -> MODE (XAM mode).
  lda     XAML
  cmp     L              ; Compare 'examine index' to hex data.
  lda     XAMH
  sbc     H
  bcs     tonextitem     ; Not less, so no more data to output.

  inc     XAML
  bne     mod8chk        ; Increment 'examine index'.
  inc     XAMH

mod8chk:
  lda     XAML           ; Check low-order 'examine index' byte
  and     #$07           ; For MOD 8 = 0
  bpl     nxtprnt        ; Always taken.

prbyte:
  pha                    ; Save A for LSD.
  lsr
  lsr
  lsr                    ; MSD to LSD position.
  lsr
  jsr     prhex          ; Output hex digit.
  pla                    ; Restore A.

prhex:
  and     #$0F           ; Mask LSD for hex print.
  ora     #$30           ; Add "0".
  cmp     #$3A           ; Digit?
  bcc     echo           ; Yes, output it.
  adc     #$06           ; Add offset for letter.

echo:
  pha
  sta     SC_DATA        ; Output character.
  lda     #$FF           ; Wait for transmit to complete.
txdelay:
  DEC                    ; Decrement A.
  bne     txdelay        ; Until A gets to 0.
  pla                    ; Restore A.
  rts                    ; Return.