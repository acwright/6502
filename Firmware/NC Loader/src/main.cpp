#include <Arduino.h>
#include "constants.h"
#include "pins.h"

void onCommand(char command);

void info();
void reset();
void load();

void initPins();

void writeAddress(uint16_t address);
void writeData(uint8_t data);

//
// MAIN
//

void setup() {
  Serial.begin(115200);

  while (!Serial);
  
  initPins();

  info();
}

void loop() {
  if (Serial.available()) {
    onCommand(Serial.read());
  }
}

//
// EVENTS
//

void onCommand(char command) {
  switch (command) {
    case 'f':
    case 'F':
      info();
      break;
    case 't':
    case 'T':
      reset();
      break;
    case 'u':
    case 'U':
      load();
      break;
  }
}

//
// METHODS
//

void info() {
  Serial.println();
  Serial.println("eeee  eeeee eeeeee eeee   88888 888888   8                                    ");                        
  Serial.println("8  8  8     8    8    8   8   8 8    e   8     eeeee eeeee eeeee eeee eeeee   ");
  Serial.println("8     8eeee 8    8    8   8e  8 8e       8e    8  88 8   8 8   8 8    8   8   ");
  Serial.println("8eeee     8 8    8 eee8   88  8 88       88    8   8 8eee8 8e  8 8eee 8eee8e  ");
  Serial.println("8   8     8 8    8 8      88  8 88   e   88    8   8 88  8 88  8 88   88   8  ");
  Serial.println("8eee8 eeee8 8eeee8 8eee   88  8 88eee8   88eee 8eee8 88  8 88ee8 88ee 88   8 ");
  Serial.println();
  Serial.println("6502 NC Loader | Version: 1.0");
  Serial.println();
  Serial.println("---------------------------------");
  Serial.println("| Created by A.C. Wright © 2025 |");
  Serial.println("---------------------------------");
  Serial.println();
  Serial.println("------------------------------------------------------");
  Serial.println("| (L)oad        | Rese(T)          | In(F)o          |");
  Serial.println("------------------------------------------------------");
  Serial.println();
}

void reset() {
  Serial.print("Resetting... ");

  pinMode(RESB, OUTPUT);
  digitalWrite(RESB, LOW);
  delay(100);
  digitalWrite(RESB, HIGH);
  delay(100);
  pinMode(RESB, INPUT);
  
  Serial.println("Done");
}

void load() {
  
}

//
// INITIALIZATION
//

void initPins() {
  pinMode(SYNC, INPUT);
  pinMode(RDY, INPUT);
  pinMode(PHI2, INPUT);
  pinMode(RWB, INPUT);
  pinMode(BE, INPUT);
  pinMode(RESB, INPUT);

  pinMode(ADATA, OUTPUT);
  pinMode(ACLK, OUTPUT);
  pinMode(AOEB, OUTPUT);

  digitalWrite(ADATA, HIGH);
  digitalWrite(ACLK, HIGH);
  digitalWrite(AOEB, HIGH);

  pinMode(DDATA, OUTPUT);
  pinMode(DCLK, OUTPUT);
  pinMode(DOEB, OUTPUT);

  digitalWrite(DDATA, HIGH);
  digitalWrite(DCLK, HIGH);
  digitalWrite(DOEB, HIGH);
}

//
// UTILITIES
//

void writeAddress(uint16_t address) {

}

void writeData(uint8_t data) {

}