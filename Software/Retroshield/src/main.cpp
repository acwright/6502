#include <Arduino.h>
#include <TimerOne.h>
#include <Bounce2.h>
#include <SD.h>

#include "pins.h"

#define VERSION     "1.0"

#define RAM_START   0x0000
#define RAM_END     0x7FFF
#define IO_START    0x8000
#define IO_END      0x9FFF
#define DEVIO_START 0x9800
#define DEVIO_END   0x9BFF
#define ROM_START   0x8000
#define ROM_END     0xFFFF

#define PERIOD          1 // 1 uS
#define FREQ_MIN        1 // 1 Hz
#define FREQ_MAX        1000000 // 1 Mhz
#define FREQ_MAX_DEBUG  2048 // 2048 Hz
#define DELAY(freq)     (unsigned long) ((((1.0 / freq) * 1000000) / PERIOD) / 2)

#define DEBOUNCE  5     // 5 milliseconds

#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')

byte RAM[RAM_END-RAM_START+1];
byte ROM[ROM_END-ROM_START+1];

Button stepButton = Button();
Button runStopButton = Button();

void onTick();
void onLow();
void onHigh();

void initRAM();
void initROM();
void initPins();
void initButtons();
void initSD();
void setAddrDirIn();
void setDataDirIn();
void setDataDirOut();

word readAddress();
byte readData();
void writeData(byte d);

void reset();
void debug();
void step();
void toggleRunStop();
void toggleDebug();
void toggleRAM();
void toggleROM();
void listROMs();
void loadROM(unsigned int index);
void changeFrequency();
void info();

unsigned int freq = 1;
unsigned int ticks = 0;
bool isDebugging = true;
bool isRunning = false;
bool RAMEnabled = true;
bool ROMEnabled = true;

word address = 0;
byte data = 0;
byte readWrite = HIGH;

String ROMs[10];

void setup() {
  initPins();
  initButtons();
  initRAM();
  initROM();

  digitalWriteFast(IRQB, HIGH);
  digitalWriteFast(NMIB, HIGH);
  digitalWriteFast(RDY, HIGH);
  digitalWriteFast(SOB, HIGH);
  digitalWriteFast(PHI2, HIGH);

  Timer1.initialize(PERIOD);
  Timer1.attachInterrupt(onTick);

  Serial.begin(9600);

  initSD();

  delay(1000);

  info();
  reset();
}

void loop() {
  stepButton.update();
  runStopButton.update();

  if (stepButton.pressed()) {
    step();
  }
  if (runStopButton.pressed()) {
    toggleRunStop();
  }

  if (Serial.available()) 
  {
    switch (Serial.read())
    {
      case '0':
        loadROM(0);
        break;
      case '1':
        loadROM(1);
        break;
      case '2':
        loadROM(2);
        break;
      case '3':
        loadROM(3);
        break;
      case '4':
        loadROM(4);
        break;
      case '5':
        loadROM(5);
        break;
      case '6':
        loadROM(6);
        break;
      case '7':
        loadROM(7);
        break;
      case '8':
        loadROM(8);
        break;
      case '9':
        loadROM(9);
        break;
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
      case 'a':
      case 'A':
        toggleRAM();
        break;
      case 'o':
      case 'O':
        toggleROM();
        break;
      case 'l':
      case 'L':
        listROMs();
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
      digitalWriteFast(PHI2, HIGH);
    }

    ticks = 0;
  } else {
    ticks++;
  }
}

void onLow() {
  // Read data from bus just before high to low transition
  data = readData();

  if (readWrite == LOW) { // WRITING
    if ((address >= IO_START) && (address <= IO_END)) {
      if ((address >= DEVIO_START) && (address <= DEVIO_END)) {
        // 6502 is writing to Dev Board IO space
        // TODO: Handle IO
      } else {
        // 6502 is writing to other IO space
      }
    } else if ((address >= RAM_START) && (address <= RAM_END)) {
      RAM[address] = data;
    }
  }

  digitalWriteFast(PHI2, LOW);

  if (freq <= FREQ_MAX_DEBUG && isDebugging) {
    debug();
  }
}

