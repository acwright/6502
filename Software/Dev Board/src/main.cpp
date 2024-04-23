#include <Arduino.h>
#include <TimerOne.h>
#include <Bounce2.h>
#include <climits>

#include "pins.h"

#define RAM_START   0x0000
#define RAM_END     0x7FFF
#define IO_START    0x8000
#define IO_END      0x9FFF
#define ROM_START   0xA000
#define ROM_END     0xFFFF

#define PERIOD          1 // 1 uS
#define FREQ_MIN        1 // 1 Hz
#define FREQ_MAX        1000000 // 1 Mhz
#define FREQ_MAX_DEBUG  2048 // 2048 Hz
#define DELAY(freq)     (unsigned long) ((((1.0 / freq) * 1000000) / PERIOD) / 2)

#define DEBOUNCE  5 // 5 milliseconds

#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')

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

void initRAM();
void initROM();
void initPins();
void initButtons();
void disableOutputs();
void enableOutputs();
void setAddrDirIn();
void setDataDirIn();
void setDataDirOut();

word readAddress();
byte readData();
void writeData(byte d);

void reset();
void debug();
void toggleRunStop();
void toggleDebug();
void changeFrequency();
void step();
void info();

unsigned int freq = 1;
unsigned int ticks = 0;
bool isDebugging = true;
bool isRunning = false;

word address = 0;
byte data = 0;
byte readWrite = HIGH;

void setup() {
  initPins();
  initButtons();
  initRAM();
  initROM();

  pinMode(PHI2, OUTPUT);
  digitalWriteFast(PHI2, HIGH);

  enableOutputs();

  Timer1.initialize(PERIOD);
  Timer1.attachInterrupt(onTick);

  Serial.begin(9600);

  delay(500);

  Serial.println();
  Serial.println("eeee  eeeee eeeeee eeee   888888               888888                            ");                        
  Serial.println("8  8  8     8    8    8   8    8 eeee ee   e   8    8   eeeee eeeee eeeee  eeeee ");
  Serial.println("8     8eeee 8    8    8   8e   8 8    88   8   8eeee8ee 8  88 8   8 8   8  8   8 ");
  Serial.println("8eeee     8 8    8 eee8   88   8 8eee 88  e8   88     8 8   8 8eee8 8eee8e 8e  8 ");
  Serial.println("8   8     8 8    8 8      88   8 88    8  8    88     8 8   8 88  8 88   8 88  8 ");
  Serial.println("8eee8 eeee8 8eeee8 8eee   88eee8 88ee  8ee8    88eeeee8 8eee8 88  8 88   8 88ee8 ");
  Serial.println();
  Serial.println("---------------------------------");
  Serial.println("| Created by A.C. Wright © 2024 |");
  Serial.println("---------------------------------");

  info();
}

void loop() {
  stepButton.update();
  runStopButton.update();
  intButton.update();

  if (stepButton.pressed()) {
    step();
  }
  if (runStopButton.pressed()) {
    toggleRunStop();
  }
  if (intButton.pressed()) {
    changeFrequency();
  }

  if (Serial.available()) 
  {
    switch (Serial.read())
    {
      case 'r':
      case 'R':
        toggleRunStop();
        break;
      case 's':
      case 'S':
        step();
        break;
      case 't':
      case 'T':
        reset();
        break;
      case 'f':
      case 'F':
        changeFrequency();
        break;
      case 'd':
      case 'D':
        toggleDebug();
        break;
      case 'i':
      case 'I':
        info();
        break;
    }
  }
}

void onTick()
{
  if (ticks == DELAY(freq)) {
    if (isRunning) {
      digitalReadFast(PHI2) ? onLow() : onHigh();
    } else if (!digitalReadFast(PHI2)) { 
      onHigh();
    }

    ticks = 0;
  } else {
    ticks++;
  }
}

