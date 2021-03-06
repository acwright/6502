EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "6502 Memory Card"
Date "2021-03-27"
Rev "1"
Comp "A.C. Wright Design"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Memory_Card:6502CARDEDGE CONN1
U 1 1 60621DAE
P 2050 2950
F 0 "CONN1" H 2050 4100 59  0000 C CNN
F 1 "6502CARDEDGE" H 2050 4000 50  0000 C CNN
F 2 "Memory Card:6502CARDEDGE" H 2050 2950 50  0001 C CNN
F 3 "" H 2050 2950 50  0001 C CNN
	1    2050 2950
	1    0    0    -1  
$EndComp
$Comp
L Memory_Card:AS6C62256 IC1
U 1 1 60623898
P 3750 2150
F 0 "IC1" H 4250 2436 59  0000 C CNN
F 1 "AS6C62256" H 4250 2331 59  0000 C CNN
F 2 "Package_DIP:DIP-28_W15.24mm_Socket_LongPads" H 3750 2150 50  0001 C CNN
F 3 "" H 3750 2150 50  0001 C CNN
	1    3750 2150
	1    0    0    -1  
$EndComp
$Comp
L Memory_Card:AT28C256 IC2
U 1 1 606279C8
P 5750 2150
F 0 "IC2" H 6300 2436 59  0000 C CNN
F 1 "AT28C256" H 6300 2331 59  0000 C CNN
F 2 "Package_DIP:DIP-28_W15.24mm_Socket_LongPads" H 5750 2150 50  0001 C CNN
F 3 "" H 5750 2150 50  0001 C CNN
	1    5750 2150
	1    0    0    -1  
$EndComp
Wire Bus Line
	900  4350 3200 4350
Connection ~ 5300 4350
Wire Bus Line
	5300 4350 7400 4350
Connection ~ 3200 4350
Wire Bus Line
	3200 4350 5300 4350
Entry Wire Line
	900  1950 1000 2050
Entry Wire Line
	900  2550 1000 2650
Entry Wire Line
	900  2750 1000 2850
Entry Wire Line
	900  3050 1000 3150
Entry Wire Line
	900  3150 1000 3250
Entry Wire Line
	900  3250 1000 3350
Entry Wire Line
	900  3350 1000 3450
Entry Wire Line
	900  3450 1000 3550
Entry Wire Line
	900  3550 1000 3650
Entry Wire Line
	900  3650 1000 3750
Entry Wire Line
	900  3750 1000 3850
Entry Wire Line
	900  3850 1000 3950
Entry Wire Line
	3200 2050 3100 2150
Entry Wire Line
	3200 2150 3100 2250
Entry Wire Line
	3200 2250 3100 2350
Entry Wire Line
	3200 2350 3100 2450
Entry Wire Line
	3200 2450 3100 2550
Entry Wire Line
	3200 2550 3100 2650
Entry Wire Line
	3200 2650 3100 2750
Entry Wire Line
	3200 2750 3100 2850
Entry Wire Line
	3200 2850 3100 2950
Entry Wire Line
	3200 2950 3100 3050
Entry Wire Line
	3200 3050 3100 3150
Entry Wire Line
	3200 3150 3100 3250
Entry Wire Line
	3200 3250 3100 3350
Entry Wire Line
	3200 3350 3100 3450
Entry Wire Line
	3200 3450 3100 3550
Entry Wire Line
	3200 3550 3100 3650
Entry Wire Line
	3200 3650 3100 3750
Entry Wire Line
	3200 3750 3100 3850
Entry Wire Line
	3200 2050 3300 2150
Entry Wire Line
	3200 2150 3300 2250
Entry Wire Line
	3200 2250 3300 2350
Entry Wire Line
	3200 2350 3300 2450
Entry Wire Line
	3200 2450 3300 2550
Entry Wire Line
	3200 2550 3300 2650
Entry Wire Line
	3200 2650 3300 2750
Entry Wire Line
	3200 2750 3300 2850
Entry Wire Line
	3200 2850 3300 2950
Entry Wire Line
	3200 2950 3300 3050
Entry Wire Line
	3200 3050 3300 3150
Entry Wire Line
	3200 3150 3300 3250
Entry Wire Line
	3200 3250 3300 3350
Entry Wire Line
	3200 3350 3300 3450
Entry Wire Line
	5300 2050 5200 2150
Entry Wire Line
	5300 2250 5200 2350
Entry Wire Line
	5300 2350 5200 2450
Entry Wire Line
	5300 2450 5200 2550
Entry Wire Line
	5300 2550 5200 2650
Entry Wire Line
	5300 2750 5200 2850
Entry Wire Line
	5300 2950 5200 3050
Entry Wire Line
	5300 3050 5200 3150
Entry Wire Line
	5300 3150 5200 3250
