#include <Arduino.h>
#include <TimerOne.h>
#include <Bounce2.h>
#include <climits>

#include "pins.h"

#define RAM_START   0x0000
#define RAM_END     0x7FFF
#define ROM_START   0x8000
#define ROM_END     0xFFFF

#define PERIOD    1 // 1 uS
#define FREQ      1 // 0 - 1000000 Hz
#define DELAY     (unsigned long) (FREQ > 0 ? (((1.0 / FREQ) * 1000000) / PERIOD) / 2 : ULONG_MAX)

#define DEBOUNCE  5 // 5 milliseconds

const byte ADDR_PINS[] = { A15, A14, A13, A12, A11, A10, A9, A8, A7, A6, A5, A4, A3, A2, A1, A0 };
const byte DATA_PINS[] = { D7, D6, D5, D4, D3, D2, D1, D0 };

byte RAM[RAM_END-RAM_START+1];
byte ROM[ROM_END-ROM_START+1];

Button intButton = Button();
Button stepButton = Button();
Button runStopButton = Button();

void onTick();
void onLow();
void onHigh();

void initPins();
void initButtons();
void initRAM();
void initROM();
void disableOutputs();
void enableOutputs();
void setAddrDirIn();
void setDataDirIn();
void setDataDirOut();
word readAddress();
byte readData();
void writeData(byte d);

void debug();

unsigned int ticks = 0;
bool isRunning = false;

void setup() {
  disableOutputs();

  initPins();
  initButtons();
  initRAM();
  initROM();

  enableOutputs();

  pinMode(PHI2, OUTPUT);
  digitalWriteFast(PHI2, LOW);

  Timer1.initialize(PERIOD);
  Timer1.attachInterrupt(onTick);

  Serial.begin(9600);
}

void loop() {
  stepButton.update();
  runStopButton.update();

  if (stepButton.pressed()) {
    onHigh();
    onLow();
  }
  if (runStopButton.pressed()) {
    isRunning = !isRunning;
  }
}

void onTick()
{
  if (ticks == DELAY) {
    if (isRunning) {
      digitalReadFast(PHI2) ? onLow() : onHigh();
    } else if (digitalReadFast(PHI2)) { 
      onLow();
    }

    ticks = 0;
  } else {
    ticks++;
  }
}

void onLow() {
  digitalWriteFast(PHI2, LOW);

  // Data hold time?
  // DELAY_FACTOR_L();

  setDataDirIn();
}

void onHigh() {
  digitalWriteFast(PHI2, HIGH);

  // Address setup time?
  // DELAY_FACTOR_H();

  word address = readAddress();

  if (digitalReadFast(RWB)) { // READ
    setDataDirOut();

    if ((ROM_START <= address) && (address <= ROM_END)) { // ROM
      writeData(ROM[address - ROM_START]);
    } else if ((RAM_START <= address) && (address <= RAM_END)) { // RAM
      writeData(RAM[address - RAM_START]);
    } else {
      // Do nothing for now...
    }
  } else { // WRITE
    if ((RAM_START <= address) && (address <= RAM_END)) {
      RAM[address] = readData();
    }
  }

  debug();
}

void debug() {
  char output[24];

  unsigned int address = 0;
  for (int n = 0; n < 16; n += 1) {
    int bit = digitalReadFast(ADDR_PINS[n]) ? 1 : 0;
    Serial.print(bit);
    address = (address << 1) + bit;
  }
  
  Serial.print("   ");
  
  unsigned int data = 0;
  for (int n = 0; n < 8; n += 1) {
    int bit = digitalReadFast(DATA_PINS[n]) ? 1 : 0;
    Serial.print(bit);
    data = (data << 1) + bit;
  }

  unsigned int rw = digitalReadFast(RWB);

  sprintf(output, "   %04x  %01x  %c %02x", address, rw, rw ? 'r' : 'W', data);
  Serial.println(output); 
}

void initRAM() {
  unsigned int i;
  
  for (i = 0; i < sizeof(RAM) ; i++){ 
    RAM[i] = 0x00; 
  }
}

void initROM() {
  unsigned int i;

  for (i = 0; i < sizeof(ROM) ; i++){ 
    ROM[i] = 0xEA; 
  }
}

void initPins() {
  pinMode(RESB, INPUT);
  pinMode(SYNC, INPUT);
  pinMode(RWB, INPUT);
  pinMode(RDY, INPUT);
  pinMode(BE, INPUT);
  pinMode(PHI2, INPUT);
  pinMode(NMIB, INPUT);
  pinMode(IRQB, INPUT);

  pinMode(OE1, OUTPUT);
  pinMode(OE2, OUTPUT);
  pinMode(OE3, OUTPUT);

  pinMode(INT_SWB, INPUT);
  pinMode(STEP_SWB, INPUT);
  pinMode(RS_SWB, INPUT);

  pinMode(GPIO_0, INPUT);
  pinMode(GPIO_1, INPUT);
  pinMode(GPIO_2, INPUT);
  pinMode(GPIO_3, INPUT);

  setAddrDirIn();
  setDataDirIn();
  
  disableOutputs();
}

void initButtons() {
  intButton.attach(INT_SWB, INPUT);
  intButton.interval(DEBOUNCE);
  intButton.setPressedState(LOW);

  stepButton.attach(STEP_SWB, INPUT);
  stepButton.interval(DEBOUNCE);
  stepButton.setPressedState(LOW);

  runStopButton.attach(RS_SWB, INPUT);
  runStopButton.interval(DEBOUNCE);
  runStopButton.setPressedState(LOW);
}

void disableOutputs() {
  digitalWriteFast(OE1, LOW);
  digitalWriteFast(OE2, LOW);
  digitalWriteFast(OE3, LOW);
}

void enableOutputs() {
  digitalWriteFast(OE1, HIGH);
  digitalWriteFast(OE2, HIGH);
  digitalWriteFast(OE3, HIGH);
}

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