EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "6502 CPU Card"
Date "2021-03-27"
Rev "1"
Comp "A.C. Wright Design"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	8400 2350 8400 2500
Wire Wire Line
	8400 2850 8400 3150
Wire Wire Line
	8400 3150 8400 3450
Wire Wire Line
	8400 3450 8400 3750
Wire Wire Line
	8400 3750 8400 4050
Wire Wire Line
	8400 4050 8400 4200
Connection ~ 8400 3750
Connection ~ 8400 3450
Connection ~ 8400 3150
Connection ~ 8400 2850
Connection ~ 8400 4050
Text GLabel 8400 2350 1    50   Input ~ 0
VCC
Wire Wire Line
	8400 4500 8400 4600
$Comp
L power:GND #PWR0101
U 1 1 60503285
P 8400 4750
F 0 "#PWR0101" H 8400 4500 50  0001 C CNN
F 1 "GND" H 8405 4577 50  0000 C CNN
F 2 "" H 8400 4750 50  0001 C CNN
F 3 "" H 8400 4750 50  0001 C CNN
	1    8400 4750
	1    0    0    -1  
$EndComp
Text GLabel 9200 2850 2    50   Output ~ 0
~RES
Text GLabel 9200 3150 2    50   Output ~ 0
RDY
Text GLabel 9200 3450 2    50   Output ~ 0
~IRQ
Text GLabel 9200 3750 2    50   Output ~ 0
~NMI
Text GLabel 9200 4050 2    50   Output ~ 0
BE
$Comp
L Device:R R1
U 1 1 6060A619
P 8800 2850
F 0 "R1" V 8593 2850 50  0000 C CNN
F 1 "1k" V 8684 2850 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 8730 2850 50  0001 C CNN
F 3 "~" H 8800 2850 50  0001 C CNN
	1    8800 2850
	0    1    1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 6060AFCC
P 8800 3150
F 0 "R2" V 8593 3150 50  0000 C CNN
F 1 "1k" V 8684 3150 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 8730 3150 50  0001 C CNN
F 3 "~" H 8800 3150 50  0001 C CNN
	1    8800 3150
	0    1    1    0   
$EndComp
$Comp
L Device:R R3
U 1 1 6060B7B7
P 8800 3450
F 0 "R3" V 8593 3450 50  0000 C CNN
F 1 "1k" V 8684 3450 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 8730 3450 50  0001 C CNN
F 3 "~" H 8800 3450 50  0001 C CNN
	1    8800 3450
	0    1    1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 6060BA52
P 8800 3750
F 0 "R4" V 8593 3750 50  0000 C CNN
F 1 "1k" V 8684 3750 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 8730 3750 50  0001 C CNN
F 3 "~" H 8800 3750 50  0001 C CNN
	1    8800 3750
	0    1    1    0   
$EndComp
$Comp
L Device:R R5
U 1 1 6060BD36
P 8800 4050
F 0 "R5" V 8593 4050 50  0000 C CNN
F 1 "1k" V 8684 4050 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 8730 4050 50  0001 C CNN
F 3 "~" H 8800 4050 50  0001 C CNN
	1    8800 4050
	0    1    1    0   
$EndComp
Wire Wire Line
	8400 2850 8650 2850
Wire Wire Line
	8950 2850 9200 2850
Wire Wire Line
	8950 3150 9200 3150
Wire Wire Line
	8400 3150 8650 3150
Wire Wire Line
	8400 3450 8650 3450
Wire Wire Line
	8950 3450 9200 3450
Wire Wire Line
	8950 3750 9200 3750
Wire Wire Line
	8400 3750 8650 3750
Wire Wire Line
	8400 4050 8650 4050
Wire Wire Line
	8950 4050 9200 4050
$Comp
L Device:C C1
U 1 1 60640EB5
P 8400 4350
F 0 "C1" H 8515 4396 50  0000 L CNN
F 1 "100nF" H 8515 4305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8438 4200 50  0001 C CNN
F 3 "~" H 8400 4350 50  0001 C CNN
	1    8400 4350
	1    0    0    -1  
