.setcpu "65C02"

.segment "ZEROPAGE"
.segment "STACK"
.segment "INPUT_BUFFER"
.segment "KERNAL_VARS"
.segment "USER_VARS"
.segment "CODE"

RTC_SECS    = $8800
RTC_MINS    = $8801
RTC_HRS     = $8802
RTC_DAYS    = $8803
RTC_DATE    = $8804
RTC_MON     = $8805
RTC_YR      = $8806
RTC_CEN     = $8807
RTC_CTRLA   = $880E
RTC_CTRLB   = $880F

ACIA_DATA   = $9000
ACIA_STATUS = $9001
ACIA_CMD    = $9002
ACIA_CTRL   = $9003

reset:
  ldx #$ff
  txs

  lda #$00
  sta ACIA_STATUS ; Soft reset (value not important)

  lda #%00010000  ; N-8-1, 115200 baud
  sta ACIA_CTRL

  lda #%00001011  ; No parity, No echo, No interrupts
  sta ACIA_CMD

  lda RTC_SECS
  jsr send_char
  lda RTC_MINS
  jsr send_char
  lda RTC_HRS
  jsr send_char
  lda RTC_DAYS
  jsr send_char
  lda RTC_DATE
  jsr send_char
  lda RTC_MON
  jsr send_char
  lda RTC_YR
  jsr send_char
  lda RTC_CEN
  jsr send_char
  lda RTC_CTRLA
  jsr send_char
  lda RTC_CTRLB
  jsr send_char

  lda #%10000000  ; Set transfer enable bit (start the clock)
  sta RTC_CTRLB

loop:
  jmp loop

send_char:
  sta ACIA_DATA 
  pha
tx_wait:
  lda ACIA_STATUS
  and #%00010000  ; Check if tx buffer not empty
  beq tx_wait     ; Loop if tx buffer not empty
  pla
  rts
