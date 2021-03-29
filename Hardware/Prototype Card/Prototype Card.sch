EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "6502 Prototype Card"
Date "2021-03-27"
Rev "1"
Comp "A.C. Wright Design"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Prototype_Card:BREADBOARDMINI B1
U 1 1 6925386A
P 4350 7400
F 0 "B1" H 4150 7610 70  0000 L BNN
F 1 "BREADBOARDMINI" H 4150 7190 70  0000 L TNN
F 2 "Prototype Card:BREADBOARD-MINI" H 4350 7400 50  0001 C CNN
F 3 "" H 4350 7400 50  0001 C CNN
	1    4350 7400
	1    0    0    -1  
$EndComp
$Comp
L Prototype_Card:BREADBOARDMINI B2
U 1 1 3BC00CCD
P 5400 7400
F 0 "B2" H 5200 7610 70  0000 L BNN
F 1 "BREADBOARDMINI" H 5200 7190 70  0000 L TNN
F 2 "Prototype Card:BREADBOARD-MINI" H 5400 7400 50  0001 C CNN
F 3 "" H 5400 7400 50  0001 C CNN
	1    5400 7400
	1    0    0    -1  
$EndComp
$Comp
L Prototype_Card:6502CARDEDGE CONN1
U 1 1 60623F64
P 2400 2600
F 0 "CONN1" H 2400 3772 59  0000 C CNN
F 1 "6502CARDEDGE" H 2400 3674 50  0000 C CNN
F 2 "Prototype Card:6502CARDEDGE" H 2400 2600 50  0001 C CNN
F 3 "" H 2400 2600 50  0001 C CNN
	1    2400 2600
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J1
U 1 1 606260CE
P 4400 2600
F 0 "J1" H 4450 3625 50  0000 C CNN
F 1 "Conn_02x20_Odd_Even" H 4450 3626 50  0001 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" H 4400 2600 50  0001 C CNN
F 3 "~" H 4400 2600 50  0001 C CNN
	1    4400 2600
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS138 U1
U 1 1 6062C5A6
P 7350 2650
F 0 "U1" H 7450 3250 50  0000 C CNN
F 1 "74LS138" H 7550 3150 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 7350 2650 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS138" H 7350 2650 50  0001 C CNN
	1    7350 2650
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x04_Odd_Even J2
U 1 1 6062D980
P 8750 2650
F 0 "J2" H 8800 2967 50  0000 C CNN
F 1 "Conn_02x04_Odd_Even" H 8800 2876 50  0001 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x04_P2.54mm_Vertical" H 8750 2650 50  0001 C CNN
F 3 "~" H 8750 2650 50  0001 C CNN
	1    8750 2650
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 606309ED
P 4450 1350
F 0 "C1" V 4702 1350 50  0000 C CNN
F 1 "100nF" V 4611 1350 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4488 1200 50  0001 C CNN
F 3 "~" H 4450 1350 50  0001 C CNN
	1    4450 1350
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C3
U 1 1 6063110D
P 4450 4000
F 0 "C3" V 4702 4000 50  0000 C CNN
F 1 "100nF" V 4611 4000 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4488 3850 50  0001 C CNN
F 3 "~" H 4450 4000 50  0001 C CNN
	1    4450 4000
	0    -1   -1   0   
$EndComp
Wire Bus Line
	1250 4300 3550 4300
Connection ~ 3550 4300
Wire Bus Line
	3550 4300 5350 4300
Wire Wire Line
	4200 1700 4100 1700
Wire Wire Line
	4100 1700 4100 1350
Wire Wire Line
	4100 1350 4300 1350
Wire Wire Line
	4600 1350 4800 1350
Wire Wire Line
	4800 1350 4800 1700
Wire Wire Line
	4800 1700 4700 1700
Wire Wire Line
	4200 3600 4100 3600
Wire Wire Line
	4100 4000 4300 4000
Wire Wire Line
	4700 3600 4800 3600
Wire Wire Line
	4800 3600 4800 4000
Wire Wire Line
	4800 4000 4600 4000
Entry Wire Line
	3550 1600 3650 1700
Entry Wire Line
	3550 1700 3450 1800
Entry Wire Line
	3550 1800 3450 1900
Entry Wire Line
	3550 1900 3450 2000
Entry Wire Line
	3550 2000 3450 2100
Entry Wire Line
	3550 2100 3450 2200
Entry Wire Line
	3550 2200 3450 2300
Entry Wire Line
	3550 2300 3450 2400
Entry Wire Line
	3550 2400 3450 2500
Entry Wire Line
	3550 2500 3450 2600
