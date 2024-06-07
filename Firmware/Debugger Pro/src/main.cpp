#include <Arduino.h>
#include <TimerOne.h>
#include <Bounce2.h>
#include <Ram.h>
#include <Io.h>
#include <Rom.h>

#if DEVBOARD
#include "devboard.h"
#elif RETROSHIELD
#include "retroshield.h"
#endif
#include "utilities.h"
#include "memory.h"

#define TIMER_PERIOD              1 // 1 uS
#define FREQS                     (String[20])  { "1 Hz", "2 Hz", "4 Hz", "8 Hz", "16 Hz", "32 Hz", "64 Hz", "122 Hz", "244 Hz", "488 Hz", "976 Hz", "1.9 kHz", "3.9 kHz", "7.8 kHz", "15.6 kHz", "31.2 kHz", "62.5 kHz", "125 kHz", "250 kHz", "500 kHz" }
#define FREQ_DELAYS               (int[20])     { 250000, 125000, 62500, 31250, 15625, 7812, 3906, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1, 0 }

#define DEBOUNCE        5          // 5 milliseconds

#define VERSION         "1.0.0"

Button intButton = Button();
Button stepButton = Button();
Button runStopButton = Button();

void onTick();

void initPins();

void setAddrDirIn();
void setDataDirIn();
void setDataDirOut();
word readAddress();
byte readData();
void writeData(byte d);

unsigned int freqIndex = 19;      // 500 kHz
unsigned int freqCounter = 0;

word address = 0;
byte data = 0;
byte readWrite = HIGH;

//
// MAIN LOOPS
//

void setup() {
  intButton.attach(INT_SWB, INPUT_PULLUP);
  intButton.interval(DEBOUNCE);
  intButton.setPressedState(LOW);

  stepButton.attach(STEP_SWB, INPUT_PULLUP);
  stepButton.interval(DEBOUNCE);
  stepButton.setPressedState(LOW);

  runStopButton.attach(RS_SWB, INPUT_PULLUP);
  runStopButton.interval(DEBOUNCE);
  runStopButton.setPressedState(LOW);

  initPins();

  RAM.init();
  IO.init();
  ROM.init();

  Timer1.initialize(TIMER_PERIOD);
  Timer1.attachInterrupt(onTick);

  Serial.begin(9600);
}

void loop() {
  intButton.update();
  stepButton.update();
  runStopButton.update();

  if (intButton.pressed()) {
    // Handle int button
  }
  if (stepButton.pressed()) {
    // Handle step button
  }
  if (runStopButton.pressed()) {
    // Handle run/stop button
  }

  if (Serial.available()) 
  {
    // Handle Serial
    // Serial.read()
  }
}

//
// EVENTS
//

void onTick()
{
  // if (freqCounter == 0) {
  //   if (isRunning) {
  //     digitalReadFast(PHI2) ? onLow() : onHigh();
  //   }

  //   freqCounter = FREQ_DELAYS[freqIndex];
  // } else {
  //   freqCounter--;
  // }

  // if (stepCounter == 0) {
  //   if (isLogging) {
  //     log();
  //     isLogging = false;
  //   }

  //   if (isStepping) {
  //     if (digitalReadFast(PHI2)) {
  //       onLow();

  //       isStepping = false; // We are done stepping...
  //       isLogging = true;   // Log on next tick
  //     } else {
  //       onHigh();
  //     }
  //   }

  //   stepCounter = FREQ_DELAYS[2]; // Use 4 Hz for step counter
  // } else {
  //   stepCounter--;
  // }
}

//
// INITIALIZATION
//