Entry Wire Line
	5300 3250 5200 3350
Entry Wire Line
	5300 3350 5200 3450
Entry Wire Line
	5300 2050 5400 2150
Entry Wire Line
	5300 2150 5400 2250
Entry Wire Line
	5300 2250 5400 2350
Entry Wire Line
	5300 2350 5400 2450
Entry Wire Line
	5300 2450 5400 2550
Entry Wire Line
	5300 2550 5400 2650
Entry Wire Line
	5300 2650 5400 2750
Entry Wire Line
	5300 2750 5400 2850
Entry Wire Line
	5300 2850 5400 2950
Entry Wire Line
	5300 2950 5400 3050
Entry Wire Line
	5300 3050 5400 3150
Entry Wire Line
	5300 3150 5400 3250
Entry Wire Line
	5300 3250 5400 3350
Entry Wire Line
	5300 3350 5400 3450
Entry Wire Line
	7400 2050 7300 2150
Entry Wire Line
	7400 2250 7300 2350
Entry Wire Line
	7400 2350 7300 2450
Entry Wire Line
	7400 2450 7300 2550
Entry Wire Line
	7400 2550 7300 2650
Entry Wire Line
	7400 2750 7300 2850
Entry Wire Line
	7400 2950 7300 3050
Entry Wire Line
	7400 3050 7300 3150
Entry Wire Line
	7400 3150 7300 3250
Entry Wire Line
	7400 3250 7300 3350
Entry Wire Line
	7400 3350 7300 3450
Text Notes 5950 3650 0    50   ~ 0
ROM
Text Notes 3950 3650 0    50   ~ 0
RAM
Wire Wire Line
	4750 2150 4850 2150
Wire Wire Line
	4750 2350 5200 2350
Wire Wire Line
	5200 2450 4750 2450
Wire Wire Line
	4750 2550 5200 2550
Wire Wire Line
	5200 2650 4750 2650
Wire Wire Line
	5200 2850 4750 2850
Wire Wire Line
	4750 3050 5200 3050
Wire Wire Line
	4750 3150 5200 3150
Wire Wire Line
	4750 3250 5200 3250
Wire Wire Line
	5200 3350 4750 3350
Wire Wire Line
	5200 3450 4750 3450
Wire Wire Line
	3750 2150 3300 2150
Wire Wire Line
	3300 2250 3750 2250
Wire Wire Line
	3300 2350 3750 2350
Wire Wire Line
	3750 2450 3300 2450
Wire Wire Line
	3300 2550 3750 2550
Wire Wire Line
	3300 2650 3750 2650
Wire Wire Line
	3750 2750 3300 2750
Wire Wire Line
	3300 2850 3750 2850
Wire Wire Line
	3750 2950 3300 2950
Wire Wire Line
	3300 3050 3750 3050
Wire Wire Line
	3750 3150 3300 3150
Wire Wire Line
	3300 3250 3750 3250
Wire Wire Line
	3300 3350 3750 3350
Wire Wire Line
	3750 3450 3600 3450
Wire Wire Line
	5400 2150 5750 2150
Wire Wire Line
	5750 2250 5400 2250
Wire Wire Line
	5400 2350 5750 2350
Wire Wire Line
	5750 2450 5400 2450
Wire Wire Line
	5400 2550 5750 2550
Wire Wire Line
	5750 2650 5400 2650
Wire Wire Line
	5400 2750 5750 2750
Wire Wire Line
	5750 2850 5400 2850
Wire Wire Line
	5400 2950 5750 2950
Wire Wire Line
	5750 3050 5400 3050
Wire Wire Line
	5400 3150 5750 3150
Wire Wire Line
	5750 3250 5400 3250
Wire Wire Line
	5400 3350 5750 3350
Wire Wire Line
	5750 3450 5650 3450
Wire Wire Line
	6850 2150 6950 2150
Wire Wire Line
	7300 2350 6850 2350
Wire Wire Line
	6850 2450 7300 2450
Wire Wire Line
	7300 2550 6850 2550
Wire Wire Line
	6850 2650 7300 2650
Wire Wire Line
	6850 2850 7300 2850
Wire Wire Line
	7300 3050 6850 3050
Wire Wire Line
	6850 3150 7300 3150
Wire Wire Line
	7300 3250 6850 3250
Wire Wire Line
	6850 3350 7300 3350
Wire Wire Line
	7300 3450 6850 3450
Wire Wire Line
	2650 2150 3100 2150
Wire Wire Line
	3100 2250 2650 2250
Wire Wire Line
	2650 2350 3100 2350
Wire Wire Line
	3100 2450 2650 2450
Wire Wire Line
	2650 2550 3100 2550
Wire Wire Line
	3100 2650 2650 2650
Wire Wire Line
	2650 2750 3100 2750
