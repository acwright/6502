#include <Arduino.h>
#include <XModem.h>
#include "constants.h"
#include "pins.h"

void onCommand(char command);
bool receiveData(void *blk_id, size_t idSize, byte *data, size_t dataSize);

void info();
void reset();
void halt();
void resume();

void initPins();

void writeAddress(uint16_t address);
void writeData(uint8_t data);

bool isLoading = false;
bool isHalted = false;

XModem xmodem;

uint8_t receivedData[128];

//
// MAIN
//

void setup() {
  Serial.begin(9600);
  while (!Serial);

  xmodem.begin(Serial, XModem::ProtocolType::XMODEM);
  xmodem.setRecieveBlockHandler(receiveData);
  
  initPins();

  info();
}

void loop() {
  if (isLoading) {
    //if (!isHalted) { halt(); }
    Serial.print("Waiting for data..");
    xmodem.receive();
    Serial.println();
    for (unsigned int i = 0; i < 128; i++) {
      Serial.print(receivedData[i], HEX);
      Serial.print("");
    }
    Serial.println();
    //if (isHalted) { resume(); }
    isLoading = false;
  } else if (Serial.available()) {
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
    case 'l':
    case 'L':
      isLoading = true;
      break;
  }
}

bool receiveData(void *blk_id, size_t idSize, byte *data, size_t dataSize) {
  // byte id = *((byte *) blk_id);

  for(unsigned int i = 0; i < dataSize; ++i) {
    receivedData[i] = data[i];
  }

  return true; // or return false to stop transfer early
}

//
// METHODS
//

void info() {
  Serial.println();
  Serial.println("eeee  eeeee eeeeee eeee   88888 888888   8                                  ");                        
  Serial.println("8  8  8     8    8    8   8   8 8    e   8     eeeee eeeee eeeee eeee eeeee ");
  Serial.println("8     8eeee 8    8    8   8e  8 8e       8e    8  88 8   8 8   8 8    8   8 ");
  Serial.println("8eeee     8 8    8 eee8   88  8 88       88    8   8 8eee8 8e  8 8eee 8eee8e");
  Serial.println("8   8     8 8    8 8      88  8 88   e   88    8   8 88  8 88  8 88   88   8");
  Serial.println("8eee8 eeee8 8eeee8 8eee   88  8 88eee8   88eee 8eee8 88  8 88ee8 88ee 88   8");
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

void halt() {
  Serial.print("Halting the 6502... ");

  while (!digitalRead(SYNC));

  pinMode(RDY, OUTPUT);
  digitalWrite(RDY, LOW);

  Serial.println("Done");

  Serial.print("Disabling the bus... ");

  pinMode(BE, OUTPUT);
  digitalWrite(BE, LOW);

  isHalted = true;

  Serial.println("Done");
}

void resume() {
  Serial.print("Re-enabling the bus... ");

  digitalWrite(BE, HIGH);
  pinMode(BE, INPUT);

  Serial.println("Done");

  Serial.print("Resuming the 6502... ");

  digitalWrite(RDY, HIGH);
  pinMode(RDY, INPUT);

  isHalted = false;

  Serial.println("Done");
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