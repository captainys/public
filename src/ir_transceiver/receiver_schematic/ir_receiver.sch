EESchema Schematic File Version 4
LIBS:IRSensor-cache
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Transistor_BJT:BC337 Q1
U 1 1 5D980FF3
P 2700 2400
F 0 "Q1" H 2891 2446 50  0000 L CNN
F 1 "BC337" H 2891 2355 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 2900 2325 50  0001 L CIN
F 3 "http://www.nxp.com/documents/data_sheet/BC817_BC817W_BC337.pdf" H 2700 2400 50  0001 L CNN
	1    2700 2400
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:BC337 Q2
U 1 1 5D98102F
P 3450 2000
F 0 "Q2" H 3641 2046 50  0000 L CNN
F 1 "BC337" H 3641 1955 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 3650 1925 50  0001 L CIN
F 3 "http://www.nxp.com/documents/data_sheet/BC817_BC817W_BC337.pdf" H 3450 2000 50  0001 L CNN
	1    3450 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 1300 4100 1300
Wire Wire Line
	2800 1300 2800 1550
Wire Wire Line
	3550 1300 3550 1800
Connection ~ 3550 1300
Wire Wire Line
	3550 1300 2800 1300
$Comp
L Device:R_US R1
U 1 1 5D981634
P 2800 1700
F 0 "R1" H 2868 1746 50  0000 L CNN
F 1 "1.2K" H 2868 1655 50  0000 L CNN
F 2 "" V 2840 1690 50  0001 C CNN
F 3 "~" H 2800 1700 50  0001 C CNN
	1    2800 1700
	1    0    0    -1  
$EndComp
$Comp
L Device:D_Photo D1
U 1 1 5D9816AD
P 2150 1950
F 0 "D1" V 2054 2107 50  0000 L CNN
F 1 "PD204-6B" V 2145 2107 50  0000 L CNN
F 2 "" H 2100 1950 50  0001 C CNN
F 3 "~" H 2100 1950 50  0001 C CNN
	1    2150 1950
	0    1    1    0   
$EndComp
Wire Wire Line
	2500 2400 2150 2400
Wire Wire Line
	2150 2400 2150 2050
Wire Wire Line
	2150 1750 2150 1300
Wire Wire Line
	2150 1300 2800 1300
Connection ~ 2800 1300
Wire Wire Line
	2800 1850 2800 2000
Wire Wire Line
	3250 2000 2800 2000
Connection ~ 2800 2000
Wire Wire Line
	2800 2000 2800 2200
Wire Wire Line
	2800 2600 2800 3050
$Comp
L Device:LED D2
U 1 1 5D983800
P 3550 2850
F 0 "D2" V 3588 2733 50  0000 R CNN
F 1 "LED" V 3497 2733 50  0000 R CNN
F 2 "" H 3550 2850 50  0001 C CNN
F 3 "~" H 3550 2850 50  0001 C CNN
	1    3550 2850
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_US R2
U 1 1 5D9838BB
P 3550 2450
F 0 "R2" H 3618 2496 50  0000 L CNN
F 1 "220" H 3618 2405 50  0000 L CNN
F 2 "" V 3590 2440 50  0001 C CNN
F 3 "~" H 3550 2450 50  0001 C CNN
	1    3550 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 2200 3550 2250
Wire Wire Line
	3550 2600 3550 2700
Wire Wire Line
	3550 3000 3550 3050
Wire Wire Line
	4100 1300 4100 1550
Connection ~ 4100 1300
$Comp
L Device:CP C1
U 1 1 5DBA76CB
P 4100 1700
F 0 "C1" H 4218 1746 50  0000 L CNN
F 1 "10uF" H 4218 1655 50  0000 L CNN
F 2 "" H 4138 1550 50  0001 C CNN
F 3 "~" H 4100 1700 50  0001 C CNN
	1    4100 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 1850 4100 1950
Wire Wire Line
	3550 2250 3900 2250
Wire Wire Line
	3900 2250 3900 2950
Connection ~ 3550 2250
Wire Wire Line
	3550 2250 3550 2300
Text Notes 4250 3100 0    50   ~ 0
(Active Low)
Wire Wire Line
	3550 3050 2800 3050
Connection ~ 2800 3050
Wire Wire Line
	2800 3050 2650 3050
Text GLabel 4200 2950 2    50   Input ~ 0
Arduino_Pin3
Wire Wire Line
	3900 2950 4200 2950
Text GLabel 2650 3050 0    50   Input ~ 0
Arduino_GND
Text GLabel 4100 1950 3    50   Input ~ 0
Arduino_GND
Text GLabel 4250 1300 2    50   Input ~ 0
Arduino_+5V
Wire Wire Line
	4100 1300 3550 1300
$EndSCHEMATC