Entry Wire Line
	3550 2600 3450 2700
Entry Wire Line
	3550 2700 3450 2800
Entry Wire Line
	3550 2800 3450 2900
Entry Wire Line
	3550 2900 3450 3000
Entry Wire Line
	3550 3000 3450 3100
Entry Wire Line
	3550 3100 3450 3200
Entry Wire Line
	3550 3200 3450 3300
Entry Wire Line
	3550 3300 3450 3400
Entry Wire Line
	3550 3400 3450 3500
Entry Wire Line
	3550 1700 3650 1800
Entry Wire Line
	3550 1800 3650 1900
Entry Wire Line
	3550 1900 3650 2000
Entry Wire Line
	3550 2000 3650 2100
Entry Wire Line
	3550 2100 3650 2200
Entry Wire Line
	3550 2200 3650 2300
Entry Wire Line
	3550 2300 3650 2400
Entry Wire Line
	3550 2400 3650 2500
Entry Wire Line
	3550 2500 3650 2600
Entry Wire Line
	3550 2600 3650 2700
Entry Wire Line
	3550 2700 3650 2800
Entry Wire Line
	3550 2800 3650 2900
Entry Wire Line
	3550 2900 3650 3000
Entry Wire Line
	3550 3000 3650 3100
Entry Wire Line
	3550 3100 3650 3200
Entry Wire Line
	3550 3200 3650 3300
Entry Wire Line
	3550 3300 3650 3400
Entry Wire Line
	3550 3400 3650 3500
Entry Wire Line
	3550 3500 3650 3600
Entry Wire Line
	5350 1600 5250 1700
Entry Wire Line
	5350 1700 5250 1800
Entry Wire Line
	5350 1800 5250 1900
Entry Wire Line
	5350 1900 5250 2000
Entry Wire Line
	5350 2000 5250 2100
Entry Wire Line
	5350 2100 5250 2200
Entry Wire Line
	5350 2200 5250 2300
Entry Wire Line
	5350 2300 5250 2400
Entry Wire Line
	5350 2400 5250 2500
Entry Wire Line
	5350 2500 5250 2600
Entry Wire Line
	5350 2600 5250 2700
Entry Wire Line
	5350 2700 5250 2800
Entry Wire Line
	5350 2800 5250 2900
Entry Wire Line
	5350 2900 5250 3000
Entry Wire Line
	5350 3000 5250 3100
Entry Wire Line
	5350 3100 5250 3200
Entry Wire Line
	5350 3200 5250 3300
Entry Wire Line
	5350 3300 5250 3400
Entry Wire Line
	5350 3400 5250 3500
Entry Wire Line
	5350 3500 5250 3600
Entry Wire Line
	1250 1600 1350 1700
Entry Wire Line
	1250 1700 1350 1800
Entry Wire Line
	1250 1800 1350 1900
Entry Wire Line
	1250 1900 1350 2000
Entry Wire Line
	1250 2000 1350 2100
Entry Wire Line
	1250 2100 1350 2200
Entry Wire Line
	1250 2200 1350 2300
Entry Wire Line
	1250 2300 1350 2400
Entry Wire Line
	1250 2400 1350 2500
Entry Wire Line
	1250 2500 1350 2600
Entry Wire Line
	1250 2600 1350 2700
Entry Wire Line
	1250 2700 1350 2800
Entry Wire Line
	1250 2800 1350 2900
Entry Wire Line
	1250 2900 1350 3000
Entry Wire Line
	1250 3000 1350 3100
Entry Wire Line
	1250 3100 1350 3200
Entry Wire Line
	1250 3200 1350 3300
Entry Wire Line
	1250 3300 1350 3400
Entry Wire Line
	1250 3400 1350 3500
Entry Wire Line
	1250 3500 1350 3600
Wire Wire Line
	1350 1700 1800 1700
Wire Wire Line
	1800 1800 1350 1800
Wire Wire Line
	1350 1900 1800 1900
Wire Wire Line
	1800 2000 1350 2000
Wire Wire Line
	1350 2100 1800 2100
Wire Wire Line
	1800 2200 1350 2200
Wire Wire Line
	1350 2300 1800 2300
Wire Wire Line
	1800 2400 1350 2400
Wire Wire Line
	1350 2500 1800 2500
Wire Wire Line
	1800 2600 1350 2600
Wire Wire Line
	1350 2700 1800 2700
Wire Wire Line
	1800 2800 1350 2800
Wire Wire Line
	1350 2900 1800 2900
Wire Wire Line
	1800 3000 1350 3000
Wire Wire Line
	1350 3100 1800 3100
