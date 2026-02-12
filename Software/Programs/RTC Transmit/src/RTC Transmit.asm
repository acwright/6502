.setcpu "65C02"

.include "../../../6502.inc"

.segment "CODE"

reset:
  ldx #$ff
  txs

  lda #$00
  sta SC_STATUS ; Soft reset (value not important)

  lda #%00010000  ; N-8-1, 115200 baud
  sta SC_CTRL

  lda #%00001011  ; No parity, No echo, No interrupts
  sta SC_CMD

  lda RTC_SEC
  jsr send_char
  lda RTC_MIN
  jsr send_char
  lda RTC_HR
  jsr send_char
  lda RTC_DAY
  jsr send_char
  lda RTC_DATE
  jsr send_char
  lda RTC_MON
  jsr send_char
  lda RTC_YR
  jsr send_char
  lda RTC_CENT
  jsr send_char
  lda RTC_CTRL_A
  jsr send_char
  lda RTC_CTRL_B
  jsr send_char

  lda #%10000000  ; Set transfer enable bit (start the clock)
  sta RTC_CTRL_B

loop:
  jmp loop

send_char:
  sta SC_DATA 
  pha
tx_wait:
  lda SC_STATUS
  and #%00010000  ; Check if tx buffer not empty
  beq tx_wait     ; Loop if tx buffer not empty
  pla
  rts