#ifndef _PINS_H
#define _PINS_H            

#define SYNC      2
#define RDY       3
#define PHI2      9
#define RWB       10
#define BE        11
#define RESB      12

#define ADATA     4
#define ACLK      5
#define AOEB      7

#define DDATA     13
#define DCLK      6
#define DOEB      8

#ifdef NANO_EVERY
#define D7        21
#define D6        20
#define D5        19
#define D4        18
#define D3        17
#define D2        16
#define D1        15
#define D0        14
#endif

#endif