Wire Wire Line
	3100 2850 2650 2850
Wire Wire Line
	2650 2950 3100 2950
Wire Wire Line
	2650 3050 3100 3050
Wire Wire Line
	3100 3150 2650 3150
Wire Wire Line
	2650 3250 3100 3250
Wire Wire Line
	3100 3350 2650 3350
Wire Wire Line
	2650 3450 3100 3450
Wire Wire Line
	3100 3550 2650 3550
Wire Wire Line
	2650 3650 3100 3650
Wire Wire Line
	2650 3750 3100 3750
Wire Wire Line
	3100 3850 2650 3850
Wire Wire Line
	1000 2050 1450 2050
Wire Wire Line
	1000 2650 1450 2650
Wire Wire Line
	1000 2850 1450 2850
Wire Wire Line
	1000 3150 1450 3150
Wire Wire Line
	1450 3250 1000 3250
Wire Wire Line
	1000 3350 1450 3350
Wire Wire Line
	1450 3450 1000 3450
Wire Wire Line
	1000 3550 1450 3550
Wire Wire Line
	1450 3650 1000 3650
Wire Wire Line
	1000 3750 1450 3750
Wire Wire Line
	1450 3850 1000 3850
Wire Wire Line
	1000 3950 1450 3950
Wire Wire Line
	6850 2250 6900 2250
Wire Wire Line
	6900 2250 6900 1700
$Comp
L Device:R R1
U 1 1 60709C30
P 6900 1400
F 0 "R1" H 6750 1450 50  0000 L CNN
F 1 "1k" H 6750 1350 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 6830 1400 50  0001 C CNN
F 3 "~" H 6900 1400 50  0001 C CNN
F 4 "C17513" H 6900 1400 50  0001 C CNN "LCSC"
	1    6900 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 1250 6900 1150
Text GLabel 6900 1150 1    50   Input ~ 0
VCC
Wire Wire Line
	6850 2750 7050 2750
Wire Wire Line
	7050 2750 7050 1150
Wire Wire Line
	7200 2950 7200 1150
Wire Wire Line
	6850 2950 7200 2950
Text GLabel 7050 1150 1    50   Input ~ 0
~READ
Text GLabel 7200 1150 1    50   Input ~ 0
~ROM_ENABLE
Wire Wire Line
	4750 2250 4800 2250
Wire Wire Line
	4800 2250 4800 1150
Wire Wire Line
	4750 2750 4950 2750
Wire Wire Line
	4950 2750 4950 1150
Wire Wire Line
	4750 2950 5100 2950
Wire Wire Line
	5100 2950 5100 1150
Text GLabel 5100 1150 1    50   Input ~ 0
A15
Text GLabel 4950 1150 1    50   Input ~ 0
~READ
Text GLabel 4800 1150 1    50   Input ~ 0
~WRITE
$Comp
L Connector_Generic:Conn_02x01 J1
U 1 1 607827E7
P 6250 1700
F 0 "J1" H 6300 1825 50  0000 C CNN
F 1 "Conn_02x01" H 6300 1826 50  0001 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 6250 1700 50  0001 C CNN
F 3 "~" H 6250 1700 50  0001 C CNN
	1    6250 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	6550 1700 6600 1700
Connection ~ 6900 1700
Wire Wire Line
	6900 1700 6900 1550
Wire Wire Line
	6050 1700 6000 1700
Text GLabel 5850 1700 0    50   Input ~ 0
~WRITE
$Comp
L Jumper:SolderJumper_2_Open JP1
U 1 1 607BDABB
P 6300 1400
F 0 "JP1" H 6300 1513 50  0000 C CNN
F 1 "SolderJumper_2_Open" H 6300 1514 50  0001 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_RoundedPad1.0x1.5mm" H 6300 1400 50  0001 C CNN
F 3 "~" H 6300 1400 50  0001 C CNN
	1    6300 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 1400 6600 1400
Wire Wire Line
	6600 1400 6600 1700
Connection ~ 6600 1700
Wire Wire Line
	6600 1700 6900 1700
Wire Wire Line
	6150 1400 6000 1400
Wire Wire Line
	6000 1400 6000 1700
Connection ~ 6000 1700
Wire Wire Line
	6000 1700 5850 1700
