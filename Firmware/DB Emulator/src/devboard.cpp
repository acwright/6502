#include "devboard.h"

void initPins() {
  pinMode(RESB, OUTPUT);
  pinMode(IRQB, INPUT);
  pinMode(NMIB, INPUT);
  pinMode(RDY, INPUT);
  pinMode(BE, INPUT);
  pinMode(SYNC, INPUT);
  pinMode(RWB, INPUT);
  pinMode(PHI2, OUTPUT);

  pinMode(CLK_SWB, INPUT);
  pinMode(STEP_SWB, INPUT);
  pinMode(RS_SWB, INPUT);

  pinMode(GPIO0, INPUT);
  pinMode(GPIO1, INPUT);
  pinMode(GPIO2, INPUT);
  pinMode(GPIO3, INPUT);
  
  digitalWriteFast(RESB, HIGH);
  digitalWriteFast(PHI2, HIGH);

  pinMode(OE1, OUTPUT);
  pinMode(OE2, OUTPUT);
  pinMode(OE3, OUTPUT);

  digitalWriteFast(OE1, LOW);
  digitalWriteFast(OE2, HIGH);
  digitalWriteFast(OE3, HIGH);
}