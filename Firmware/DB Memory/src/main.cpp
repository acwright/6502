#include <Arduino.h>

#include "devboard.h"
#include "utilities.h"
#include "constants.h"
#include "rom.h"

byte RAM[RAM_END - RAM_START + 1];

bool isEdge = false;
int cycles = 8;

void onChange();
void onHigh();
void onLow();
void initRAM();

//
// MAIN LOOPS
//

void setup() {
  initPins();
  initRAM();

  setAddrDirIn();
  setDataDirIn();

  digitalWriteFast(RDY, LOW);
  pinMode(RESB, OUTPUT);
  digitalWriteFast(RESB, LOW);
  delay(500);
  //attachInterrupt(PHI2, onChange, CHANGE);
  digitalWriteFast(RDY, HIGH);
  digitalWriteFast(RESB, HIGH);
  pinMode(RESB, INPUT);
}

void loop() {
  if (digitalReadFast(PHI2)) {
    digitalWriteFast(RDY, LOW);
    cycles--;

    if (cycles <= 0) {
      cycles = 8;
      onHigh();
    }
  } else {
    // isEdge = false;
    if (cycles == 8) {
      delayNanoseconds(20);
      setDataDirIn();
      digitalWriteFast(RDY, HIGH);
    }
  }
}

FASTRUN void onChange() {
  isEdge = true;
}

FASTRUN void onHigh() {
  bool readWrite   = digitalReadFast(RWB);
  word address     = readAddress();

  if (readWrite == HIGH) { // READING
    if ((address >= ROM_CODE) && (address <= ROM_END)) { // ROM
      setDataDirOut();
      writeData(ROM[address - ROM_START]);
    } else if ((address >= RAM_START) && (address <= RAM_END)) { // RAM
      setDataDirOut();
      writeData(RAM[address - RAM_START]);
    }
  } else { // WRITING
    delayNanoseconds(50);

    if ((address >= RAM_START) && (address <= RAM_END)) { // RAM
      RAM[address - RAM_START] = readData();
    }
  }
}

FASTRUN void onLow() {
  //delayNanoseconds(20);
  setDataDirIn();
}

//
// INITIALIZATION
//

void initRAM() {
  for (word a = 0x0000; a < (RAM_END - RAM_START + 1); a++) {
    RAM[a] = 0x00; 
  }
}