Text Label 1050 2050 0    50   ~ 0
GND
NoConn ~ 1450 2150
NoConn ~ 1450 2250
NoConn ~ 1450 2350
NoConn ~ 1450 2450
NoConn ~ 1450 2550
NoConn ~ 1450 2750
NoConn ~ 1450 2950
NoConn ~ 1450 3050
Text Label 1050 2650 0    50   ~ 0
R~W
Text Label 1050 2850 0    50   ~ 0
PHI2
Text Label 1050 3150 0    50   ~ 0
D7
Text Label 1050 3250 0    50   ~ 0
D6
Text Label 1050 3350 0    50   ~ 0
D5
Text Label 1050 3450 0    50   ~ 0
D4
Text Label 1050 3550 0    50   ~ 0
D3
Text Label 1050 3650 0    50   ~ 0
D2
Text Label 1050 3750 0    50   ~ 0
D1
Text Label 1050 3850 0    50   ~ 0
D0
Text Label 1050 3950 0    50   ~ 0
VCC
Text Label 3050 2150 2    50   ~ 0
EXP1
Text Label 3050 2250 2    50   ~ 0
EXP0
Text Label 3050 2350 2    50   ~ 0
A15
Text Label 3050 2450 2    50   ~ 0
A14
Text Label 3050 2550 2    50   ~ 0
A13
Text Label 3050 2650 2    50   ~ 0
A12
Text Label 3050 2750 2    50   ~ 0
A11
Text Label 3050 2850 2    50   ~ 0
A10
Text Label 3050 2950 2    50   ~ 0
A9
Text Label 3050 3050 2    50   ~ 0
A8
Text Label 3050 3150 2    50   ~ 0
A7
Text Label 3050 3250 2    50   ~ 0
A6
Text Label 3050 3350 2    50   ~ 0
A5
Text Label 3050 3450 2    50   ~ 0
A4
Text Label 3050 3550 2    50   ~ 0
A3
Text Label 3050 3650 2    50   ~ 0
A2
Text Label 3050 3750 2    50   ~ 0
A1
Text Label 3050 3850 2    50   ~ 0
A0
Text Label 5150 2150 2    50   ~ 0
VCC
Text Label 5150 2350 2    50   ~ 0
A13
Text Label 5150 2450 2    50   ~ 0
A8
Text Label 5150 2550 2    50   ~ 0
A9
Text Label 5150 2650 2    50   ~ 0
A11
Text Label 5150 2850 2    50   ~ 0
A10
Text Label 5150 3050 2    50   ~ 0
D7
Text Label 5150 3150 2    50   ~ 0
D6
Text Label 5150 3250 2    50   ~ 0
D5
Text Label 5150 3350 2    50   ~ 0
D4
Text Label 5150 3450 2    50   ~ 0
D3
Text Label 7250 2150 2    50   ~ 0
VCC
Text Label 7250 2350 2    50   ~ 0
A13
Text Label 7250 2450 2    50   ~ 0
A8
Text Label 7250 2550 2    50   ~ 0
A9
Text Label 7250 2650 2    50   ~ 0
A11
Text Label 7250 2850 2    50   ~ 0
A10
Text Label 7250 3050 2    50   ~ 0
D7
Text Label 7250 3150 2    50   ~ 0
D6
Text Label 7250 3250 2    50   ~ 0
D5
Text Label 7250 3350 2    50   ~ 0
D4
Text Label 7250 3450 2    50   ~ 0
D3
Text Label 3350 2150 0    50   ~ 0
A14
Text Label 3350 2250 0    50   ~ 0
A12
Text Label 3350 2350 0    50   ~ 0
A7
Text Label 3350 2450 0    50   ~ 0
A6
Text Label 3350 2550 0    50   ~ 0
A5
Text Label 3350 2650 0    50   ~ 0
A4
Text Label 3350 2750 0    50   ~ 0
A3
Text Label 3350 2850 0    50   ~ 0
A2
Text Label 3350 2950 0    50   ~ 0
A1
Text Label 3350 3050 0    50   ~ 0
A0
Text Label 3350 3150 0    50   ~ 0
D0
Text Label 3350 3250 0    50   ~ 0
D1
Text Label 3350 3350 0    50   ~ 0
D2
Text Label 3350 3450 0    50   ~ 0
GND
Text Label 5450 3450 0    50   ~ 0
GND
Text Label 5450 2150 0    50   ~ 0
A14
Text Label 5450 2250 0    50   ~ 0
A12
Text Label 5450 2350 0    50   ~ 0
A7
Text Label 5450 2450 0    50   ~ 0
A6
Text Label 5450 2550 0    50   ~ 0
A5
Text Label 5450 2650 0    50   ~ 0
A4
Text Label 5450 2750 0    50   ~ 0
A3
Text Label 5450 2850 0    50   ~ 0
A2
Text Label 5450 2950 0    50   ~ 0
A1
Text Label 5450 3050 0    50   ~ 0
A0
Text Label 5450 3150 0    50   ~ 0
D0
Text Label 5450 3250 0    50   ~ 0
D1
Text Label 5450 3350 0    50   ~ 0
D2
$Comp
L 74xx:74HC00 U1
U 1 1 60846A9F
P 9500 2150
F 0 "U1" H 9500 2475 50  0000 C CNN
F 1 "74HC00" H 9500 2384 50  0000 C CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 9500 2150 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 9500 2150 50  0001 C CNN
F 4 "C5586" H 9500 2150 50  0001 C CNN "LCSC"
	1    9500 2150
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC00 U1
U 2 1 6084B876
P 9500 2750
F 0 "U1" H 9500 3075 50  0000 C CNN
F 1 "74HC00" H 9500 2984 50  0000 C CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 9500 2750 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 9500 2750 50  0001 C CNN
F 4 "C5586" H 9500 2750 50  0001 C CNN "LCSC"
	2    9500 2750
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC00 U1
U 3 1 6084D585
P 8650 2850
F 0 "U1" H 8650 3175 50  0000 C CNN
F 1 "74HC00" H 8650 3084 50  0000 C CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 8650 2850 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 8650 2850 50  0001 C CNN
F 4 "C5586" H 8650 2850 50  0001 C CNN "LCSC"
	3    8650 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 2850 8950 2850
