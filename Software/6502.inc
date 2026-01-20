; ***                     ***
; ***   6502 INCLUDES     ***
; ***                     ***

READ_PTR            = ZP
WRITE_PTR           = ZP + 1

IRQ_PTR             = KERNAL_VARS       ; $0300
NMI_PTR             = KERNAL_VARS + 2   ; $0302

; RAM Card | IO 1 & 2 | $8000-$87FF 
RAM_DATA            = $8000             ; R/W ($8000-$83FE)
RAM_DATA_H          = $8400             ; R/W ($8400-$87FE)
RAM_BANK            = $83FF             ; All three RAM_BANK addresses address the same bank switching
RAM_BANK_L          = $83FF             ; latch and can be univerally interchanged
RAM_BANK_H          = $87FF             ; RAM bank latch value ($00-$FF) sets which of 256 ~1K banks of RAM is selected in both H or L

; Emulator | IO 2 | $8400-$840F 
EMU_SER_DATA        = $8400             ; R/W
EMU_SER_CTRL        = $8401             ; Write
EMU_SER_STATUS      = $8401             ; Read
EMU_KB_CMD          = $8402             ; Write
EMU_KB_DATA         = $8402             ; Read
EMU_MOUSE_X         = $8403             ; Read
EMU_MOUSE_Y         = $8404             ; Read
EMU_MOUSE_W         = $8405             ; Read
EMU_MOUSE_BTNS      = $8406             ; Read
EMU_JOY_BTNS        = $8407             ; Read
EMU_RTC_SEC         = $8408             ; Read
EMU_RTC_MIN         = $8409             ; Read
EMU_RTC_HR          = $840A             ; Read
EMU_RTC_DAY         = $840B             ; Read
EMU_RTC_MON         = $840C             ; Read
EMU_RTC_YR          = $840D             ; Read
EMU_PRAM_DATA       = $840E             ; R/W
EMU_PRAM_ADDR       = $840F             ; R/W
EMU_RAM             = $8600             ; R/W
EMU_RAM_BANK        = $87FF             ; Write

; RTC Card | IO 3 | $8800-$881F
RTC_SEC             = $8800
RTC_MIN             = $8801
RTC_HR              = $8802
RTC_DAY             = $8803
RTC_DATE            = $8804
RTC_MON             = $8805
RTC_YR              = $8806
RTC_CENT            = $8807
RTC_ALARM_SEC       = $8808
RTC_ALARM_MIN       = $8809
RTC_ALARM_HR        = $880A
RTC_ALARM_DATE      = $880B
RTC_WD_L            = $880C
RTC_WD_H            = $880D
RTC_CTRL_A          = $880E
RTC_CTRL_B          = $880F
RTC_RAM_ADDR        = $8810
RTC_RAM_DATA        = $8813

; Storage Card | IO 4 | $8C00-$8C07
ST_DATA             = $8C00             ; R/W
ST_ERROR            = $8C01             ; Read
ST_FEATURE          = $8C01             ; Write
ST_SECT_CNT         = $8C02             ; R/W
ST_LBA_0            = $8C03             ; R/W
ST_LBA_1            = $8C04             ; R/W
ST_LBA_2            = $8C05             ; R/W
ST_LBA_3            = $8C06             ; R/W
ST_STATUS           = $8C07             ; Read
ST_CMD              = $8C07             ; Write
ST_ERR_AMNF         = %00000001
ST_ERR_ABRT         = %00000100
ST_ERR_IDNF         = %00010000
ST_ERR_UNC          = %01000000
ST_ERR_BBK          = %10000000
ST_STATUS_ERR       = %00000001
ST_STATUS_IDX       = %00000010
ST_STATUS_CORR      = %00000100
ST_STATUS_DRQ       = %00001000
ST_STATUS_DSC       = %00010000
ST_STATUS_DWF       = %00100000
ST_STATUS_RDY       = %01000000
ST_STATUS_BSY       = %10000000

; Serial Card | IO 5 | $9000-$9003
SC_DATA             = $9000             ; R/W
SC_RESET            = $9001             ; Write
SC_STATUS           = $9001             ; Read
SC_CMD              = $9002             ; R/W
SC_CTRL             = $9003             ; R/W

; GPIO Card / Input Board | IO 6 | $9400-$940F
GPIO_PORTB          = $9400
GPIO_PORTA          = $9401
GPIO_DDRB           = $9402
GPIO_DDRA           = $9403
GPIO_T1CL           = $9404
GPIO_T1CH           = $9405
GPIO_T1LL           = $9406
GPIO_T1LH           = $9407
GPIO_T2CL           = $9408
GPIO_T2CH           = $9409
GPIO_SR             = $940A
GPIO_ACR            = $940B
GPIO_PCR            = $940C
GPIO_IFR            = $940D
GPIO_IER            = $940E
GPIO_ORA            = $940F

; Sound Card (AY-3-8912) | IO 7 | $9800-$980F
SND_AY_PORTB           = $9800
SND_AY_PORTA           = $9801
SND_AY_DDRB            = $9802
SND_AY_DDRA            = $9803
SND_AY_T1CL            = $9804
SND_AY_T1CH            = $9805
SND_AY_T1LL            = $9806
SND_AY_T1LH            = $9807
SND_AY_T2CL            = $9808
SND_AY_T2CH            = $9809
SND_AY_SR              = $980A
SND_AY_ACR             = $980B
SND_AY_PCR             = $980C
SND_AY_IFR             = $980D
SND_AY_IER             = $980E
SND_AY_ORA             = $980F
SND_AY_BC1H            = %11100000      ; SND_PCR CA2
SND_AY_BC1L            = %11000000      ; SND_PCR CA2
SND_AY_BDIRH           = %00001110      ; SND_PCR CB2
SND_AY_BDIRL           = %00001100      ; SND_PCR CB2

; Video Card / VGA Card | IO 8 | $9C00-$9C01
VC_DATA             = $9C00             ; R/W
VC_REG              = $9C01             ; Write
VC_STATUS           = $9C01             ; Read

; VERA | IO 8 | $9C00-$9C0F
VERA_ADDR_LOW       = $9C00
VERA_ADDR_MID       = $9C01
VERA_ADDR_HIGH      = $9C02
VERA_DATA0          = $9C03
VERA_DATA1          = $9C04
VERA_CTRL           = $9C05
VERA_DC_VIDEO       = $9C09
VERA_DC_HSCALE      = $9C0A
VERA_DC_VSCALE      = $9C0B
VERA_DC_BORDER      = $9C0B
VERA_L0_CONFIG      = $9C0D
VERA_L0_MAPBASE     = $9C0E
VERA_L0_TILEBASE    = $9C0F

; LCD Card | IO 1 | $9C00-$9C0F
LCD_PORTB           = $9C00
LCD_PORTA           = $9C01
LCD_DDRB            = $9C02
LCD_DDRA            = $9C03
LCD_T1CL            = $9C04
LCD_T1CH            = $9C05
LCD_T1LL            = $9C06
LCD_T1LH            = $9C07
LCD_T2CL            = $9C08
LCD_T2CH            = $9C09
LCD_SR              = $9C0A
LCD_ACR             = $9C0B
LCD_PCR             = $9C0C
LCD_IFR             = $9C0D
LCD_IER             = $9C0E
LCD_ORA             = $9C0F
LCD_E               = %10000000
LCD_RW              = %01000000
LCD_RS              = %00100000