$EndComp
$Comp
L CPU_Card:W65C02S6TPG-14 IC1
U 1 1 605F7FF5
P 4950 2750
F 0 "IC1" H 5550 3086 59  0000 C CNN
F 1 "W65C02S6TPG-14" H 5550 2981 59  0000 C CNN
F 2 "Package_DIP:DIP-40_W15.24mm_Socket_LongPads" H 4950 2750 50  0001 C CNN
F 3 "" H 4950 2750 50  0001 C CNN
	1    4950 2750
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x02_Odd_Even J1
U 1 1 605FA2C4
P 5500 1800
F 0 "J1" H 5550 1925 50  0000 C CNN
F 1 "Conn_02x02_Odd_Even" H 5550 1926 50  0001 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x02_P2.54mm_Vertical" H 5500 1800 50  0001 C CNN
F 3 "~" H 5500 1800 50  0001 C CNN
	1    5500 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 2750 4800 2750
Wire Wire Line
	4800 2750 4800 1900
Wire Wire Line
	4800 1900 5250 1900
Wire Wire Line
	4950 2950 4700 2950
Wire Wire Line
	4700 2950 4700 1800
Wire Wire Line
	4700 1800 5250 1800
Wire Wire Line
	5800 1800 5850 1800
Wire Wire Line
	5800 1900 5850 1900
Text GLabel 5950 1900 2    50   Output ~ 0
EXP0
Text GLabel 5950 1800 2    50   Output ~ 0
PHI1O
Wire Bus Line
	4200 5300 6900 5300
Wire Bus Line
	4200 5300 1800 5300
Connection ~ 4200 5300
Entry Wire Line
	6900 2650 6800 2750
Entry Wire Line
	6900 2950 6800 3050
Entry Wire Line
	6900 3050 6800 3150
Entry Wire Line
	6900 3250 6800 3350
Entry Wire Line
	6900 3350 6800 3450
Entry Wire Line
	6900 3450 6800 3550
Entry Wire Line
	6900 3550 6800 3650
Entry Wire Line
	6900 3650 6800 3750
Entry Wire Line
	6900 3750 6800 3850
Entry Wire Line
	6900 3850 6800 3950
Entry Wire Line
	6900 3950 6800 4050
Entry Wire Line
	6900 4050 6800 4150
Entry Wire Line
	6900 4150 6800 4250
Entry Wire Line
	6900 4250 6800 4350
Entry Wire Line
	6900 4350 6800 4450
Entry Wire Line
	6900 4450 6800 4550
Entry Wire Line
	6900 4550 6800 4650
Entry Wire Line
	4200 2750 4300 2850
Entry Wire Line
	4200 2950 4300 3050
Entry Wire Line
	4200 3050 4300 3150
Entry Wire Line
	4200 3150 4300 3250
Entry Wire Line
	4200 3250 4300 3350
Entry Wire Line
	4200 3350 4300 3450
Entry Wire Line
	4200 3450 4300 3550
Entry Wire Line
	4200 3550 4300 3650
Entry Wire Line
	4200 3650 4300 3750
Entry Wire Line
	4200 3750 4300 3850
Entry Wire Line
	4200 3850 4300 3950
Entry Wire Line
	4200 3950 4300 4050
Entry Wire Line
	4200 4050 4300 4150
Entry Wire Line
	4200 4150 4300 4250
Entry Wire Line
	4200 4250 4300 4350
Entry Wire Line
	4200 4350 4300 4450
Entry Wire Line
	4200 4450 4300 4550
Entry Wire Line
	4200 4550 4300 4650
Entry Wire Line
	4200 2850 4100 2950
Entry Wire Line
	4200 2950 4100 3050
Entry Wire Line
	4200 3050 4100 3150
Entry Wire Line
	4200 3150 4100 3250
Entry Wire Line
	4200 3250 4100 3350
Entry Wire Line
	4200 3350 4100 3450
Entry Wire Line
	4200 3450 4100 3550
Entry Wire Line
	4200 3550 4100 3650
Entry Wire Line
	4200 3650 4100 3750
Entry Wire Line
	4200 3750 4100 3850
Entry Wire Line
	4200 3850 4100 3950
Entry Wire Line
	4200 3950 4100 4050
Entry Wire Line
	4200 4050 4100 4150
Entry Wire Line
	4200 4150 4100 4250
Entry Wire Line
	4200 4250 4100 4350
Entry Wire Line
	4200 4350 4100 4450
Entry Wire Line
	4200 4450 4100 4550
