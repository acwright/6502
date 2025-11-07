.setcpu "65C02"

.segment "ZEROPAGE"
CF_INFO_PTR:  .res 2
CF_BUFFER_PTR:  .res 2
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
CF_INFO:        .res 256
CF_BUFFER:      .res 512
.segment "CODE"

CF_DATA     = $8C00   ; Data port
CF_ERR      = $8C01   ; Read: Error Code | Write: Feature
CF_FEAT     = $8C01   ; Read: Error Code | Write: Feature
CF_SECT     = $8C02   ; Number of sectors to transfer
CF_LBA0     = $8C03   ; Sector Address LBA 0 [0:7]
CF_LBA1     = $8C04   ; Sector Address LBA 1 [8:15]
CF_LBA2     = $8C05   ; Sector Address LBA 2 [16:23]
CF_LBA3     = $8C06   ; Sector Address LBA 3 [24:27 (LSB)]
CF_STAT     = $8C07   ; Read: Status | Write: Command
CF_CMD      = $8C07   ; Read: Status | Write: Command

ACIA_DATA   = $9000
ACIA_STATUS = $9001
ACIA_CMD    = $9002
ACIA_CTRL   = $9003

reset:
  ldx #$ff
  txs

  lda #<CF_BUFFER
  sta CF_BUFFER_PTR
  lda #>CF_BUFFER
  sta CF_BUFFER_PTR + 1

  lda #<CF_INFO
  sta CF_INFO_PTR
  lda #>CF_INFO
  sta CF_INFO_PTR + 1

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
  sta CF_CMD
  jsr cf_wait_rdy
  lda #$00
  sta CF_LBA0
  sta CF_LBA1
  sta CF_LBA2
  lda #$E0                  ; LBA3=0, Master, Mode=LBA
  sta CF_LBA3
  lda #$01                  ; 8-bit transfers
  sta CF_FEAT
  lda #$EF                  ; Set feature
  sta CF_CMD
  jsr cf_wait_rdy
  jsr cf_err
  rts

cf_info:
  lda #$EC                  ; Drive ID command
  sta CF_CMD
  jsr cf_wait_dat
  jsr cf_err
  
  ldy #0
  ldx #2                    ; Read 2 pages (512 bytes)
cf_info_loop:
  lda CF_DATA
  sta (CF_INFO_PTR),y
  iny
  bne cf_info_loop
  inc CF_INFO_PTR + 1       ; Increment high byte of pointer
  dex
  bne cf_info_loop
  dec CF_INFO_PTR + 1       ; Restore original pointer
  dec CF_INFO_PTR + 1
cf_info_exit:
  rts

cf_read_sector:
  lda #1                    ; Read 1 sector
  sta CF_SECT
  jsr cf_wait_rdy
  lda #$20                  ; Read sector command
  sta CF_CMD
  jsr cf_wait_dat
  jsr cf_err

  ldy #0
  ldx #2                    ; Read 2 pages (512 bytes) from sector 0
cf_read_sector_loop:
  lda CF_DATA
  sta (CF_BUFFER_PTR),y
  iny
  bne cf_read_sector_loop
  inc CF_BUFFER_PTR + 1     ; Increment high byte of pointer
  dex
  bne cf_read_sector_loop
  dec CF_BUFFER_PTR + 1     ; Restore original pointer
  dec CF_BUFFER_PTR + 1
cf_read_sector_exit:
  rts

cf_wait_bsy:
  lda CF_STAT
  and #%10000000
  bne cf_wait_bsy
cf_wait_bsy_exit:
  rts

cf_wait_rdy:
  lda CF_STAT
  and #%11000000            ; Mask out BSY and RDY flags
  cmp #%01000000
  bne cf_wait_rdy
cf_wait_rdy_exit:
  rts

cf_wait_dat:
  lda CF_STAT
  and #%11001000            ; Mask out BSY, RDY and DRQ flags
  cmp #%01001000
  bne cf_wait_dat
cf_wait_dat_exit:
  rts

cf_err:
  lda CF_STAT
  and #%00000001            ; Mask out error flag
  beq cf_no_err
  lda #'!'
  jsr acia_send_char
  lda CF_ERR                ; Get error code
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
  sta ACIA_STATUS             ; Soft reset (value not important)
  lda #%00011111              ; N-8-1, 19200 baud
  sta ACIA_CTRL
  lda #%00001011              ; No parity, No echo, No interrupts
  sta ACIA_CMD
  rts

acia_send_char:
  sta ACIA_DATA
  pha
acia_tx_wait:
  lda ACIA_STATUS
  and #%00010000              ; Check if tx buffer not empty
  beq acia_tx_wait            ; Loop if tx buffer not empty
  pla
  rts

success:     .asciiz "Success!"

.segment "VECTORS"

  .word reset
  .word reset
  .word reset