void onHigh() {
  digitalWriteFast(PHI2, HIGH);
  
  setDataDirIn();

  readWrite = digitalReadFast(RWB);
  address = readAddress();

  if (readWrite == HIGH) { // READING
    if ((address >= IO_START) && (address <= IO_END)) {
      if ((address >= DEVIO_START) && (address <= DEVIO_END)) {
        // 6502 is reading from Dev Board IO space
        // TODO: Handle IO
      } else {
        // 6502 is reading from other IO space
      }
    } else if ((address >= ROM_START) && (address <= ROM_END) && ROMEnabled) { // ROM
      setDataDirOut();
      writeData(ROM[address - ROM_START]);
    } else if ((address >= RAM_START) && (address <= RAM_END) && RAMEnabled) { // RAM
      setDataDirOut();
      writeData(RAM[address - RAM_START]);
    } 
  }
}

void reset() {
  bool isCurrentlyRunning = isRunning;

  if (isCurrentlyRunning) {
    isRunning = false;
  }

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

  if (isCurrentlyRunning) {
    isRunning = true;
  }
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
  isRunning = !isRunning;

  Serial.println(isRunning ? "Running…" : "Stopped");
}

void toggleDebug() {
  isDebugging = !isDebugging;

  Serial.print("Debug: ");
  Serial.println(isDebugging ? "Enabled" : "Disabled");
}

void toggleRAM() {
  RAMEnabled = !RAMEnabled;

  Serial.print("RAM: ");
  Serial.println(RAMEnabled ? "Enabled" : "Disabled");
}

void toggleROM() {
  ROMEnabled = !ROMEnabled;

  Serial.print("ROM: ");
  Serial.println(ROMEnabled ? "Enabled" : "Disabled");
}

void listROMs() {
  File ROMDirectory = SD.open("/ROMS");

  for (unsigned int i = 0; i < 10; i++) {
    File file = ROMDirectory.openNextFile();

    if (file) {
      ROMs[i] = String(file.name());
      file.close();
    } else {
      ROMs[i] = "?";
    }
  }

  for (unsigned int j = 0; j < 10; j++) {
    Serial.print("(");
    Serial.print(j);
    Serial.print(")");
    Serial.print(" - ");
    Serial.println(ROMs[j]);
  }
  Serial.println();

  ROMDirectory.close();
}

void loadROM(unsigned int index) {
  String directory = "/ROMS/";
  String path = directory + ROMs[index];

  File file = SD.open(path.c_str());

  if (file) {
    unsigned int i = 0;

    while(file.available()) {
      ROM[i] = file.read();
      i++;
    }

    Serial.print("Loaded ROM: ");
    Serial.println(ROMs[index]);
  } else {
    Serial.println("Invalid ROM!");
  }

  file.close();
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
  delay(100);
}

void info() {
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
  Serial.println();
  Serial.print("6502 Dev Board Debugger | Version: ");
  Serial.println(VERSION);
  Serial.println();
  Serial.print("RAM: ");
  Serial.println(RAMEnabled ? "Enabled" : "Disabled");
  Serial.print("ROM: ");
  Serial.println(ROMEnabled ? "Enabled" : "Disabled");
  Serial.print("Debug: ");
  Serial.println(isDebugging ? "Enabled" : "Disabled");
  Serial.print("Frequency: ");
  Serial.print(freq);
  Serial.println(" Hz");
  Serial.println();
  Serial.println("-------------------------------------------------------");
  Serial.println("| (R)un / Stop        | (S)tep          | Rese(T)     |");
  Serial.println("-------------------------------------------------------");
  Serial.println("| Toggle R(A)M        | Toggle R(O)M    | (L)ist ROMs |");
  Serial.println("-------------------------------------------------------");
  Serial.println("| Change (F)requency  | Toggle (D)ebug  | (I)nfo      |");
  Serial.println("-------------------------------------------------------");
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
    readWrite ? 'R' : 'W'
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
  pinMode(RESB, OUTPUT);
  pinMode(GPIO0, OUTPUT);
  pinMode(RWB, INPUT_PULLUP);
  pinMode(RDY, OUTPUT);
  pinMode(SOB, OUTPUT);
  pinMode(PHI2, OUTPUT);
  pinMode(NMIB, OUTPUT);
  pinMode(IRQB, OUTPUT);

  pinMode(STEP_SWB, INPUT);
  pinMode(RS_SWB, INPUT);

  setAddrDirIn();
  setDataDirIn();
}

void initButtons() {
  stepButton.attach(STEP_SWB, INPUT);
  stepButton.interval(DEBOUNCE);
  stepButton.setPressedState(LOW);

  runStopButton.attach(RS_SWB, INPUT);
  runStopButton.interval(DEBOUNCE);
  runStopButton.setPressedState(LOW);
}

void initSD() {
  SD.begin(BUILTIN_SDCARD);
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