Connection ~ 8950 2850
Wire Wire Line
	8950 2850 9200 2850
Wire Wire Line
	9200 2250 9050 2250
Wire Wire Line
	9050 2250 9050 2450
Wire Wire Line
	9050 2650 9200 2650
Wire Wire Line
	8350 2950 8300 2950
Wire Wire Line
	8300 2950 8300 2850
Wire Wire Line
	8300 2750 8350 2750
Wire Wire Line
	9050 2450 8050 2450
Connection ~ 9050 2450
Wire Wire Line
	9050 2450 9050 2650
Wire Wire Line
	8300 2850 8150 2850
Wire Wire Line
	8150 2850 8150 2050
Wire Wire Line
	8150 2050 9200 2050
Connection ~ 8300 2850
Wire Wire Line
	8300 2850 8300 2750
Wire Wire Line
	8150 2050 8050 2050
Connection ~ 8150 2050
Text GLabel 8050 2450 0    50   Input ~ 0
PHI2
Text GLabel 8050 2050 0    50   Input ~ 0
R~W
Wire Wire Line
	9800 2150 9900 2150
Wire Wire Line
	9800 2750 9900 2750
Text GLabel 9900 2150 2    50   Output ~ 0
~READ
Text GLabel 9900 2750 2    50   Output ~ 0
~WRITE
$Comp
L 74xx:74HC00 U2
U 4 1 60882FE8
P 5850 5850
F 0 "U2" H 5850 6175 50  0000 C CNN
F 1 "74HC00" H 5850 6084 50  0000 C CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 5850 5850 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 5850 5850 50  0001 C CNN
F 4 "C5586" H 5850 5850 50  0001 C CNN "LCSC"
	4    5850 5850
	1    0    0    -1  
$EndComp
Text GLabel 7400 5850 2    50   Output ~ 0
~ROM_ENABLE
$Comp
L 74xx:74HC00 U1
U 5 1 6088BB28
P 8200 4350
F 0 "U1" H 8350 4800 50  0000 L CNN
F 1 "74HC00" H 8350 4700 50  0000 L CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 8200 4350 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 8200 4350 50  0001 C CNN
F 4 "C5586" H 8200 4350 50  0001 C CNN "LCSC"
	5    8200 4350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 6088DDE2
P 7750 4350
F 0 "C3" H 7800 4450 50  0000 L CNN
F 1 "100nF" H 7800 4250 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7788 4200 50  0001 C CNN
F 3 "~" H 7750 4350 50  0001 C CNN
F 4 "C49678" H 7750 4350 50  0001 C CNN "LCSC"
	1    7750 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 3850 8200 3750
Wire Wire Line
	7750 3750 7750 4200
Wire Wire Line
	7750 4500 7750 4950
Wire Wire Line
	7750 4950 8200 4950
Wire Wire Line
	8200 4950 8200 4850
Wire Wire Line
	7750 3750 7750 3650
Connection ~ 7750 3750
Text GLabel 7750 3650 1    50   Input ~ 0
VCC
$Comp
L power:PWR_FLAG #FLG01
U 1 1 608A2D22
P 7750 3750
F 0 "#FLG01" H 7750 3825 50  0001 C CNN
F 1 "PWR_FLAG" V 7750 3877 50  0001 L CNN
F 2 "" H 7750 3750 50  0001 C CNN
F 3 "~" H 7750 3750 50  0001 C CNN
	1    7750 3750
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C2
U 1 1 608A3815
P 6300 3850
F 0 "C2" V 6048 3850 50  0000 C CNN
F 1 "100nF" V 6139 3850 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6338 3700 50  0001 C CNN
F 3 "~" H 6300 3850 50  0001 C CNN
F 4 "C49678" H 6300 3850 50  0001 C CNN "LCSC"
	1    6300 3850
	0    1    1    0   