Wire Wire Line
	1800 3200 1350 3200
Wire Wire Line
	1350 3300 1800 3300
Wire Wire Line
	1800 3400 1350 3400
Wire Wire Line
	1350 3500 1800 3500
Wire Wire Line
	1800 3600 1350 3600
Wire Wire Line
	3000 1800 3450 1800
Wire Wire Line
	3450 1900 3000 1900
Wire Wire Line
	3000 2000 3450 2000
Wire Wire Line
	3000 2100 3450 2100
Wire Wire Line
	3450 2200 3000 2200
Wire Wire Line
	3000 2300 3450 2300
Wire Wire Line
	3450 2400 3000 2400
Wire Wire Line
	3000 2500 3450 2500
Wire Wire Line
	3450 2600 3000 2600
Wire Wire Line
	3000 2700 3450 2700
Wire Wire Line
	3000 2800 3450 2800
Wire Wire Line
	3450 2900 3000 2900
Wire Wire Line
	3000 3000 3450 3000
Wire Wire Line
	3450 3100 3000 3100
Wire Wire Line
	3000 3200 3450 3200
Wire Wire Line
	3450 3300 3000 3300
Wire Wire Line
	3000 3400 3450 3400
Wire Wire Line
	3450 3500 3000 3500
Wire Wire Line
	4100 3600 4100 4000
Wire Wire Line
	3650 3600 4100 3600
Connection ~ 4100 3600
Wire Wire Line
	4800 3600 5250 3600
Connection ~ 4800 3600
Wire Wire Line
	4800 1700 5250 1700
Connection ~ 4800 1700
Wire Wire Line
	4100 1700 3650 1700
Connection ~ 4100 1700
Wire Wire Line
	3650 1800 4200 1800
Wire Wire Line
	4700 1800 5250 1800
Wire Wire Line
	5250 1900 4700 1900
Wire Wire Line
	4700 2000 5250 2000
Wire Wire Line
	5250 2100 4700 2100
Wire Wire Line
	4700 2200 5250 2200
Wire Wire Line
	5250 2300 4700 2300
Wire Wire Line
	4700 2400 5250 2400
Wire Wire Line
	5250 2500 4700 2500
Wire Wire Line
	4700 2600 5250 2600
Wire Wire Line
	5250 2700 4700 2700
Wire Wire Line
	4700 2800 5250 2800
Wire Wire Line
	5250 2900 4700 2900
Wire Wire Line
	4700 3000 5250 3000
Wire Wire Line
	5250 3100 4700 3100
Wire Wire Line
	4700 3200 5250 3200
Wire Wire Line
	5250 3300 4700 3300
Wire Wire Line
	4700 3400 5250 3400
Wire Wire Line
	5250 3500 4700 3500
Wire Wire Line
	3650 3400 4200 3400
Wire Wire Line
	3650 3500 4200 3500
Wire Wire Line
	4200 3300 3650 3300
Wire Wire Line
	3650 3200 4200 3200
Wire Wire Line
	4200 3100 3650 3100
Wire Wire Line
	3650 3000 4200 3000
Wire Wire Line
	4200 2900 3650 2900
Wire Wire Line
	3650 2800 4200 2800
Wire Wire Line
	4200 2700 3650 2700
Wire Wire Line
	3650 2600 4200 2600
Wire Wire Line
	4200 2500 3650 2500
Wire Wire Line
	3650 2400 4200 2400
Wire Wire Line
	4200 2300 3650 2300
Wire Wire Line
	3650 2200 4200 2200
Wire Wire Line
	4200 2100 3650 2100
Wire Wire Line
	3650 2000 4200 2000
Wire Wire Line
	4200 1900 3650 1900
Wire Wire Line
	7850 2350 8500 2350
Wire Wire Line
	8500 2350 8500 2550
Wire Wire Line
	8500 2550 8550 2550
Wire Wire Line
	7850 2550 8300 2550
Wire Wire Line
	8300 2550 8300 2750
Wire Wire Line
	8300 2750 8550 2750
Wire Wire Line
	7850 2650 8200 2650
Wire Wire Line
	8200 2650 8200 2850
Wire Wire Line
	8200 2850 8550 2850
Wire Wire Line
	7850 2750 8100 2750
Wire Wire Line
	8100 2750 8100 2950
Wire Wire Line
	8100 2950 9150 2950
Wire Wire Line
	9150 2950 9150 2850
Wire Wire Line
	9150 2850 9050 2850
Wire Wire Line
	7850 2850 8000 2850
Wire Wire Line
	8000 2850 8000 3050
