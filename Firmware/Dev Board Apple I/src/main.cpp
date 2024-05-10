////////////////////////////////////////////////////////////////////
// RetroShield 6502 for Teensy 3.5
// Apple 1
//
// 2019/09/13
// Version 0.1

// The MIT License (MIT)

// Copyright (c) 2019 Erturk Kocalar, 8Bitforce.com

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Date         Comments                                            Author
// -----------------------------------------------------------------------------
// 09/13/2019   Bring-up on Teensy 3.5.                             Erturk
// 01/11/2024   Added Teensy 4.1 support.                           Erturk

#include <Arduino.h>

////////////////////////////////////////////////////////////////////
// Options
//   outputDEBUG: Print memory access debugging messages.
////////////////////////////////////////////////////////////////////
#define outputDEBUG       0
#define DEBUG_SPEW_DELAY  1000

////////////////////////////////////////////////////////////////////
// BOARD DEFINITIONS
////////////////////////////////////////////////////////////////////

#include "memorymap.h"      // Memory Map (ROM, RAM, PERIPHERALS)
#include "portmap.h"        // Pin mapping to cpu
#include "setuphold.h"      // Delays required to meet setup/hold
#include "6821.h"           // 6821 Emulation


////////////////////////////////////////////////////////////////////
// Apple Cassette Interface (ACI)
// emulate data read and write.
////////////////////////////////////////////////////////////////////
// added by Lorenz Born

#define ACI_FLIP        0xC000     // output flip-flop 74LS74
#define ACI_FLIP_START  0xC000
#define ACI_FLIP_END    0xC0FF
#define ACI_TAPEIN      0xC081     // tape input logic 
bool    ACI_FLIP_FF     = false;   // TO_TAPE FlipFlop state.


unsigned long clock_cycle_count;
unsigned long clock_cycle_last;

word          uP_ADDR;
byte          uP_DATA;

void uP_assert_reset()
{
  // Drive RESET conditions
  digitalWriteFast(uP_RESET_N,  LOW);
  digitalWriteFast(uP_IRQ_N,    HIGH);
  digitalWriteFast(uP_NMI_N,    HIGH);
  digitalWriteFast(uP_RDY,      HIGH);
}

void uP_release_reset()
{
  // Drive RESET conditions
  digitalWriteFast(uP_RESET_N,  HIGH);
}

