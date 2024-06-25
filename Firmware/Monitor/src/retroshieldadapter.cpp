#include "retroshieldadapter.h"

#if RETROSHIELD_ADAPTER
void initPins() {
  pinMode(RESB, OUTPUT);
  pinMode(IRQB, INPUT);
  pinMode(NMIB, INPUT);
  pinMode(RDY, INPUT);
  pinMode(SYNC, INPUT);
  pinMode(RWB, INPUT);
  pinMode(PHI2, OUTPUT);

  pinMode(INT_SWB, INPUT);
  pinMode(STEP_SWB, INPUT);
  pinMode(RS_SWB, INPUT);

  pinMode(GPIO0, INPUT);

  digitalWriteFast(RESB, HIGH);
  digitalWriteFast(PHI2, HIGH);
}
#endif