Wire Wire Line
	8000 3050 9250 3050
Wire Wire Line
	9250 3050 9250 2750
Wire Wire Line
	9250 2750 9050 2750
Wire Wire Line
	7850 2950 7900 2950
Wire Wire Line
	7900 2950 7900 3150
Wire Wire Line
	7900 3150 9350 3150
Wire Wire Line
	9350 3150 9350 2650
Wire Wire Line
	9350 2650 9050 2650
Wire Wire Line
	8400 2650 8550 2650
Wire Wire Line
	7850 2450 8400 2450
Wire Wire Line
	8400 2450 8400 2650
Wire Wire Line
	9050 2550 9450 2550
Wire Wire Line
	9450 2550 9450 3250
Wire Wire Line
	9450 3250 7850 3250
Wire Wire Line
	7850 3250 7850 3050
Text Label 7900 2350 0    50   ~ 0
$8000
Text Label 7900 2450 0    50   ~ 0
$8400
Text Label 7900 2550 0    50   ~ 0
$8800
Text Label 7900 2650 0    50   ~ 0
$8C00
Text Label 8200 2950 0    50   ~ 0
$9000
Text Label 8200 3050 0    50   ~ 0
$9400
Text Label 8200 3150 0    50   ~ 0
$9800
Text Label 8200 3250 0    50   ~ 0
$9C00
Wire Wire Line
	6850 2350 6750 2350
Wire Wire Line
	6850 2450 6750 2450
Wire Wire Line
	6850 2550 6750 2550
Wire Wire Line
	6750 2850 6850 2850
Wire Wire Line
	6850 2950 6750 2950
Wire Wire Line
	6850 3050 6750 3050
Text GLabel 6750 2350 0    50   Input ~ 0
A10
Text GLabel 6750 2450 0    50   Input ~ 0
A11
Text GLabel 6750 2550 0    50   Input ~ 0
A12
Text GLabel 6750 2850 0    50   Input ~ 0
A15
Text GLabel 6750 2950 0    50   Input ~ 0
A14
Text GLabel 6750 3050 0    50   Input ~ 0
A13
Wire Wire Line
	7350 2050 7350 1750
Wire Wire Line
	7350 3350 7350 3400
$Comp
L power:GND #PWR01
U 1 1 60742704
P 7350 3500
F 0 "#PWR01" H 7350 3250 50  0001 C CNN
F 1 "GND" H 7355 3327 50  0000 C CNN
F 2 "" H 7350 3500 50  0001 C CNN
F 3 "" H 7350 3500 50  0001 C CNN
	1    7350 3500
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG02
U 1 1 60749631
P 7350 3400
F 0 "#FLG02" H 7350 3475 50  0001 C CNN
F 1 "PWR_FLAG" V 7350 3528 50  0001 L CNN
F 2 "" H 7350 3400 50  0001 C CNN
F 3 "~" H 7350 3400 50  0001 C CNN
	1    7350 3400
	0    1    1    0   
$EndComp
Connection ~ 7350 3400
Wire Wire Line
	7350 3400 7350 3500
Text GLabel 7350 1600 1    50   Input ~ 0
VCC
$Comp
L power:PWR_FLAG #FLG01
U 1 1 6074FA87
P 7350 1750
F 0 "#FLG01" H 7350 1825 50  0001 C CNN
F 1 "PWR_FLAG" V 7350 1878 50  0001 L CNN
F 2 "" H 7350 1750 50  0001 C CNN
F 3 "~" H 7350 1750 50  0001 C CNN
	1    7350 1750
	0    1    1    0   
$EndComp
Connection ~ 7350 1750
Wire Wire Line
	7350 1750 7350 1600
$Comp
L Device:C C2
U 1 1 60750911
P 6150 2700
F 0 "C2" V 6402 2700 50  0000 C CNN
F 1 "100nF" V 6311 2700 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6188 2550 50  0001 C CNN
F 3 "~" H 6150 2700 50  0001 C CNN
	1    6150 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 1750 6150 1750
Wire Wire Line
	6150 1750 6150 2550
Wire Wire Line
	6150 3400 6150 2850
Wire Wire Line
	6150 3400 7350 3400