#if DEVBOARD
void initPins() {
  pinMode(RESB, OUTPUT);
  pinMode(IRQB, OUTPUT);
  pinMode(NMIB, OUTPUT);
  pinMode(RDY, OUTPUT);
  pinMode(BE, OUTPUT);
  pinMode(PHI2, OUTPUT);
  
  pinMode(SYNC, INPUT);
  pinMode(RWB, INPUT);

  pinMode(OE1, OUTPUT);
  pinMode(OE2, OUTPUT);
  pinMode(OE3, OUTPUT);

  pinMode(INT_SWB, INPUT);
  pinMode(STEP_SWB, INPUT);
  pinMode(RS_SWB, INPUT);

  pinMode(GPIO0, INPUT);
  pinMode(GPIO1, INPUT);
  pinMode(GPIO2, INPUT);
  pinMode(GPIO3, INPUT);

  setAddrDirIn();
  setDataDirIn();

  digitalWriteFast(RESB, HIGH);
  digitalWriteFast(IRQB, HIGH);
  digitalWriteFast(NMIB, HIGH);
  digitalWriteFast(RDY, HIGH);
  digitalWriteFast(BE, HIGH);
  digitalWriteFast(PHI2, LOW);

  digitalWriteFast(OE1, HIGH);
  digitalWriteFast(OE2, HIGH);
  digitalWriteFast(OE3, HIGH);
}
#elif RETROSHIELD
void initPins() {
  pinMode(RESB, OUTPUT);
  pinMode(IRQB, OUTPUT);
  pinMode(NMIB, OUTPUT);
  pinMode(RDY, OUTPUT);
  pinMode(SOB, OUTPUT);
  pinMode(PHI2, OUTPUT);

  pinMode(RWB, INPUT);

  pinMode(INT_SWB, INPUT);
  pinMode(STEP_SWB, INPUT);
  pinMode(RS_SWB, INPUT);

  setAddrDirIn();
  setDataDirIn();

  digitalWriteFast(RESB, HIGH);
  digitalWriteFast(IRQB, HIGH);
  digitalWriteFast(NMIB, HIGH);
  digitalWriteFast(RDY, HIGH);
  digitalWriteFast(SOB, HIGH);
  digitalWriteFast(PHI2, LOW);
}
#endif

void initButtons() {
  intButton.attach(INT_SWB, INPUT_PULLUP);
  intButton.interval(DEBOUNCE);
  intButton.setPressedState(LOW);

  stepButton.attach(STEP_SWB, INPUT_PULLUP);
  stepButton.interval(DEBOUNCE);
  stepButton.setPressedState(LOW);

  runStopButton.attach(RS_SWB, INPUT_PULLUP);
  runStopButton.interval(DEBOUNCE);
  runStopButton.setPressedState(LOW);
}

//
// UTILITIES
//

void setAddrDirIn() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);
  pinMode(A12, INPUT);
  pinMode(A13, INPUT);
  pinMode(A14, INPUT);
  pinMode(A15, INPUT);
}

void setDataDirIn() {
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D7, INPUT);
}

void setDataDirOut() {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
}

word readAddress() {
  word a = 0;

  a = a | digitalReadFast(A15);
  a = a << 1;
  a = a | digitalReadFast(A14);
  a = a << 1;
  a = a | digitalReadFast(A13);
  a = a << 1;
  a = a | digitalReadFast(A12);
  a = a << 1;
  a = a | digitalReadFast(A11);
  a = a << 1;
  a = a | digitalReadFast(A10);
  a = a << 1;
  a = a | digitalReadFast(A9);
  a = a << 1;
  a = a | digitalReadFast(A8);
  a = a << 1;
  a = a | digitalReadFast(A7);
  a = a << 1;
  a = a | digitalReadFast(A6);
  a = a << 1;
  a = a | digitalReadFast(A5);
  a = a << 1;
  a = a | digitalReadFast(A4);
  a = a << 1;
  a = a | digitalReadFast(A3);
  a = a << 1;
  a = a | digitalReadFast(A2);
  a = a << 1;
  a = a | digitalReadFast(A1);
  a = a << 1;
  a = a | digitalReadFast(A0);
  
  return a;
}

byte readData() {
  byte d = 0;

  d = d | digitalReadFast(D7);
  d = d << 1;
  d = d | digitalReadFast(D6);
  d = d << 1;
  d = d | digitalReadFast(D5);
  d = d << 1;
  d = d | digitalReadFast(D4);
  d = d << 1;
  d = d | digitalReadFast(D3);
  d = d << 1;
  d = d | digitalReadFast(D2);
  d = d << 1;
  d = d | digitalReadFast(D1);
  d = d << 1;
  d = d | digitalReadFast(D0);

  return d;
}

void writeData(byte d) {
  digitalWriteFast(D0, d & 1);
  d = d >> 1;
  digitalWriteFast(D1, d & 1);
  d = d >> 1;
  digitalWriteFast(D2, d & 1);
  d = d >> 1;
  digitalWriteFast(D3, d & 1);
  d = d >> 1;
  digitalWriteFast(D4, d & 1);
  d = d >> 1;
  digitalWriteFast(D5, d & 1);
  d = d >> 1;
  digitalWriteFast(D6, d & 1);
  d = d >> 1;
  digitalWriteFast(D7, d & 1);
}