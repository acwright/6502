#ifndef _DEVBOARD_H
#define _DEVBOARD_H

#include <Arduino.h>

#ifdef DEVBOARD_0
#define RESB      28  // RESET          
#define SYNC      30  // SYNC           
#define RWB       27  // READ/WRITE     
#define RDY       29  // RDY            
#define BE        18  // BE             
#define PHI2      13  // PHI2           
#define NMIB      41  // NMI            
#define IRQB      31  // IRQ            

#define A15       26  // A15            
#define A14       25  // A14            
#define A13       24  // A13            
#define A12       12  // A12            
#define A11       11  // A11            
#define A10       10  // A10            
#define A9        9   // A9             
#define A8        8   // A8             
#define A7        7   // A7             
#define A6        6   // A6             
#define A5        5   // A5             
#define A4        4   // A4             
#define A3        3   // A3             
#define A2        2   // A2             
#define A1        1   // A1             
#define A0        0   // A0             

#define D7        40  // D7             
#define D6        39  // D6             
#define D5        38  // D5             
#define D4        37  // D4             
#define D3        36  // D3             
#define D2        35  // D2             
#define D1        34  // D1             
#define D0        33  // D0             

#define OE1       32  // OUTPUT ENABLE 1
#define OE2       22  // OUTPUT ENABLE 2
#define OE3       23  // OUTPUT ENABLE 3

#define GPIO0     14  // GPIO 0         
#define GPIO1     15  // GPIO 1         
#define GPIO2     16  // GPIO 2         
#define GPIO3     17  // GPIO 3         

#define CLK_SWB   21  // CLOCK SWITCH
#define STEP_SWB  20  // STEP SWITCH    
#define RS_SWB    19  // RUN/STOP SWITCH
#endif

#ifdef DEVBOARD_1
#define RESB      38  // RESET          
#define SYNC      37  // SYNC           
#define RWB       32  // READ/WRITE     
#define RDY       36  // RDY            
#define BE        35  // BE             
#define PHI2      13  // PHI2           
#define NMIB      34  // NMI            
#define IRQB      33  // IRQ            

#define A15       31  // A15            
#define A14       30  // A14            
#define A13       25  // A13            
#define A12       24  // A12            
#define A11       12  // A11            
#define A10       11  // A10            
#define A9        10  // A9             
#define A8        9   // A8             
#define A7        8   // A7             
#define A6        7   // A6             
#define A5        6   // A5             
#define A4        5   // A4             
#define A3        4   // A3             
#define A2        3   // A2             
#define A1        2   // A1             
#define A0        0   // A0             

#define D7        16  // D7             
#define D6        17  // D6             
#define D5        41  // D5             
#define D4        40  // D4             
#define D3        15  // D3             
#define D2        14  // D2             
#define D1        18  // D1             
#define D0        19  // D0             

#define MOSI1     26  // MOSI1        
#define MISO1     1   // MISO1         
#define SCK1      27  // SCK1         
#define CS0       28  // CS0
#define CS1       29  // CS1          
#define CS2       39  // CS2 

#define RESET_SWB 23  // RESET SWITCH
#define CLK_SWB   22  // CLOCK SWITCH
#define STEP_SWB  21  // STEP SWITCH    
#define RS_SWB    20  // RUN/STOP SWITCH
#endif

#endif