$EndComp
$Comp
L Device:C C1
U 1 1 608A3DE9
P 4250 3850
F 0 "C1" V 3998 3850 50  0000 C CNN
F 1 "100nF" V 4089 3850 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4288 3700 50  0001 C CNN
F 3 "~" H 4250 3850 50  0001 C CNN
F 4 "C49678" V 4250 3850 50  0001 C CNN "LCSC"
	1    4250 3850
	0    1    1    0   
$EndComp
Wire Wire Line
	5650 3450 5650 3850
Wire Wire Line
	5650 3850 6150 3850
Connection ~ 5650 3450
Wire Wire Line
	5650 3450 5400 3450
Wire Wire Line
	3600 3450 3600 3850
Wire Wire Line
	3600 3850 4100 3850
Connection ~ 3600 3450
Wire Wire Line
	3600 3450 3300 3450
Wire Wire Line
	4400 3850 4850 3850
Wire Wire Line
	4850 3850 4850 2150
Connection ~ 4850 2150
Wire Wire Line
	4850 2150 5200 2150
Wire Wire Line
	6450 3850 6950 3850
Wire Wire Line
	6950 3850 6950 2150
Connection ~ 6950 2150
Wire Wire Line
	6950 2150 7300 2150
$Comp
L 74xx:74HC00 U3
U 1 1 608D22EB
P 2150 6350
F 0 "U3" H 2150 6675 50  0000 C CNN
F 1 "74HC00" H 2150 6584 50  0000 C CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 2150 6350 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 2150 6350 50  0001 C CNN
F 4 "C5586" H 2150 6350 50  0001 C CNN "LCSC"
	1    2150 6350
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC00 U3
U 2 1 608D4F07
P 3050 6350
F 0 "U3" H 3050 6675 50  0000 C CNN
F 1 "74HC00" H 3050 6584 50  0000 C CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 3050 6350 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 3050 6350 50  0001 C CNN
F 4 "C5586" H 3050 6350 50  0001 C CNN "LCSC"
	2    3050 6350
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC00 U3
U 3 1 608D6F8D
P 3950 6350
F 0 "U3" H 3950 6675 50  0000 C CNN
F 1 "74HC00" H 3950 6584 50  0000 C CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 3950 6350 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 3950 6350 50  0001 C CNN
F 4 "C5586" H 3950 6350 50  0001 C CNN "LCSC"
	3    3950 6350
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC00 U3
U 4 1 608D8F72
P 4850 6350
F 0 "U3" H 4850 6675 50  0000 C CNN
F 1 "74HC00" H 4850 6584 50  0000 C CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 4850 6350 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 4850 6350 50  0001 C CNN
F 4 "C5586" H 4850 6350 50  0001 C CNN "LCSC"
	4    4850 6350
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC00 U2
U 1 1 608DC5E2
P 3050 5350
F 0 "U2" H 3050 5675 50  0000 C CNN
F 1 "74HC00" H 3050 5584 50  0000 C CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 3050 5350 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 3050 5350 50  0001 C CNN
F 4 "C5586" H 3050 5350 50  0001 C CNN "LCSC"
	1    3050 5350
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC00 U2
U 2 1 608DE631
P 3950 5350
F 0 "U2" H 3950 5675 50  0000 C CNN
F 1 "74HC00" H 3950 5584 50  0000 C CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 3950 5350 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 3950 5350 50  0001 C CNN
F 4 "C5586" H 3950 5350 50  0001 C CNN "LCSC"
	2    3950 5350
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC00 U2
U 3 1 608E08AF
P 4850 5350
F 0 "U2" H 4850 5675 50  0000 C CNN
F 1 "74HC00" H 4850 5584 50  0000 C CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 4850 5350 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 4850 5350 50  0001 C CNN
F 4 "C5586" H 4850 5350 50  0001 C CNN "LCSC"
	3    4850 5350
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC00 U1
U 4 1 608E2BC4
P 6700 5850
F 0 "U1" H 6700 6175 50  0000 C CNN
F 1 "74HC00" H 6700 6084 50  0000 C CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 6700 5850 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 6700 5850 50  0001 C CNN
F 4 "C5586" H 6700 5850 50  0001 C CNN "LCSC"
	4    6700 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 5350 5300 5350
Wire Wire Line
	5300 5350 5300 5750
Wire Wire Line
	5300 6350 5300 5950
Wire Wire Line
	5150 6350 5300 6350
Wire Wire Line
	4550 6450 4400 6450
Wire Wire Line
	4400 6450 4400 6350
Wire Wire Line
	4400 6350 4250 6350
Wire Wire Line
	4550 5450 4400 5450
Wire Wire Line
	4400 5450 4400 5350
Wire Wire Line
	4400 5350 4250 5350
Wire Wire Line
	3550 6250 3550 6350