$Comp
L CPU_Card:6502CARDEDGE CONN1
U 1 1 605F5B01
P 3000 3650
F 0 "CONN1" H 3000 4800 59  0000 C CNN
F 1 "6502CARDEDGE" H 3000 4700 50  0000 C CNN
F 2 "CPU Card:6502CARDEDGE" H 3000 3650 50  0001 C CNN
F 3 "" H 3000 3650 50  0001 C CNN
	1    3000 3650
	1    0    0    -1  
$EndComp
Entry Wire Line
	1800 2650 1900 2750
Entry Wire Line
	1800 2750 1900 2850
Entry Wire Line
	1800 2850 1900 2950
Entry Wire Line
	1800 2950 1900 3050
Entry Wire Line
	1800 3050 1900 3150
Entry Wire Line
	1800 3150 1900 3250
Entry Wire Line
	1800 3250 1900 3350
Entry Wire Line
	1800 3350 1900 3450
Entry Wire Line
	1800 3450 1900 3550
Entry Wire Line
	1800 3550 1900 3650
Entry Wire Line
	1800 3650 1900 3750
Entry Wire Line
	1800 3750 1900 3850
Entry Wire Line
	1800 3850 1900 3950
Entry Wire Line
	1800 3950 1900 4050
Entry Wire Line
	1800 4050 1900 4150
Entry Wire Line
	1800 4150 1900 4250
Entry Wire Line
	1800 4250 1900 4350
Entry Wire Line
	1800 4350 1900 4450
Entry Wire Line
	1800 4450 1900 4550
Entry Wire Line
	1800 4550 1900 4650
Wire Wire Line
	6150 2750 6800 2750
Wire Wire Line
	6150 3050 6800 3050
Wire Wire Line
	6150 3150 6800 3150
Wire Wire Line
	6150 3350 6800 3350
Wire Wire Line
	6150 3450 6800 3450
Wire Wire Line
	6150 3550 6800 3550
Wire Wire Line
	6150 3650 6800 3650
Wire Wire Line
	6150 3750 6800 3750
Wire Wire Line
	6150 3850 6800 3850
Wire Wire Line
	6150 3950 6800 3950
Wire Wire Line
	6150 4050 6800 4050
Wire Wire Line
	6150 4150 6800 4150
Wire Wire Line
	6150 4250 6800 4250
Wire Wire Line
	6150 4350 6800 4350
Wire Wire Line
	6150 4450 6800 4450
Wire Wire Line
	6150 4550 6800 4550
Wire Wire Line
	6150 4650 6800 4650
Wire Wire Line
	4950 2850 4300 2850
Wire Wire Line
	4300 3050 4950 3050
Wire Wire Line
	4950 3150 4300 3150
Wire Wire Line
	4300 3250 4950 3250
Wire Wire Line
	4950 3350 4300 3350
Wire Wire Line
	4300 3450 4950 3450
Wire Wire Line
	4950 3550 4300 3550
Wire Wire Line
	4300 3650 4950 3650
Wire Wire Line
	4950 3750 4300 3750
Wire Wire Line
	4300 3850 4950 3850
Wire Wire Line
	4950 3950 4300 3950
Wire Wire Line
	4300 4050 4950 4050
Wire Wire Line
	4950 4150 4300 4150
Wire Wire Line
	4300 4250 4950 4250
Wire Wire Line
	4950 4350 4300 4350
Wire Wire Line
	4300 4450 4950 4450
Wire Wire Line
	4950 4550 4300 4550
Wire Wire Line
	4300 4650 4950 4650
Wire Wire Line
	4100 2950 3600 2950
Wire Wire Line
	3600 3050 4100 3050
Wire Wire Line
	4100 3150 3600 3150
Wire Wire Line
	3600 3250 4100 3250
Wire Wire Line
	3600 3350 4100 3350
Wire Wire Line
	4100 3450 3600 3450
Wire Wire Line
	3600 3550 4100 3550
Wire Wire Line
	4100 3650 3600 3650
Wire Wire Line
	3600 3750 4100 3750
Wire Wire Line
	4100 3850 3600 3850
Wire Wire Line
	3600 3950 4100 3950
Wire Wire Line
	4100 4050 3600 4050
Wire Wire Line
	3600 4150 4100 4150
Wire Wire Line
	4100 4250 3600 4250