void uP_init()
{
  // Set directions for ADDR & DATA Bus.
 
  configure_PINMODE_ADDR();
  configure_PINMODE_DATA();

  pinMode(uP_RESET_N, OUTPUT);
  pinMode(uP_RW_N,    INPUT_PULLUP);
  pinMode(uP_RDY,     OUTPUT);
  pinMode(uP_SYNC_N,    INPUT);
  pinMode(uP_IRQ_N,   OUTPUT);
  pinMode(uP_NMI_N,   OUTPUT);
  pinMode(uP_CLK_E,   OUTPUT);
  pinMode(uP_BE,    OUTPUT);

  pinMode(23, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(32, OUTPUT);

  digitalWriteFast(23, HIGH); 
  digitalWriteFast(22, HIGH); 
  digitalWriteFast(32, HIGH); 

  digitalWriteFast(uP_BE, HIGH); 
  digitalWriteFast(uP_CLK_E, LOW); 
  uP_assert_reset();

  clock_cycle_count = 0;

  // Reduce ACI header from 10sec to quick
  aci_bin[(0xC171) - (0xC100)] = 0x06;     
}


////////////////////////////////////////////////////////////////////
// Processor Control Loop
////////////////////////////////////////////////////////////////////
// This is where the action is.
// it reads processor control signals and acts accordingly.
//
byte DATA_OUT;
byte DATA_IN;

inline __attribute__((always_inline))
void cpu_tick()
{ 
  
  CLK_HIGH;    // E goes high

  DELAY_FACTOR_H();
  
  uP_ADDR = ADDR();
  
  if (STATE_RW_N)	  
  //////////////////////////////////////////////////////////////////
  // HIGH = READ
  {
    // change DATA port to output to uP:
    // DATA_DIR_OUT();
    xDATA_DIR_OUT();
        
    // ROM?
    if ( (ROM_START <= uP_ADDR) && (uP_ADDR <= ROM_END) )
      DATA_OUT = rom_bin[ (uP_ADDR - ROM_START) ];
    else
    // check if reading from Apple Cassette Interface (ACI) in the
    // "C" block in PROM on ACI board
    // added Lorenz Born
    if ( (ACI_START <= uP_ADDR) && (uP_ADDR <= ACI_END) )
    {
      // DATA_OUT = pgm_read_byte_near(aci_bin + (uP_ADDR - ACI_START));
      DATA_OUT = aci_bin[uP_ADDR - ACI_START];
    }
    else
    // check if reading from Apple BASIC in the "E" block of memory
    if ( (BASIC_START <= uP_ADDR) && (uP_ADDR <= BASIC_END) )
      DATA_OUT = basic_bin[ (uP_ADDR - BASIC_START) ];
    else
    // RAM?
    if ( (RAM_START <= uP_ADDR) && (uP_ADDR <= RAM_END) )
      // Use Arduino RAM for stack/important stuff
      DATA_OUT = ( RAM[uP_ADDR - RAM_START] );
    else
    // 6821?
    if ( KBD <=uP_ADDR && uP_ADDR <= DSPCR )   
    {      
      // KBD?
      if (uP_ADDR == KBD)
      {
        if (regKBDCR & 0x02)
          // KBD register  
          {
            DATA_OUT = regKBD;
            regKBDCR = regKBDCR & 0x7F;    // clear IRQA bit upon read
          }
        else
          DATA_OUT = regKBDDIR;
      }
      else
      // KBDCR?
      if (uP_ADDR == KBDCR)
      {
        // KBDCR register
        DATA_OUT = regKBDCR;  
      }
      else
      // DSP?
      if (uP_ADDR == DSP)
      {
        if (regDSPCR & 0x02) 
          // DSP register  
          {
            DATA_OUT = regDSP;
            regDSPCR = regDSPCR & 0x7F;    // clear IRQA bit upon read
          }
        else
          DATA_OUT = regDSPDIR;
      }
      else
      // DSPCR?
      if (uP_ADDR == DSPCR)
      {
        // DSPCR register
        DATA_OUT = regDSPCR;  
      }   
      
    }
    // else
    // Access ACI_FLIP?
    // if ( (ACI_FLIP_START <= uP_ADDR) && (uP_ADDR <= ACI_FLIP_END) )
    // {
    //   ACI_FLIP_FF = not ACI_FLIP_FF;
    //   digitalWriteFast(uP_GPIO, ACI_FLIP_FF);
    // }

    // Start driving the databus out
    SET_DATA_OUT( DATA_OUT );
      
#if outputDEBUG
    // if (clock_cycle_count > 0x3065A)
    {
      char tmp[50];
      sprintf(tmp, "-- A=%0.4X D=%0.2X\n\r", uP_ADDR, DATA_OUT);
      Serial.write(tmp);
    }
#endif

    // 6502 can be stopped when CLK=HIGH.

    // Go slow when debugging
    if (outputDEBUG && digitalReadFast(uP_RESET_N))
    {
      if (0) { delay(DEBUG_SPEW_DELAY); }    
      if (1) { while(!Serial.available()); Serial.read(); }
    }
  } 
  else 
  //////////////////////////////////////////////////////////////////
  // R/W = LOW = WRITE
  {
    DATA_IN = xDATA_IN();
    
    // RAM?
    if ( (RAM_START <= uP_ADDR) && (uP_ADDR <= RAM_END) )
      // Use Arduino RAM for stack/important stuff
      RAM[uP_ADDR - RAM_START] = DATA_IN;
    else
    // 6821?
    if ( KBD <=uP_ADDR && uP_ADDR <= DSPCR )
    {
      // KBD?
      if (uP_ADDR == KBD)
      {
        if (regKBDCR & 0x02)
          // KBD register
          {  
            regKBD = DATA_IN;
          }
        else
          regKBDDIR = DATA_IN;
      }
      else
      // KBDCR?
      if (uP_ADDR == KBDCR)
      {
        // KBDCR register
        regKBDCR = DATA_IN & 0X7F;  
      }
      else
      // DSP?
      if (uP_ADDR == DSP)
      {
        if (regDSPCR & 0x02)
          // DSP register
          if (DATA_IN == 0x8D)
            Serial.write("\r\n");    // send CR / LF
          else
            Serial.write(regDSP = DATA_IN & 0x7F);
        else
          regDSPDIR = DATA_IN;  
      }
      else
      // DSPCR?
      if (uP_ADDR == DSPCR)
      {
        // DSPCR register
        regDSPCR = DATA_IN;  
      }
    }
    // else
    // // Access ACI_FLIP?
    // if ( (ACI_FLIP_START <= uP_ADDR) && (uP_ADDR <= ACI_FLIP_END) )
    // {
    //   ACI_FLIP_FF = not ACI_FLIP_FF;
    //   digitalWriteFast(uP_GPIO, ACI_FLIP_FF);
    // }    
    
#if outputDEBUG
    if (1) // (clock_cycle_count > 0x3065A)
    {
      char tmp[50];
      sprintf(tmp, "WR A=%0.4X D=%0.2X\n\r", uP_ADDR, DATA_IN);
      Serial.write(tmp);
    }
#endif
  }

  
  //////////////////////////////////////////////////////////////////
    
  // start next cycle
  CLK_LOW;    // E goes low
  DELAY_FACTOR_L();  

  xDATA_DIR_IN();
  
#if outputDEBUG
  clock_cycle_count++;
#endif
}

////////////////////////////////////////////////////////////////////
// Serial Event
////////////////////////////////////////////////////////////////////

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */

inline __attribute__((always_inline))
void serialEvent0() 
{
  if (Serial.available())
    if ((regKBDCR & 0x80) == 0x00)      // read serial byte only if we can set 6821 interrupt
    {
      cli();                            // stop interrupts while changing 6821 guts.
      // 6821 portA is available      
      int ch = toupper( Serial.read() );    // apple1 expects upper case
      regKBD = ch | 0x80;               // apple1 expects bit 7 set for incoming characters.
      regKBDCR = regKBDCR | 0x80;       // set 6821 interrupt
      sei();
    }
  return;
}

////////////////////////////////////////////////////////////////////
// Setup
////////////////////////////////////////////////////////////////////
void setup() 
{
  Serial.begin(0);
  while (!Serial);

  // Serial.write(27);       // ESC command
  // Serial.print("[2J");    // clear screen command
  // Serial.write(27);
  // Serial.print("[H");
  Serial.println("\n");
  Serial.println("Configuration:");
  Serial.println("==============");
  print_teensy_version();
  Serial.print("Debug:      "); Serial.println(outputDEBUG, HEX);
  Serial.print("SRAM Size:  "); Serial.print(RAM_END - RAM_START + 1, DEC); Serial.println(" Bytes");
  Serial.print("SRAM_START: 0x"); Serial.println(RAM_START, HEX); 
  Serial.print("SRAM_END:   0x"); Serial.println(RAM_END, HEX); 
  Serial.println("");
  Serial.println("=======================================================");
  Serial.println("> WOZ Monitor, Integer BASIC, Apple Cassette Interface");
  Serial.println("> by Steve Wozniak");
  Serial.println("=======================================================");
  Serial.println("Notes:");
  Serial.println("1) Enter E000R to start Apple BASIC.");  
  Serial.println("2) Cassette Interface added to RetroShield 6502 by Lorenz Born.");  
  Serial.println("   ACI Audio Out is available on GPIO output."); 
  Serial.println("   Somebody should write code to handle incoming audio :)"); 
   

  // Initialize processor GPIO's
  uP_init();
  m6821_init();

  // Reset processor for 25 cycles
  uP_assert_reset();
  for (int i=0; i<25; i++) cpu_tick();
  uP_release_reset();

  Serial.println("\n");
}


////////////////////////////////////////////////////////////////////
// Loop()
////////////////////////////////////////////////////////////////////

void loop()
{
  byte i = 0;
  
  // Loop forever
  //  
  while(1)
  {

    cpu_tick();

    // Check serial events but not every cycle.
    // Watch out for clock mismatch (cpu tick vs serialEvent counters are /128)
    i++;
    if (i == 0) serialEvent0();
    if (i == 0) Serial.flush();
  }
}
