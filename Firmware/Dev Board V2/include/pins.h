#ifndef _PINS_H
#define _PINS_H

#define RESB      28  // RESET            3.18 -> 8.18
#define SYNC      30  // SYNC             3.23 -> 8.23
#define RWB       27  // READ/WRITE       1.31 -> 6.31
#define RDY       29  // RDY              4.31 -> 9.31
#define BE        18  // BE               1.17 -> 6.17
#define PHI2      13  // PHI2             2.3  -> 7.3
#define NMIB      41  // NMI              1.21 -> 6.21
#define IRQB      31  // IRQ              3.22 -> 8.22

#define A15       26  // A15              1.30 -> 6.30
#define A14       25  // A14              1.13 -> 6.13
#define A13       24  // A13              1.12 -> 6.12
#define A12       12  // A12              2.1  -> 7.1
#define A11       11  // A11              2.2  -> 7.2
#define A10       10  // A10              2.0  -> 7.0
#define A9        9   // A9               2.11 -> 7.11
#define A8        8   // A8               2.16 -> 7.16
#define A7        7   // A7               2.17 -> 7.17
#define A6        6   // A6               2.10 -> 7.10
#define A5        5   // A5               4.8  -> 9.8
#define A4        4   // A4               4.6  -> 9.6
#define A3        3   // A3               4.5  -> 9.5
#define A2        2   // A2               4.4  -> 9.4
#define A1        1   // A1               1.2  -> 6.2
#define A0        0   // A0               1.3  -> 6.3

#define D7        40  // D7               1.20 -> 6.20
#define D6        39  // D6               1.29 -> 6.29
#define D5        38  // D5               1.28 -> 6.28
#define D4        37  // D4               2.19 -> 7.19
#define D3        36  // D3               2.18 -> 7.18
#define D2        35  // D2               2.28 -> 7.28
#define D1        34  // D1               2.29 -> 7.29
#define D0        33  // D0               4.7  -> 9.7

#define OE1       32  // OUTPUT ENABLE 1  2.12 -> 7.12
#define OE2       22  // OUTPUT ENABLE 2  1.24 -> 6.24
#define OE3       23  // OUTPUT ENABLE 3  1.25 -> 6.25

#define GPIO0     14  // GPIO 0           1.18 -> 6.18
#define GPIO1     15  // GPIO 1           1.19 -> 6.19
#define GPIO2     16  // GPIO 2           1.23 -> 6.23
#define GPIO3     17  // GPIO 3           1.22 -> 6.22

#define INT_SWB   21  // INTERVAL SWITCH  1.27 -> 6.27
#define STEP_SWB  20  // STEP SWITCH      1.26 -> 6.26
#define RS_SWB    19  // RUN/STOP SWITCH  1.16 -> 6.16

#endif