Wire Wire Line
	3600 4350 4100 4350
Wire Wire Line
	4100 4450 3600 4450
Wire Wire Line
	3600 4550 4100 4550
Wire Wire Line
	2400 2750 1900 2750
Wire Wire Line
	1900 2850 2400 2850
Wire Wire Line
	2400 2950 1900 2950
Wire Wire Line
	1900 3050 2400 3050
Wire Wire Line
	2400 3150 1900 3150
Wire Wire Line
	1900 3250 2400 3250
Wire Wire Line
	2400 3350 1900 3350
Wire Wire Line
	1900 3450 2400 3450
Wire Wire Line
	2400 3550 1900 3550
Wire Wire Line
	1900 3650 2400 3650
Wire Wire Line
	1900 3850 2400 3850
Wire Wire Line
	1900 3750 2400 3750
Wire Wire Line
	2400 3950 1900 3950
Wire Wire Line
	1900 4050 2400 4050
Wire Wire Line
	2400 4150 1900 4150
Wire Wire Line
	1900 4250 2400 4250
Wire Wire Line
	2400 4350 1900 4350
Wire Wire Line
	1900 4450 2400 4450
Wire Wire Line
	1900 4550 2400 4550
Wire Wire Line
	1900 4650 2400 4650
Text Label 1950 2750 0    50   ~ 0
GND
Text Label 1950 2850 0    50   ~ 0
~RES
Text Label 1950 2950 0    50   ~ 0
~IRQ
Text Label 1950 3050 0    50   ~ 0
~NMI
Text Label 1950 3150 0    50   ~ 0
RDY
Text Label 1950 3250 0    50   ~ 0
BE
Text Label 1950 3350 0    50   ~ 0
R~W
Text Label 1950 3450 0    50   ~ 0
~ML
Text Label 1950 3550 0    50   ~ 0
PHI2
Text Label 1950 3650 0    50   ~ 0
PHI1O
Text Label 1950 3750 0    50   ~ 0
SYNC
Text Label 1950 3850 0    50   ~ 0
D7
Text Label 1950 3950 0    50   ~ 0
D6
Text Label 1950 4050 0    50   ~ 0
D5
Text Label 1950 4150 0    50   ~ 0
D4
Text Label 1950 4250 0    50   ~ 0
D3
Text Label 1950 4350 0    50   ~ 0
D2
Text Label 1950 4450 0    50   ~ 0
D1
Text Label 1950 4550 0    50   ~ 0
D0
Text Label 1950 4650 0    50   ~ 0
VCC
Text Label 4050 2950 2    50   ~ 0
EXP0
Text Label 4050 3050 2    50   ~ 0
A15
Text Label 4050 3150 2    50   ~ 0
A14
Text Label 4050 3250 2    50   ~ 0
A13
Text Label 4050 3350 2    50   ~ 0
A12
Text Label 4050 3450 2    50   ~ 0
A11
Text Label 4050 3550 2    50   ~ 0
A10
Text Label 4050 3650 2    50   ~ 0
A9
Text Label 4050 3750 2    50   ~ 0
A8
Text Label 4050 3850 2    50   ~ 0
A7
Text Label 4050 3950 2    50   ~ 0
A6
Text Label 4050 4050 2    50   ~ 0
A5
Text Label 4050 4150 2    50   ~ 0
A4
Text Label 4050 4250 2    50   ~ 0
A3
Text Label 4050 4350 2    50   ~ 0
A2
Text Label 4050 4450 2    50   ~ 0
A1
Text Label 4050 4550 2    50   ~ 0
A0
Text Label 6750 2750 2    50   ~ 0
~RES
Text Label 6750 3050 2    50   ~ 0
PHI2
Text Label 6750 3150 2    50   ~ 0
BE
Text Label 6750 3350 2    50   ~ 0
R~W
Text Label 6750 3450 2    50   ~ 0
D0
Text Label 6750 3550 2    50   ~ 0
D1
Text Label 6750 3650 2    50   ~ 0
D2
Text Label 6750 3750 2    50   ~ 0
D3
Text Label 6750 3850 2    50   ~ 0
D4
Text Label 6750 3950 2    50   ~ 0
D5
Text Label 6750 4050 2    50   ~ 0
D6
Text Label 6750 4150 2    50   ~ 0
D7
Text Label 6750 4250 2    50   ~ 0
A15
Text Label 6750 4350 2    50   ~ 0
A14
Text Label 6750 4450 2    50   ~ 0
A13
Text Label 6750 4550 2    50   ~ 0
A12
Text Label 6750 4650 2    50   ~ 0
GND
Text Label 4350 2850 0    50   ~ 0
RDY
Text Label 4350 3050 0    50   ~ 0
~IRQ
Text Label 4350 3150 0    50   ~ 0
~ML
Text Label 4350 3250 0    50   ~ 0
~NMI
Text Label 4350 3350 0    50   ~ 0
SYNC
Text Label 4350 3450 0    50   ~ 0
VCC
Text Label 4350 3550 0    50   ~ 0
A0
Text Label 4350 3650 0    50   ~ 0
A1
Text Label 4350 3750 0    50   ~ 0
A2
Text Label 4350 3850 0    50   ~ 0
A3
Text Label 4350 3950 0    50   ~ 0
A4
Text Label 4350 4050 0    50   ~ 0
A5
Text Label 4350 4150 0    50   ~ 0
A6
Text Label 4350 4250 0    50   ~ 0
A7
Text Label 4350 4350 0    50   ~ 0
A8
Text Label 4350 4450 0    50   ~ 0
A9
Text Label 4350 4550 0    50   ~ 0
A10
Text Label 4350 4650 0    50   ~ 0
A11
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 606EA11F
P 8400 2500
F 0 "#FLG0101" H 8400 2575 50  0001 C CNN
F 1 "PWR_FLAG" V 8400 2627 50  0001 L CNN
F 2 "" H 8400 2500 50  0001 C CNN
F 3 "~" H 8400 2500 50  0001 C CNN
	1    8400 2500
	0    -1   -1   0   
