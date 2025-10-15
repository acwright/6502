.setcpu "65C02"
.segment "CODE"

XAML  = $24                            ; Last "opened" location Low
XAMH  = $25                            ; Last "opened" location High
STL   = $26                            ; Store address Low
STH   = $27                            ; Store address High
L     = $28                            ; Hex value parsing Low
H     = $29                            ; Hex value parsing High
YSAV  = $2A                            ; Used to see if hex value is given
MODE  = $2B                            ; $00=XAM, $7F=STOR, $AE=BLOCK XAM

IN    = $0200                          ; Input buffer

ACIA_DATA   = $9000
ACIA_STATUS = $9001
ACIA_CMD    = $9002
ACIA_CTRL   = $9003

RESET:
  lda     #$1F           ; 8-N-1, 19200 baud.
  sta     ACIA_CTRL
  ldy     #$8B           ; No parity, no echo, no interrupts.
  sty     ACIA_CMD
  lda     #$1B           ; Begin with escape.

NOTCR:
  cmp     #$08           ; Backspace key?
  beq     BACKSPACE      ; Yes.
  cmp     #$1B           ; ESC?
  beq     ESCAPE         ; Yes.
  iny                    ; Advance text index.
  bpl     NEXTCHAR       ; Auto ESC if line longer than 127.

ESCAPE:
  lda     #$5C           ; "\".
  jsr     ECHO           ; Output it.

GETLINE:
  lda     #$0D           ; Send CR
  jsr     ECHO

  ldy     #$01           ; Initialize text index.
BACKSPACE:      
  dey                    ; Back up text index.
  bmi     GETLINE        ; Beyond start of line, reinitialize.

NEXTCHAR:
  lda     ACIA_STATUS    ; Check status.
  and     #$08           ; Key ready?
  beq     NEXTCHAR       ; Loop until ready.
  lda     ACIA_DATA      ; Load character. B7 will be '0'.
  sta     IN,Y           ; Add to text buffer.
  jsr     ECHO           ; Display character.
  cmp     #$0D           ; CR?
  bne     NOTCR          ; No.

  ldy     #$FF           ; Reset text index.
  lda     #$00           ; For XAM mode.
  tax                    ; X=0.
SETBLOCK:
  asl
SETSTOR:
  asl                    ; Leaves $7B if setting STOR mode.
  sta     MODE           ; $00 = XAM, $74 = STOR, $B8 = BLOK XAM.
BLSKIP:
  iny                    ; Advance text index.
NEXTITEM:
  lda     IN,Y           ; Get character.
  cmp     #$0D           ; CR?
  beq     GETLINE        ; Yes, done this line.
  cmp     #$2E           ; "."?
  bcc     BLSKIP         ; Skip delimiter.
  beq     SETBLOCK       ; Set BLOCK XAM mode.
  cmp     #$3A           ; ":"?
  beq     SETSTOR        ; Yes, set STOR mode.
  cmp     #$52           ; "R"?
  beq     RUN            ; Yes, run user program.
  stx     L              ; $00 -> L.
  stx     H              ;    and H.
  sty     YSAV           ; Save Y for comparison

NEXTHEX:
  lda     IN,Y           ; Get character for hex test.
  eor     #$30           ; Map digits to $0-9.
  cmp     #$0A           ; Digit?
  bcc     DIG            ; Yes.
  ADC     #$88           ; Map letter "A"-"F" to $FA-FF.
  cmp     #$FA           ; Hex letter?
  bcc     NOTHEX         ; No, character not hex.
DIG:
  asl
  asl                    ; Hex digit to MSD of A.
  asl
  asl

  ldx     #$04           ; Shift count.
HEXSHIFT:
  asl                    ; Hex digit left, MSB to carry.
  rol     L              ; Rotate into LSD.
  rol     H              ; Rotate into MSD's.
  dex                    ; Done 4 shifts?
  bne     HEXSHIFT       ; No, loop.
  iny                    ; Advance text index.
  bne     NEXTHEX        ; Always taken. Check next character for hex.

NOTHEX:
  cpy     YSAV           ; Check if L, H empty (no hex digits).
  beq     ESCAPE         ; Yes, generate ESC sequence.

  bit     MODE           ; Test MODE byte.
  bvc     NOTSTOR        ; B6=0 is STOR, 1 is XAM and BLOCK XAM.

  lda     L              ; LSD's of hex data.
  sta     (STL,X)        ; Store current 'store index'.
  inc     STL            ; Increment store index.
  bne     NEXTITEM       ; Get next item (no carry).
  inc     STH            ; Add carry to 'store index' high order.
TONEXTITEM:     
  jmp     NEXTITEM       ; Get next command item.

RUN:
  jmp     (XAML)         ; Run at current XAM index.

NOTSTOR:
  bmi     XAMNEXT        ; B7 = 0 for XAM, 1 for BLOCK XAM.

  ldx     #$02           ; Byte count.
SETADR:         
  lda     L-1,X          ; Copy hex data to
  sta     STL-1,X        ;  'store index'.
  sta     XAML-1,X       ; And to 'XAM index'.
  dex                    ; Next of 2 bytes.
  bne     SETADR         ; Loop unless X = 0.

NXTPRNT:
  bne     PRDATA         ; NE means no address to print.
  lda     #$0D           ; CR.
  jsr     ECHO           ; Output it.
  lda     XAMH           ; 'Examine index' high-order byte.
  jsr     PRBYTE         ; Output it in hex format.
  lda     XAML           ; Low-order 'examine index' byte.
  jsr     PRBYTE         ; Output it in hex format.
  lda     #$3A           ; ":".
  jsr     ECHO           ; Output it.

PRDATA:
  lda     #$20           ; Blank.
  jsr     ECHO           ; Output it.
  lda     (XAML,X)       ; Get data byte at 'examine index'.
  jsr     PRBYTE         ; Output it in hex format.
XAMNEXT:        
  stx     MODE           ; 0 -> MODE (XAM mode).
  lda     XAML
  cmp     L              ; Compare 'examine index' to hex data.
  lda     XAMH
  sbc     H
  bcs     TONEXTITEM     ; Not less, so no more data to output.

  inc     XAML
  bne     MOD8CHK        ; Increment 'examine index'.
  inc     XAMH

MOD8CHK:
  lda     XAML           ; Check low-order 'examine index' byte
  and     #$07           ; For MOD 8 = 0
  bpl     NXTPRNT        ; Always taken.

PRBYTE:
  pha                    ; Save A for LSD.
  lsr
  lsr
  lsr                    ; MSD to LSD position.
  lsr
  jsr     PRHEX          ; Output hex digit.
  pla                    ; Restore A.

PRHEX:
  and     #$0F           ; Mask LSD for hex print.
  ora     #$30           ; Add "0".
  cmp     #$3A           ; Digit?
  bcc     ECHO           ; Yes, output it.
  adc     #$06           ; Add offset for letter.

ECHO:
  pha
  sta     ACIA_DATA      ; Output character.
  lda     #$FF           ; Wait for transmit to complete.
TXDELAY:
  DEC                    ; Decrement A.
  bne     TXDELAY        ; Until A gets to 0.
  pla                    ; Restore A.
  rts                    ; Return.

.segment "VECTORS"

  .word   $0F00          ; NMI vector
  .word   RESET          ; RESET vector
  .word   $0000          ; IRQ vector