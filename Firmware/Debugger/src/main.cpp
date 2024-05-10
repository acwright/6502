#include <Arduino.h>
#include <ArduinoJson.h>
#include <TimerOne.h>
#include <Bounce2.h>
#include <SD.h>
#include <QNEthernet.h>

#if DEVBOARD
#include "devboard.h"
#elif RETROSHIELD
#include "retroshield.h"
#endif
#include "utilities.h"

using namespace qindesign::network;

#define RAM_START   0x0000
#define RAM_END     0x7FFF
#define IO_START    0x9BFF
#define IO_END      0x9FFF
#define ROM_START   0x8000
#define ROM_CODE    0xA000
#define ROM_END     0xFFFF

#define TIMER_PERIOD              1 // 1 uS
#define FREQS                     (String[20])  {"1 Hz", "2 Hz", "4 Hz", "8 Hz", "16 Hz", "32 Hz", "64 Hz", "122 Hz", "244 Hz", "488 Hz", "976 Hz", "1.9 kHz", "3.9 kHz", "7.8 kHz", "15.6 kHz", "31.2 kHz", "62.5 kHz", "125 kHz", "250 kHz", "500 kHz"}
#define FREQ_DELAYS               (int[20])     {250000, 125000, 62500, 31250, 15625, 7812, 3906, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1, 0}

#define DEBOUNCE  5     // 5 milliseconds
#define IO_BUFFER_SIZE  255

byte RAM[RAM_END-RAM_START+1];
byte IO[IO_END-IO_START+1];
byte ROM[ROM_END-ROM_START+1];

byte IOPayloadReadPointer = 0;
byte IOPayloadWritePointer = 0;
JsonDocument IOPayloads[IO_BUFFER_SIZE];

Button intButton = Button();
Button stepButton = Button();
Button runStopButton = Button();

void onTick();
void onLow();
void onHigh();
void onCommand(char command);

void initPins();
void initButtons();
void initRAM();
void initIO();
void initROM();
void initSD();
void initEthernet();

void setAddrDirIn();
void setDataDirIn();
void setDataDirOut();
word readAddress();
byte readData();
void writeData(byte d);

void reset();
void info();
void debug();
void step(); 
void decreaseFrequency();
void increaseFrequency();
void toggleRunStop();
void toggleDebug();
void toggleRAM();
void toggleIO();
void toggleROM();
void listROMs();
void loadROM(unsigned int index);

unsigned int freqIndex = 0; // 1 Hz
unsigned int freqCounter = 0;

bool isDebugging = true;
bool isRunning = false;
bool RAMEnabled = true;
bool IOEnabled = true;
bool ROMEnabled = true;
String romFile = "None";

word address = 0;
byte data = 0;
byte readWrite = HIGH;

String ROMs[10];

EthernetClient ethClient;

//
// MAIN
//

void setup() {
  initPins();
  initButtons();
  initRAM();
  initIO();
  initROM();

  Timer1.initialize(TIMER_PERIOD);
  Timer1.attachInterrupt(onTick);

  Serial.begin(9600);

  delay(1500);

  initSD();
  initEthernet();

  info();
  reset();
}

void loop() {
  intButton.update();
  stepButton.update();
  runStopButton.update();

  if (intButton.pressed()) {
    increaseFrequency();
  }
  if (stepButton.pressed()) {
    step();
  }
  if (runStopButton.pressed()) {
    toggleRunStop();
  }

  if (Serial.available()) 
  {
    onCommand(Serial.read());
  }
}

//
// EVENTS
//

void onTick()
{
  if (freqCounter == 0) {
    if (isRunning) {
      digitalReadFast(PHI2) ? onLow() : onHigh();
    } else if (!digitalReadFast(PHI2)) { 
      digitalWriteFast(PHI2, HIGH);
    }

    freqCounter = FREQ_DELAYS[freqIndex];
  } else {
    freqCounter--;
  }
}

void onLow() {
  // Read data from bus just before high to low transition
  data = readData();

  if (readWrite == LOW) { // WRITING
    if ((address >= IO_START) && (address <= IO_END) && IOEnabled) {
      IO[address - IO_START] = data;
    } else if ((address >= RAM_START) && (address <= RAM_END) && RAMEnabled) {
      RAM[address - RAM_START] = data;
    }
  }

  digitalWriteFast(PHI2, LOW);

  if (isDebugging) {
    debug();
  }
}

void onHigh() {
  digitalWriteFast(PHI2, HIGH);
  
  setDataDirIn();

  readWrite = digitalReadFast(RWB);
  address = readAddress();

  if (readWrite == HIGH) { // READING
    if ((address >= IO_START) && (address <= IO_END) && IOEnabled) {
      setDataDirOut();
      writeData(IO[address - IO_START]);
    } else if ((address >= RAM_START) && (address <= RAM_END) && RAMEnabled) { // RAM
      setDataDirOut();
      writeData(RAM[address - RAM_START]);
    } else if ((address >= ROM_CODE) && (address <= ROM_END) && ROMEnabled) { // ROM
      setDataDirOut();
      writeData(ROM[address - ROM_START]);
    }
  }
}

void onCommand(char command) {
  switch (command)
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
      case 'i':
      case 'I':
        toggleIO();
        break;
      case 'o':
      case 'O':
        toggleROM();
        break;
      case 'l':
      case 'L':
        listROMs();
        break;
      case '-':
        decreaseFrequency();
        break;
      case '+':
        increaseFrequency();
        break;
      case 'd':
      case 'D':
        toggleDebug();
        break;
      case 'f':
      case 'F':
        info();
        break;
    }
}