void onLow() {
  digitalWriteFast(PHI2, LOW);

  setDataDirIn();

  if (readWrite == LOW) { // WRITING
    data = readData();

    if ((RAM_START <= address) && (address <= RAM_END)) {
      RAM[address] = data;
    } else if ((IO_START <= address) && (address <= IO_END)) {
      // TODO: Handle IO
    }
  }

  if (freq <= FREQ_MAX_DEBUG && isDebugging) {
    debug();
  }
}

void onHigh() {
  digitalWriteFast(PHI2, HIGH);

  address = readAddress();
  readWrite = digitalReadFast(RWB);

  if (readWrite == HIGH) { // READING
    setDataDirOut();

    if ((ROM_START <= address) && (address <= ROM_END)) { // ROM
      data = ROM[address - ROM_START];

      writeData(data);
    } else if ((RAM_START <= address) && (address <= RAM_END)) { // RAM
      data = RAM[address - RAM_START];

      writeData(data);
    } else if ((IO_START <= address) && (address <= IO_END)) {
      // TODO: Handle IO
    }
  }
}

void reset() {
  pinMode(RESB, OUTPUT);
  digitalWriteFast(RESB, LOW);
  digitalWriteFast(PHI2, LOW);
  delay(100);
  digitalWriteFast(PHI2, HIGH);
  delay(100);
  digitalWriteFast(PHI2, LOW);
  delay(100);
  digitalWriteFast(PHI2, HIGH);
  delay(100);
  digitalWriteFast(RESB, HIGH);
  pinMode(RESB, INPUT);
}

void changeFrequency() {
  if (freq < FREQ_MAX) {
    freq *= 2;
  } else {
    freq = FREQ_MIN;
  }
  ticks = 0;

  Serial.print("Frequency: ");
  Serial.print(freq);
  Serial.println(" Hz");
}

void toggleRunStop() {
  if (isRunning) {
    isRunning = false;
    Serial.println("Stopped");
  } else {
    isRunning = true;
    Serial.println("Running…");
  }
}

void toggleDebug() {
  if (isDebugging) {
    isDebugging = false;
  } else {
    isDebugging = true;
  }
  Serial.print("Debug: ");
  Serial.println(isDebugging ? "Enabled" : "Disabled");
}

void step() {
  onLow();
  if (!isDebugging) {
    Serial.print("Tick…  ");
  }
  delay(100);
  onHigh();
  if (!isDebugging) {
    Serial.println("Tock…");
  }
}

void info() {
  Serial.println();
  Serial.print("Frequency: ");
  Serial.print(freq);
  Serial.println(" Hz");
  Serial.print("Debug: ");
  Serial.println(isDebugging ? "Enabled" : "Disabled");
  Serial.println();
  Serial.println("(R)un / Stop | (S)tep | Rese(T) | Change (F)requency | Toggle (D)ebug | (I)nfo");
  Serial.println();
}

void debug() {
  char output[64];
  
  sprintf(
    output, 
    "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c  %c%c%c%c%c%c%c%c  %01X  0x%04X  0x%02X  %c", 
    BYTE_TO_BINARY((address >> 8) & 0xFF),
    BYTE_TO_BINARY(address & 0xFF), 
    BYTE_TO_BINARY(data),
    readWrite, 
    address, 
    data, 
    readWrite ? 'r' : 'W'
  );

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
  pinMode(OE1, OUTPUT);
  pinMode(OE2, OUTPUT);
  pinMode(OE3, OUTPUT);

  disableOutputs();

  pinMode(RESB, INPUT);
  pinMode(SYNC, INPUT);
  pinMode(RWB, INPUT);
  pinMode(RDY, INPUT);
  pinMode(BE, INPUT);
  pinMode(PHI2, OUTPUT);
  pinMode(NMIB, INPUT);
  pinMode(IRQB, INPUT);

  pinMode(INT_SWB, INPUT);
  pinMode(STEP_SWB, INPUT);
  pinMode(RS_SWB, INPUT);

  pinMode(GPIO_0, INPUT);
  pinMode(GPIO_1, INPUT);
  pinMode(GPIO_2, INPUT);
  pinMode(GPIO_3, INPUT);

  setAddrDirIn();
  setDataDirIn();
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