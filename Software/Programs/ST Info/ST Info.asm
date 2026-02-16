.setcpu "65C02"

.include "../../6502.inc"

.segment "CODE"

ST_INFO_PTR     := $00
ST_BUFFER_PTR   := $02
ST_INFO         := $400
ST_BUFFER       := $500

reset:
  ldx #$ff
  txs

  lda #<ST_BUFFER
  sta ST_BUFFER_PTR
  lda #>ST_BUFFER
  sta ST_BUFFER_PTR + 1

  lda #<ST_INFO
  sta ST_INFO_PTR
  lda #>ST_INFO
  sta ST_INFO_PTR + 1

  jsr acia_init

  jsr cf_init
  jsr cf_info
  jsr cf_read_sector

  ldx #0
  jsr print

loop:
  jmp loop

cf_init:
  jsr cf_wait_rdy
  lda #$04                  ; Reset command
  sta ST_CMD
  jsr cf_wait_rdy
  lda #$00
  sta ST_LBA_0
  sta ST_LBA_1
  sta ST_LBA_2
  lda #$E0                  ; LBA3=0, Master, Mode=LBA
  sta ST_LBA_3
  lda #$01                  ; 8-bit transfers
  sta ST_FEATURE
  lda #$EF                  ; Set feature
  sta ST_CMD
  jsr cf_wait_rdy
  jsr cf_err
  rts

cf_info:
  lda #$EC                  ; Drive ID command
  sta ST_CMD
  jsr cf_wait_dat
  jsr cf_err
  
  ldy #0
  ldx #2                    ; Read 2 pages (512 bytes)
cf_info_loop:
  lda ST_DATA
  sta (ST_INFO_PTR),y
  iny
  bne cf_info_loop
  inc ST_INFO_PTR + 1       ; Increment high byte of pointer
  dex
  bne cf_info_loop
  dec ST_INFO_PTR + 1       ; Restore original pointer
  dec ST_INFO_PTR + 1
cf_info_exit:
  rts

cf_read_sector:
  lda #1                    ; Read 1 sector
  sta ST_SECT_CNT
  jsr cf_wait_rdy
  lda #$20                  ; Read sector command
  sta ST_CMD
  jsr cf_wait_dat
  jsr cf_err

  ldy #0
  ldx #2                    ; Read 2 pages (512 bytes) from sector 0
cf_read_sector_loop:
  lda ST_DATA
  sta (ST_BUFFER_PTR),y
  iny
  bne cf_read_sector_loop
  inc ST_BUFFER_PTR + 1     ; Increment high byte of pointer
  dex
  bne cf_read_sector_loop
  dec ST_BUFFER_PTR + 1     ; Restore original pointer
  dec ST_BUFFER_PTR + 1
cf_read_sector_exit:
  rts

cf_wait_bsy:
  lda ST_STATUS
  and #%10000000
  bne cf_wait_bsy
cf_wait_bsy_exit:
  rts

cf_wait_rdy:
  lda ST_STATUS
  and #%11000000            ; Mask out BSY and RDY flags
  cmp #%01000000
  bne cf_wait_rdy
cf_wait_rdy_exit:
  rts

cf_wait_dat:
  lda ST_STATUS
  and #%11001000            ; Mask out BSY, RDY and DRQ flags
  cmp #%01001000
  bne cf_wait_dat
cf_wait_dat_exit:
  rts

cf_err:
  lda ST_STATUS
  and #%00000001            ; Mask out error flag
  beq cf_no_err
  lda #'!'
  jsr acia_send_char
  lda ST_ERROR                ; Get error code
  jsr acia_send_char
cf_no_err:
  rts

print:
  lda success,x
  beq print_crlf
  jsr acia_send_char
  inx
  jmp print
print_crlf:
  lda #$0D                   ; Carriage Return
  jsr acia_send_char
  lda #$0A                   ; Line feed
  jsr acia_send_char
  rts

acia_init:
  lda #$00
  sta SC_STATUS             ; Soft reset (value not important)
  lda #%00010000              ; N-8-1, 115200 baud
  sta SC_CTRL
  lda #%00001011              ; No parity, No echo, No interrupts
  sta SC_CMD
  rts

acia_send_char:
  sta SC_DATA
  pha
acia_tx_wait:
  lda SC_STATUS
  and #%00010000              ; Check if tx buffer not empty
  beq acia_tx_wait            ; Loop if tx buffer not empty
  pla
  rts

success:     .asciiz "Success!"