$EndComp
Connection ~ 8400 2500
Wire Wire Line
	8400 2500 8400 2850
NoConn ~ 6150 2850
NoConn ~ 6150 2950
NoConn ~ 3600 2850
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 60707BD0
P 8400 4600
F 0 "#FLG0102" H 8400 4675 50  0001 C CNN
F 1 "PWR_FLAG" V 8400 4727 50  0001 L CNN
F 2 "" H 8400 4600 50  0001 C CNN
F 3 "~" H 8400 4600 50  0001 C CNN
	1    8400 4600
	0    -1   -1   0   
$EndComp
Connection ~ 8400 4600
Wire Wire Line
	8400 4600 8400 4750
$Comp
L Jumper:SolderJumper_2_Open JP2
U 1 1 606F23BA
P 5550 2150
F 0 "JP2" H 5550 2263 50  0000 C CNN
F 1 "SolderJumper_2_Open" H 5550 2264 50  0001 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_RoundedPad1.0x1.5mm" H 5550 2150 50  0001 C CNN
F 3 "~" H 5550 2150 50  0001 C CNN
	1    5550 2150
	1    0    0    -1  
$EndComp
$Comp
L Jumper:SolderJumper_2_Open JP1
U 1 1 606F2C26
P 5550 1550
F 0 "JP1" H 5550 1663 50  0000 C CNN
F 1 "SolderJumper_2_Open" H 5550 1664 50  0001 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_RoundedPad1.0x1.5mm" H 5550 1550 50  0001 C CNN
F 3 "~" H 5550 1550 50  0001 C CNN
	1    5550 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 1550 5850 1550
Wire Wire Line
	5850 1550 5850 1800
Connection ~ 5850 1800
Wire Wire Line
	5850 1800 5950 1800
Wire Wire Line
	5400 1550 5250 1550
Wire Wire Line
	5250 1550 5250 1800
Connection ~ 5250 1800
Wire Wire Line
	5250 1800 5300 1800
Wire Wire Line
	5400 2150 5250 2150
Wire Wire Line
	5250 2150 5250 1900
Connection ~ 5250 1900
Wire Wire Line
	5250 1900 5300 1900
Wire Wire Line
	5700 2150 5850 2150
Wire Wire Line
	5850 2150 5850 1900
Wire Bus Line
	4200 2650 4200 5300
Wire Bus Line
	6900 2650 6900 5300
Wire Bus Line
	1800 2650 1800 5300
Connection ~ 5850 1900
Wire Wire Line
	5850 1900 5950 1900
$EndSCHEMATC
