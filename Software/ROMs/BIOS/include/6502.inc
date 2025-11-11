.setcpu "65C02"

.segment "ZEROPAGE"
READ_PTR:     .res 1
WRITE_PTR:    .res 1

.segment "STACK"
STACK: .res $100

.segment "INPUT_BUFFER"
INPUT_BUFFER: .res $100

.segment "KERNAL_VARS"

; LCD Card | $8000-$800F
LCD_PORTB       = $8000
LCD_PORTA       = $8001
LCD_DDRB        = $8002
LCD_DDRA        = $8003
LCD_T1CL        = $8004
LCD_T1CH        = $8005
LCD_T1LL        = $8006
LCD_T1LH        = $8007
LCD_T2CL        = $8008
LCD_T2CH        = $8009
LCD_SR          = $800A
LCD_ACR         = $800B
LCD_PCR         = $800C
LCD_IFR         = $800D
LCD_IER         = $800E
LCD_ORA         = $800F

LCD_E           = %10000000
LCD_RW          = %01000000
LCD_RS          = %00100000

; RAM Card | $8000-$87FF
RAM_DATA_L      = $8000     ; R/W ($8000-$83FE)
RAM_DATA_H      = $8400     ; R/W ($8400-$87FE)
RAM_BANK        = $83FF     ; All three RAM_BANK addresses address the same bank 
RAM_BANK_L      = $83FF     ; latch and can be univerally interchanged
RAM_BANK_H      = $87FF     ; RAM bank latch value ($00-$FF) sets which of 256 ~1K banks of RAM is selected in both H or L

; Emulator | $8400-$8411
EMU_DATA        = $8400     ; Write
EMU_SCRATCH     = $8401     ; R/W   (Devboard 0 only)
EMU_GPIO_DATA   = $8402     ; R/W   (Devboard 0 only)
EMU_GPIO_DDR    = $8403     ; R/W   (Devboard 0 only)
EMU_SPI_TRANS   = $8401     ; R/W   (Devboard 1 only)
EMU_SPI_CTRL    = $8402     ; Write (Devboard 1 only)
EMU_SPI_STATUS  = $8402     ; Read  (Devboard 1 only)
EMU_SPI_CLK     = $8403     ; R/W   (Devboard 1 only)
EMU_KB_DATA     = $8404     ; Read
EMU_MOUSE_X     = $8405     ; Read
EMU_MOUSE_Y     = $8406     ; Read
EMU_MOUSE_W     = $8407     ; Read
EMU_MOUSE_BTNS  = $8408     ; Read
EMU_JOY_BTNS    = $8409     ; Read
EMU_JOY_BTNS_L  = $840A     ; Read
EMU_JOY_BTNS_H  = $840B     ; Read
EMU_RTC_SEC     = $840C     ; Read
EMU_RTC_MIN     = $840D     ; Read
EMU_RTC_HR      = $840E     ; Read
EMU_RTC_DAY     = $840F     ; Read
EMU_RTC_MON     = $8410     ; Read
EMU_RTC_YR      = $8411     ; Read

; RTC Card | $8800-$881F
RTC_SEC         = $8800
RTC_MIN         = $8801
RTC_HR          = $8802
RTC_DAY         = $8803
RTC_DATE        = $8804
RTC_MON         = $8805
RTC_YR          = $8806
RTC_CENT        = $8807
RTC_ALARM_SEC   = $8808
RTC_ALARM_MIN   = $8809
RTC_ALARM_HR    = $880A
RTC_ALARM_DATE  = $880B
RTC_WD_L        = $880C
RTC_WD_H        = $880D
RTC_CTRL_A      = $880E
RTC_CRTL_B      = $880F
RTC_RAM_ADDR    = $8810
RTC_RAM_DATA    = $8813

; Storage Card | $8C00-$8C07
ST_DATA         = $8C00     ; R/W
ST_ERROR        = $8C01     ; Read
ST_FEATURE      = $8C01     ; Write
ST_SECT_CNT     = $8C02     ; R/W
ST_LBA_0        = $8C03     ; R/W
ST_LBA_1        = $8C04     ; R/W
ST_LBA_2        = $8C05     ; R/W
ST_LBA_3        = $8C06     ; R/W
ST_STATUS       = $8C07     ; Read
ST_CMD          = $8C07     ; Write

ST_ERR_AMNF     = %00000001
ST_ERR_ABRT     = %00000100
ST_ERR_IDNF     = %00010000
ST_ERR_UNC      = %01000000
ST_ERR_BBK      = %10000000
ST_STATUS_ERR   = %00000001
ST_STATUS_IDX   = %00000010
ST_STATUS_CORR  = %00000100
ST_STATUS_DRQ   = %00001000
ST_STATUS_DSC   = %00010000
ST_STATUS_DWF   = %00100000
ST_STATUS_RDY   = %01000000
ST_STATUS_BSY   = %10000000

; Serial Card | $9000-$9003
SC_DATA         = $9000     ; R/W
SC_RESET        = $9001     ; Write
SC_STATUS       = $9001     ; Read
SC_CMD          = $9002     ; R/W
SC_CTRL         = $9003     ; R/W

; GPIO Card | $9400-$940F
GPIO_PORTB      = $9400
GPIO_PORTA      = $9401
GPIO_DDRB       = $9402
GPIO_DDRA       = $9403
GPIO_T1CL       = $9404
GPIO_T1CH       = $9405
GPIO_T1LL       = $9406
GPIO_T1LH       = $9407
GPIO_T2CL       = $9408
GPIO_T2CH       = $9409
GPIO_SR         = $940A
GPIO_ACR        = $940B
GPIO_PCR        = $940C
GPIO_IFR        = $940D
GPIO_IER        = $940E
GPIO_ORA        = $940F

; Sound Card | $9800-$980F
SND_PORTB       = $9400
SND_PORTA       = $9401
SND_DDRB        = $9402
SND_DDRA        = $9403
SND_T1CL        = $9404
SND_T1CH        = $9405
SND_T1LL        = $9406
SND_T1LH        = $9407
SND_T2CL        = $9408
SND_T2CH        = $9409
SND_SR          = $940A
SND_ACR         = $940B
SND_PCR         = $940C
SND_IFR         = $940D
SND_IER         = $940E
SND_ORA         = $940F

SND_BC1H        = %11100000   ; SND_PCR CA2
SND_BC1L        = %11000000   ; SND_PCR CA2
SND_BDIRH       = %00001110   ; SND_PCR CB2
SND_BDIRL       = %00001100   ; SND_PCR CB2

; Video Card | $9C00-$9C01
VC_DATA         = $9C00       ; R/W
VC_REG          = $9C01       ; Write
VC_STATUS       = $9C01       ; Read