//
// METHODS
//

void info() {
  Serial.println();
  Serial.println("eeee  eeeee eeeeee eeee   eeeeee                                            ");                        
  Serial.println("8  8  8     8    8    8   8    8 eeee eeeee  e   e eeeee eeeee eeee eeeee   ");
  Serial.println("8     8eeee 8    8    8   8e   8 8    8   8  8   8 8   8 8   8 8    8   8   ");
  Serial.println("8eeee     8 8    8 eee8   88   8 8eee 8eee8e 8e  8 8e    8e    8eee 8eee8e  ");
  Serial.println("8   8     8 8    8 8      88   8 88   88   8 88  8 88 e8 88 e8 88   88   8  ");
  Serial.println("8eee8 eeee8 8eeee8 8eee   88eee8 88ee 88eee8 88ee8 88ee8 88ee8 88ee 88   8  ");
  Serial.println();
  Serial.print("6502 Debugger | Version: ");
  Serial.println(VERSION);
  Serial.println();
  Serial.println("---------------------------------");
  Serial.println("| Created by A.C. Wright © 2024 |");
  Serial.println("---------------------------------");
  Serial.println();
  Serial.print("RAM: ");
  Serial.println(RAMEnabled ? "Enabled" : "Disabled");
  Serial.print("IO: ");
  Serial.println(IOEnabled ? "Enabled" : "Disabled");
  Serial.print("ROM: ");
  Serial.print(ROMEnabled ? "Enabled" : "Disabled");
  Serial.print(" (");
  Serial.print(romFile);
  Serial.println(")");
  Serial.print("Debug: ");
  Serial.println(isDebugging ? "Enabled" : "Disabled");
  Serial.print("Frequency: ");
  Serial.println(FREQS[freqIndex]);
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());
  Serial.println();
  Serial.println("-------------------------------------------------------");
  Serial.println("| (R)un / Stop        | (S)tep          | Rese(T)     |");
  Serial.println("-------------------------------------------------------");
  Serial.println("| Toggle R(A)M        | Toggle R(O)M    | (L)ist ROMs |");
  Serial.println("-------------------------------------------------------");
  Serial.println("| Toggle (I)O         |                 |             |");
  Serial.println("-------------------------------------------------------");
  Serial.println("| (+/-) Clk Frequency | Toggle (D)ebug  | In(F)o      |");
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
  digitalWriteFast(PHI2, LOW);
  delay(100);
  digitalWriteFast(PHI2, HIGH);
  delay(100);
  digitalWriteFast(RESB, HIGH);

  if (isCurrentlyRunning) {
    isRunning = true;
  }
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

void decreaseFrequency() {
  if (freqIndex > 0) {
    freqIndex--;
  } else {
    freqIndex = 19;
  }

  Serial.print("Frequency: ");
  Serial.println(FREQS[freqIndex]);
}

void increaseFrequency() {
  if (freqIndex < 19) {
    freqIndex++;
  } else {
    freqIndex = 0;
  }

  Serial.print("Frequency: ");
  Serial.println(FREQS[freqIndex]);
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

void toggleIO() {
  IOEnabled = !IOEnabled;

  Serial.print("IO: ");
  Serial.println(IOEnabled ? "Enabled" : "Disabled");
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
      String filename = String(file.name());
      if (!filename.startsWith(".")) {
        ROMs[i] = filename;
      }
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

    romFile  = ROMs[index];

    Serial.print("Loaded ROM: ");
    Serial.println(romFile);
  } else {
    Serial.println("Invalid ROM!");
  }

  file.close();
}

//
// INITIALIZATION
//

void initRAM() {
  unsigned int i;
  
  for (i = 0; i < sizeof(RAM) ; i++) {
    RAM[i] = 0x00; 
  }
}

void initIO() {
  unsigned int i;
  
  for (i = 0; i < sizeof(IO) ; i++) {
    IO[i] = 0x00; 
  }
}

void initROM() {
  unsigned int i;

  // Fill ROM with NOPs by default
  for (i = 0; i < sizeof(ROM) ; i++) {
    ROM[i] = 0xEA;
  }
}

#if DEVBOARD
void initPins() {
  pinMode(RESB, INPUT);
  pinMode(SYNC, INPUT);
  pinMode(RWB, INPUT);
  pinMode(RDY, INPUT);
  pinMode(BE, INPUT);
  pinMode(PHI2, OUTPUT);
  pinMode(NMIB, INPUT);
  pinMode(IRQB, INPUT);

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
  digitalWriteFast(PHI2, HIGH);
  
  digitalWriteFast(OE1, HIGH);
  digitalWriteFast(OE2, HIGH);
  digitalWriteFast(OE3, HIGH);
}
#elif RETROSHIELD
void initPins() {
  pinMode(RESB, OUTPUT);
  pinMode(RWB, INPUT);
  pinMode(RDY, OUTPUT);
  pinMode(SOB, OUTPUT);
  pinMode(PHI2, OUTPUT);
  pinMode(NMIB, OUTPUT);
  pinMode(IRQB, OUTPUT);

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
  digitalWriteFast(PHI2, HIGH);
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

void initSD() {
  SD.begin(BUILTIN_SDCARD);
}

void initEthernet() {
  Ethernet.begin();

  delay(1500);
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