Wire Wire Line
	3550 6450 3650 6450
Wire Wire Line
	3550 6250 3650 6250
Wire Wire Line
	3650 5250 3550 5250
Wire Wire Line
	3550 5250 3550 5350
Wire Wire Line
	3550 5450 3650 5450
Wire Wire Line
	3350 5350 3550 5350
Connection ~ 3550 5350
Wire Wire Line
	3550 5350 3550 5450
Wire Wire Line
	3350 6350 3550 6350
Connection ~ 3550 6350
Wire Wire Line
	3550 6350 3550 6450
Wire Wire Line
	1850 6250 1800 6250
Wire Wire Line
	1800 6250 1800 6350
Wire Wire Line
	1800 6450 1850 6450
Wire Wire Line
	1800 6350 1650 6350
Wire Wire Line
	1650 6350 1650 5900
Wire Wire Line
	1650 5450 2750 5450
Connection ~ 1800 6350
Wire Wire Line
	1800 6350 1800 6450
Wire Wire Line
	2450 6350 2600 6350
Wire Wire Line
	2600 6350 2600 6250
Wire Wire Line
	2600 6250 2750 6250
Wire Wire Line
	2750 6450 2600 6450
Wire Wire Line
	2600 6450 2600 6750
Wire Wire Line
	2600 6750 1500 6750
Wire Wire Line
	1650 5900 1500 5900
Connection ~ 1650 5900
Wire Wire Line
	1650 5900 1650 5450
Text GLabel 1500 6750 0    50   Input ~ 0
A13
Text GLabel 1500 5900 0    50   Input ~ 0
A14
Wire Wire Line
	2750 5250 2600 5250
Text GLabel 1500 5250 0    50   Input ~ 0
~HI_ROM_DISABLE
Wire Wire Line
	4550 5250 4450 5250
Wire Wire Line
	4450 5250 4450 4900
Wire Wire Line
	4450 5250 4450 6250
Wire Wire Line
	4450 6250 4550 6250
Connection ~ 4450 5250
Text GLabel 4450 4900 1    50   Input ~ 0
A15
$Comp
L 74xx:74HC00 U2
U 5 1 60A19523
P 9150 4350
F 0 "U2" H 9300 4800 50  0000 L CNN
F 1 "74HC00" H 9300 4700 50  0000 L CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 9150 4350 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 9150 4350 50  0001 C CNN
F 4 "C5586" H 9150 4350 50  0001 C CNN "LCSC"
	5    9150 4350
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC00 U3
U 5 1 60A1C055
P 10050 4350
F 0 "U3" H 10200 4800 50  0000 L CNN
F 1 "74HC00" H 10200 4700 50  0000 L CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 10050 4350 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc00" H 10050 4350 50  0001 C CNN
F 4 "C5586" H 10050 4350 50  0001 C CNN "LCSC"
	5    10050 4350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 60A2B83E
P 8700 4350
F 0 "C4" H 8750 4450 50  0000 L CNN
F 1 "100nF" H 8750 4250 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8738 4200 50  0001 C CNN
F 3 "~" H 8700 4350 50  0001 C CNN
F 4 "C49678" H 8700 4350 50  0001 C CNN "LCSC"
	1    8700 4350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 60A2BC23
P 9600 4350
F 0 "C5" H 9650 4450 50  0000 L CNN
F 1 "100nF" H 9650 4250 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9638 4200 50  0001 C CNN
F 3 "~" H 9600 4350 50  0001 C CNN
F 4 "C49678" H 9600 4350 50  0001 C CNN "LCSC"
	1    9600 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 4200 8700 3750
Wire Wire Line
	8700 3750 9150 3750
Wire Wire Line
	9150 3750 9150 3850
Wire Wire Line
	8700 4500 8700 4950
Wire Wire Line
	8700 4950 9150 4950
Wire Wire Line
	9150 4950 9150 4850
Wire Wire Line
	9150 4950 9600 4950
Wire Wire Line
	10050 4950 10050 4850
Connection ~ 9150 4950
Wire Wire Line
	9600 4500 9600 4950
Connection ~ 9600 4950
Wire Wire Line
	9600 4950 10050 4950
Wire Wire Line
	8200 4950 8700 4950
Connection ~ 8200 4950
Connection ~ 8700 4950
Wire Wire Line
	7750 3750 8200 3750
Connection ~ 8700 3750
Connection ~ 8200 3750
Wire Wire Line
	8200 3750 8700 3750
Wire Wire Line
	9150 3750 9600 3750
Wire Wire Line
	9600 3750 9600 4200
Connection ~ 9150 3750
Wire Wire Line
	9600 3750 10050 3750
Wire Wire Line
	10050 3750 10050 3850
Connection ~ 9600 3750
Wire Wire Line
	10050 4950 10050 5050