Text Label 1400 1700 0    50   ~ 0
GND
Text Label 1400 1800 0    50   ~ 0
~RES
Text Label 1400 1900 0    50   ~ 0
~IRQ
Text Label 1400 2000 0    50   ~ 0
~NMI
Text Label 1400 2100 0    50   ~ 0
RDY
Text Label 1400 2200 0    50   ~ 0
BE
Text Label 1400 2300 0    50   ~ 0
R~W
Text Label 1400 2400 0    50   ~ 0
~ML
Text Label 1400 2500 0    50   ~ 0
PHI2
Text Label 1400 2600 0    50   ~ 0
PHI1O
Text Label 1400 2700 0    50   ~ 0
SYNC
Text Label 1400 2800 0    50   ~ 0
D7
Text Label 1400 2900 0    50   ~ 0
D6
Text Label 1400 3000 0    50   ~ 0
D5
Text Label 1400 3100 0    50   ~ 0
D4
Text Label 1400 3200 0    50   ~ 0
D3
Text Label 1400 3300 0    50   ~ 0
D2
Text Label 1400 3400 0    50   ~ 0
D1
Text Label 1400 3500 0    50   ~ 0
D0
Text Label 1400 3600 0    50   ~ 0
VCC
Text Label 3700 1700 0    50   ~ 0
GND
Text Label 3700 1800 0    50   ~ 0
~RES
Text Label 3700 1900 0    50   ~ 0
~IRQ
Text Label 3700 2000 0    50   ~ 0
~NMI
Text Label 3700 2100 0    50   ~ 0
RDY
Text Label 3700 2200 0    50   ~ 0
BE
Text Label 3700 2300 0    50   ~ 0
R~W
Text Label 3700 2400 0    50   ~ 0
~ML
Text Label 3700 2500 0    50   ~ 0
PHI2
Text Label 3700 2600 0    50   ~ 0
PHI1O
Text Label 3700 2700 0    50   ~ 0
SYNC
Text Label 3700 2800 0    50   ~ 0
D7
Text Label 3700 2900 0    50   ~ 0
D6
Text Label 3700 3000 0    50   ~ 0
D5
Text Label 3700 3100 0    50   ~ 0
D4
Text Label 3700 3200 0    50   ~ 0
D3
Text Label 3700 3300 0    50   ~ 0
D2
Text Label 3700 3400 0    50   ~ 0
D1
Text Label 3700 3500 0    50   ~ 0
D0
Text Label 3700 3600 0    50   ~ 0
VCC
Text Label 3400 1800 2    50   ~ 0
EXP1
Text Label 3400 1900 2    50   ~ 0
EXP0
Text Label 3400 2000 2    50   ~ 0
A15
Text Label 3400 2100 2    50   ~ 0
A14
Text Label 3400 2200 2    50   ~ 0
A13
Text Label 3400 2300 2    50   ~ 0
A12
Text Label 3400 2400 2    50   ~ 0
A11
Text Label 3400 2500 2    50   ~ 0
A10
Text Label 3400 2600 2    50   ~ 0
A9
Text Label 3400 2700 2    50   ~ 0
A8
Text Label 3400 2800 2    50   ~ 0
A7
Text Label 3400 2900 2    50   ~ 0
A6
Text Label 3400 3000 2    50   ~ 0
A5
Text Label 3400 3100 2    50   ~ 0
A4
Text Label 3400 3200 2    50   ~ 0
A3
Text Label 3400 3300 2    50   ~ 0
A2
Text Label 3400 3400 2    50   ~ 0
A1
Text Label 3400 3500 2    50   ~ 0
A0
Text Label 5200 1700 2    50   ~ 0
VCC
Text Label 5200 1800 2    50   ~ 0
EXP1
Text Label 5200 1900 2    50   ~ 0
EXP0
Text Label 5200 2000 2    50   ~ 0
A15
Text Label 5200 2100 2    50   ~ 0
A14
Text Label 5200 2200 2    50   ~ 0
A13
Text Label 5200 2300 2    50   ~ 0
A12
Text Label 5200 2400 2    50   ~ 0
A11
Text Label 5200 2500 2    50   ~ 0
A10
Text Label 5200 2600 2    50   ~ 0
A9
Text Label 5200 2700 2    50   ~ 0
A8
Text Label 5200 2800 2    50   ~ 0
A7
Text Label 5200 2900 2    50   ~ 0
A6
Text Label 5200 3000 2    50   ~ 0
A5
Text Label 5200 3100 2    50   ~ 0
A4
Text Label 5200 3200 2    50   ~ 0
A3
Text Label 5200 3300 2    50   ~ 0
A2
Text Label 5200 3400 2    50   ~ 0
A1
Text Label 5200 3500 2    50   ~ 0
A0
Text Label 5200 3600 2    50   ~ 0
GND
Wire Bus Line
	1250 1600 1250 4300
Wire Bus Line
	5350 1600 5350 4300
Wire Bus Line
	3550 1600 3550 4300
$EndSCHEMATC