Connection ~ 10050 4950
$Comp
L power:GND #PWR01
U 1 1 60AAD6E4
P 10050 5050
F 0 "#PWR01" H 10050 4800 50  0001 C CNN
F 1 "GND" H 10055 4877 50  0000 C CNN
F 2 "" H 10050 5050 50  0001 C CNN
F 3 "" H 10050 5050 50  0001 C CNN
	1    10050 5050
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG02
U 1 1 60AADA6D
P 10050 4950
F 0 "#FLG02" H 10050 5025 50  0001 C CNN
F 1 "PWR_FLAG" V 10050 5077 50  0001 L CNN
F 2 "" H 10050 4950 50  0001 C CNN
F 3 "~" H 10050 4950 50  0001 C CNN
	1    10050 4950
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_02x02_Odd_Even J2
U 1 1 60AE5EB7
P 4000 7300
F 0 "J2" H 4050 7425 50  0000 C CNN
F 1 "Conn_02x02_Odd_Even" H 4050 7426 50  0001 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x02_P2.54mm_Vertical" H 4000 7300 50  0001 C CNN
F 3 "~" H 4000 7300 50  0001 C CNN
	1    4000 7300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 7300 3750 7300
Wire Wire Line
	3800 7400 3750 7400
Text GLabel 3700 7300 0    50   Input ~ 0
EXP1
Text GLabel 3700 7400 0    50   Input ~ 0
EXP0
Wire Wire Line
	4300 7300 4350 7300
Wire Wire Line
	4350 7300 4350 7350
Wire Wire Line
	4350 7400 4300 7400
Wire Wire Line
	4350 7350 4450 7350
Connection ~ 4350 7350
Wire Wire Line
	4350 7350 4350 7400
Text GLabel 4450 7350 2    50   Output ~ 0
~HI_ROM_DISABLE
$Comp
L Jumper:SolderJumper_2_Open JP2
U 1 1 60B3698E
P 4050 7000
F 0 "JP2" H 4050 7113 50  0000 C CNN
F 1 "SolderJumper_2_Open" H 4050 7114 50  0001 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_RoundedPad1.0x1.5mm" H 4050 7000 50  0001 C CNN
F 3 "~" H 4050 7000 50  0001 C CNN
	1    4050 7000
	1    0    0    -1  
$EndComp
$Comp
L Jumper:SolderJumper_2_Open JP3
U 1 1 60B3724F
P 4050 7650
F 0 "JP3" H 4050 7763 50  0000 C CNN
F 1 "SolderJumper_2_Open" H 4050 7764 50  0001 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_RoundedPad1.0x1.5mm" H 4050 7650 50  0001 C CNN
F 3 "~" H 4050 7650 50  0001 C CNN
	1    4050 7650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 7000 4350 7000
Wire Wire Line
	4350 7000 4350 7300
Connection ~ 4350 7300
Wire Wire Line
	4350 7400 4350 7650
Wire Wire Line
	4350 7650 4200 7650
Connection ~ 4350 7400
Wire Wire Line
	3750 7000 3750 7300
Connection ~ 3750 7300
Wire Wire Line
	3750 7300 3700 7300
Wire Wire Line
	3750 7650 3750 7400
Connection ~ 3750 7400
Wire Wire Line
	3750 7400 3700 7400
$Comp
L Device:R R2
U 1 1 60B7A29A
P 2600 5000
F 0 "R2" H 2450 5050 50  0000 L CNN
F 1 "1k" H 2450 4950 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 2530 5000 50  0001 C CNN
F 3 "~" H 2600 5000 50  0001 C CNN
F 4 "C17513" H 2600 5000 50  0001 C CNN "LCSC"
	1    2600 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 5150 2600 5250
Connection ~ 2600 5250
Wire Wire Line
	2600 5250 1500 5250
Wire Wire Line
	2600 4850 2600 4750
Text GLabel 2600 4750 1    50   Input ~ 0
VCC
Wire Wire Line
	5300 5750 5550 5750
Wire Wire Line
	5550 5950 5300 5950
Wire Wire Line
	6400 5750 6300 5750
Wire Wire Line
	6300 5750 6300 5850
Wire Wire Line
	6300 5950 6400 5950
Wire Wire Line
	6150 5850 6300 5850
Connection ~ 6300 5850
Wire Wire Line
	6300 5850 6300 5950
Wire Wire Line
	7000 5850 7400 5850
Wire Wire Line
	3750 7000 3900 7000
Wire Wire Line
	3750 7650 3900 7650
Wire Bus Line
	7400 2050 7400 4350
Wire Bus Line
	900  1950 900  4350
Wire Bus Line
	5300 2050 5300 4350
Wire Bus Line
	3200 2050 3200 4350
$EndSCHEMATC
