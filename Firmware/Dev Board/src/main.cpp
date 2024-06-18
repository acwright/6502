#include <Arduino.h>
#include <TimerOne.h>
#include <TimeLib.h>
#include <Bounce2.h>
#include <SD.h>
#include <EEPROM.h>

#include "pins.h"
#include "utilities.h"

/*      MEMORY MAP      */
/* -------------------- */
/* |                  | */
/* |        ROM       | */
/* |    $A000-$FFFF   | */
/* |                  | */
/* -------------------- */
/* |    IO 7 ($9C00)  | */
/* -------------------- */
/* |    IO 6 ($9800)  | */
/* -------------------- */
/* |    IO 5 ($9400)  | */
/* -------------------- */
/* |    IO 4 ($9000)  | */
/* -------------------- */
/* |    IO 3 ($8C00)  | */
/* -------------------- */
/* |    IO 2 ($8800)  | */
/* -------------------- */
/* |    IO 1 ($8400)  | */
/* -------------------- */
/* |    IO 0 ($8000)  | */
/* -------------------- */
/* |                  | */
/* |        RAM       | */
/* |    $0000-$7FFF   | */
/* |                  | */
/* -------------------- */

#define RAM_START   0x0000
#define RAM_END     0x7FFF
#define IO_START    0x8000
#define IO_END      0x9FFF
#define ROM_START   0x8000
#define ROM_CODE    0xA000
#define ROM_END     0xFFFF

#define TIMER_PERIOD              1 // 1 uS
#define FREQS                     (String[20])  { "1 Hz", "2 Hz", "4 Hz", "8 Hz", "16 Hz", "32 Hz", "64 Hz", "122 Hz", "244 Hz", "488 Hz", "976 Hz", "1.9 kHz", "3.9 kHz", "7.8 kHz", "15.6 kHz", "31.2 kHz", "62.5 kHz", "125 kHz", "250 kHz", "500 kHz" }
#define FREQ_DELAYS               (int[20])     { 250000, 125000, 62500, 31250, 15625, 7812, 3906, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1, 0 }

#define DEBOUNCE        5          // 5 milliseconds

#define IO_BANK_SIZE    0x3FF

#define PAGE_SIZE       1024
#define RAM_PAGES       32
#define IO_PAGES        8
#define ROM_PAGES       32
#define ROM_MAX         256

#define VERSION         "1.0.0"

const word IO_BANKS[8] { 0x8000, 0x8400, 0x8800, 0x8C00, 0x9000, 0x9400, 0x9800, 0x9C00 };

byte RAM[RAM_END - RAM_START + 1];
byte IO[IO_END - IO_START + 1];
byte ROM[ROM_END - ROM_START + 1];

Button intButton = Button();
Button stepButton = Button();
Button runStopButton = Button();

void onTick();
void onCycleBegin();
void onCycleEnd();
void onCommand(char command);

void initPins();
void initButtons();
void initRAM();
void initIO();
void initROM();
void initSD();

void setAddrDirIn();
void setDataDirIn();
void setDataDirOut();
word readAddress();
byte readData();
void writeData(byte d);

void reset();
void info();
void log();
void step(); 
void snapshot();
void decreaseFrequency();
void increaseFrequency();
void toggleRunStop();
void toggleOutput();
void toggleRAM();
void toggleIO();
void toggleIOBank();
void toggleROM();
void readROMs();
void listROMs();
void loadROM(unsigned int index);
void prevPage();
void nextPage();

time_t syncTime();

unsigned int freqIndex = 19;      // 500 kHz
volatile unsigned int freqCounter = 0;
volatile unsigned int stepCounter = 0;
volatile bool isEdge = false;
volatile bool isRunning = false;
volatile bool isStepping = false;
volatile bool isLogging = false;

bool OutputEnabled = false;
bool RAMEnabled = false;
bool IOEnabled = false;
bool ROMEnabled = false;
String romFile = "None";
byte IOBank = 7;                  // By default, debugger IO bank is $9C00

word address = 0;
byte data = 0;
bool readWrite = HIGH;
bool be = HIGH;
bool rdy = HIGH;
bool sync = LOW;
bool resb = HIGH;
bool irqb = HIGH;
bool nmib = HIGH;

String ROMs[ROM_MAX];
unsigned int romPage = 0;

//
// MAIN LOOPS
//

void setup() {
  initPins();
  initButtons();
  initRAM();
  initIO();
  initROM();
  initSD();

  setSyncProvider(syncTime);
  setSyncInterval(1);

  Serial.begin(9600);
  
  // Wait up to 2 sec for serial to connect
  int timeout = 2000;
  while (!Serial && timeout--) {
    delay(1);
  }

  info();

  Timer1.initialize(TIMER_PERIOD);
  Timer1.attachInterrupt(onTick);
}

void loop() {
  // Update buttons
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

  // Check for key press
  if (Serial.available()) 
  {
    onCommand(Serial.read());
  }
}

//
// EVENTS
//

FASTRUN void onTick()
{
  // This is called in the timer interrupt so extra care should be taken!

  if (freqCounter <= 0) {
    if (isRunning) {
      if (!digitalReadFast(PHI2)) {
        digitalWriteFast(PHI2, HIGH);
        onCycleBegin();
      } else {
        onCycleEnd();
        digitalWriteFast(PHI2, LOW);
        setDataDirIn();
      }
    }

    freqCounter = FREQ_DELAYS[freqIndex];
  } else {
    freqCounter--;
  }

  if (stepCounter <= 0) {
    if (isStepping) {
      if (!digitalReadFast(PHI2)) {
        digitalWriteFast(PHI2, HIGH);
        onCycleBegin();
      } else {
        onCycleEnd();
        digitalWriteFast(PHI2, LOW);
        isStepping = false; // We are done stepping...
        isLogging = true;   // Log on next tick
      }
    }

    // Handle step logging
    if (isLogging) {
      log();
      isLogging = false;
    }

    stepCounter = FREQ_DELAYS[2]; // Use 4 Hz for step counter
  } else {
    stepCounter--;
  }
}

FASTRUN void onCycleBegin() {
  // Read the address and r/w at the beginning of cycle (low to high transition)
  address = readAddress();
  readWrite = digitalReadFast(RWB);
  sync = digitalReadFast(SYNC);
  be = digitalReadFast(BE);

  if (readWrite == HIGH) { // READING
    // Check if in IO space first since it overlaps ROM space...
    // Only output data if address is in selected IOBank area and IO is enabled
    if ((address >= IO_BANKS[IOBank]) && (address <= IO_BANKS[IOBank] + IO_BANK_SIZE) && IOEnabled) { // IO
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

FASTRUN void onCycleEnd() {
  // Read data at end of cycle (just before high to low transition)
  data = readData();

  // Read the control pins
  resb = digitalReadFast(RESB);
  nmib = digitalReadFast(NMIB);
  irqb = digitalReadFast(IRQB);
  rdy = digitalReadFast(RDY);

  if (readWrite == LOW) { // WRITING
    if ((address >= IO_START) && (address <= IO_END)) { // IO
      IO[address - IO_START] = data;
    } else if ((address >= RAM_START) && (address <= RAM_END)) { // RAM
      RAM[address - RAM_START] = data;
    }
  }
}

void onCommand(char command) {
  switch (command) {
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
    case 'p':
    case 'P':
      snapshot();
      break;
    case 'u':
    case 'U':
      toggleOutput();
      break;
    case 'a':
    case 'A':
      toggleRAM();
      break;
    case 'i':
    case 'I':
      toggleIO();
      break;
    case 'k':
    case 'K':
      toggleIOBank();
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
      info();
      break;
    case 'g':
    case 'G':
      log();
      break;
    case '-':
      decreaseFrequency();
      break;
    case '+':
      increaseFrequency();
      break;
      break;
    case '[':
      prevPage();
      break;
    case ']':
      nextPage();
      break;
  }
}

//
// METHODS
//

void info() {
  Serial.println();
  Serial.println("eeee  eeeee eeeeee eeee   8eeee8 8eeee 88   8   8eeee8   8eee88 8eeee8 8eee8  8eeee8 ");                        
  Serial.println("8  8  8     8    8    8   8    8 8     88   8   8    8   8    8 8    8 8   8  8    8 ");
  Serial.println("8     8eeee 8    8    8   8e   8 8eeee 88  e8   8eeee8ee 8    8 8eeee8 8eee8e 8e   8 ");
  Serial.println("8eeee     8 8    8 eee8   88   8 88     8  8    88     8 8    8 88   8 88   8 88   8 ");
  Serial.println("8   8     8 8    8 8      88   8 88     8  8    88     8 8    8 88   8 88   8 88   8 ");
  Serial.println("8eee8 eeee8 8eeee8 8eee   88eee8 88eee  8ee8    88eeeee8 8eeee8 88   8 88   8 88eee8 ");
  Serial.println();
  Serial.print("6502 Dev Board | Version: ");
  Serial.print(VERSION);
  Serial.println();
  Serial.println("---------------------------------");
  Serial.println("| Created by A.C. Wright © 2024 |");
  Serial.println("---------------------------------");
  Serial.println();
  Serial.print("OUTPUT: ");
  Serial.println(OutputEnabled ? "Enabled" : "Disabled");
  Serial.print("RAM: ");
  Serial.println(RAMEnabled ? "Enabled" : "Disabled");
  Serial.print("IO: ");
  Serial.print(IOEnabled ? "Enabled" : "Disabled");
  Serial.print(" ($");
  Serial.print(IO_BANKS[IOBank], HEX);
  Serial.println(")");
  Serial.print("ROM: ");
  Serial.print(ROMEnabled ? "Enabled" : "Disabled");
  Serial.print(" (");
  Serial.print(romFile);
  Serial.println(")");
  Serial.print("Frequency: ");
  Serial.println(FREQS[freqIndex]);
  Serial.println();
  Serial.println("------------------------------------------------------------");
  Serial.println("| (R)un / Stop         | (S)tep           | Rese(T)        |");
  Serial.println("------------------------------------------------------------");
  Serial.println("| Toggle R(A)M         | Toggle (I)O      | Toggle R(O)M   |");
  Serial.println("------------------------------------------------------------");
  Serial.println("| (L)ist ROMs          | Change IO Ban(K) | Sna(P)shot     |");
  Serial.println("------------------------------------------------------------");
  Serial.println("| (+/-) Clk Frequency  | Toggle Outp(U)ts | In(F)o / Lo(G) |");
  Serial.println("------------------------------------------------------------");
  Serial.println();
}

void log() {
  char output[128];

  sprintf(
    output, 
    "| %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c | %c%c%c%c%c%c%c%c | 0x%04X | 0x%02X | %c | RDY: %01X | BE: %01X | RESB: %01X | NMIB: %01X | IRQB: %01X | SYNC: %01X |", 
    BYTE_TO_BINARY((address >> 8) & 0xFF),
    BYTE_TO_BINARY(address & 0xFF), 
    BYTE_TO_BINARY(data),
    address, 
    data, 
    readWrite ? 'R' : 'W',
    rdy,
    be,
    resb,
    nmib,
    irqb,
    sync
  );

  Serial.println(output);
}

void reset() {
  bool isCurrentlyRunning = isRunning;

  if (isCurrentlyRunning) {
    toggleRunStop();
  }

  Serial.print("Resetting... ");

  pinMode(RESB, OUTPUT);
  digitalWriteFast(RESB, LOW);
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
  digitalWriteFast(PHI2, LOW);
  delay(100);
  digitalWriteFast(RESB, HIGH);
  pinMode(RESB, INPUT);

  Serial.println("Done");

  if (isCurrentlyRunning) {
    toggleRunStop();
  }
}

void step() {
  if (!isRunning) {
    isStepping = true;
  } else {
    Serial.println("Clock must be stopped before stepping!");
  }
}

void snapshot() {
  Serial.print("Creating snapshot... ");

  if (!SD.mediaPresent()) {
    Serial.print("No Card Detected!");
    return;
  }

  if (!SD.exists("Snapshots")) {
    SD.mkdir("Snapshots");
  }

  time_t time = now();
  String path = "Snapshots/";
  String IOpath = "Snapshots/";

  path.append(time);
  path.append(".bin");

  File snapshot = SD.open(path.c_str(), FILE_WRITE);

  if (snapshot) {
    for(unsigned int i = 0; i < (RAM_END - RAM_START); i++) {
      snapshot.write(RAM[i]);
    }
    snapshot.close();
  }

  Serial.print("Success! (");
  Serial.print(time);
  Serial.println(")");
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

  if (!isRunning && digitalReadFast(PHI2)) { 
    onCycleEnd();
    digitalWriteFast(PHI2, LOW);
  }

  Serial.println(isRunning ? "Running…" : "Stopped");
}

void toggleOutput() {
  OutputEnabled = !OutputEnabled;

  if (OutputEnabled) {
    digitalWriteFast(OE1, HIGH);
    digitalWriteFast(OE2, HIGH);
    digitalWriteFast(OE3, HIGH);
  } else {
    digitalWriteFast(OE1, LOW);
    digitalWriteFast(OE2, LOW);
    digitalWriteFast(OE3, LOW);
  }

  Serial.print("OUTPUT: ");
  Serial.println(OutputEnabled ? "Enabled" : "Disabled");
}

void toggleRAM() {
  RAMEnabled = !RAMEnabled;

  Serial.print("RAM: ");
  Serial.println(RAMEnabled ? "Enabled" : "Disabled");
}

void toggleIO() {
  IOEnabled = !IOEnabled;

  Serial.print("IO: ");
  Serial.print(IOEnabled ? "Enabled" : "Disabled");
  Serial.print(" ($");
  Serial.print(IO_BANKS[IOBank], HEX);
  Serial.println(")");
}

void toggleIOBank() {
  if (IOBank < 7) {
    IOBank++;
  } else {
    IOBank = 0;
  }

  Serial.print("IO: ");
  Serial.print(IOEnabled ? "Enabled" : "Disabled");
  Serial.print(" ($");
  Serial.print(IO_BANKS[IOBank], HEX);
  Serial.println(")");
}

void toggleROM() {
  ROMEnabled = !ROMEnabled;

  Serial.print("ROM: ");
  Serial.println(ROMEnabled ? "Enabled" : "Disabled");
}

void readROMs() {
  if (!SD.exists("ROMS")) {
    SD.mkdir("ROMS");
  }

  for (unsigned int i = 0; i < ROM_MAX; i++) {
    ROMs[i] = "?";
  }

  File ROMDirectory = SD.open("ROMS");

  unsigned int index = 0;

  while(true) {
    File file = ROMDirectory.openNextFile();

    if (file) {
      String filename = String(file.name());
      if (!filename.startsWith(".")) {
        ROMs[index] = filename;
        index++;
      }
      file.close();
    } else {
      ROMDirectory.close();
      break;
    }
  }
}

void listROMs() {
  for (unsigned int j = (romPage * 8); j < ((romPage * 8) + 8); j++) {
    Serial.print("(");
    Serial.print(j - (romPage * 8));
    Serial.print(")");
    Serial.print(" - ");
    Serial.println(ROMs[j]);
  }

  Serial.println();
  Serial.print("Page: ");
  Serial.print(romPage);
  Serial.println(" | ([) Prev Page | Next Page (]) |");
  Serial.println();
}

void loadROM(unsigned int index) {
  String directory = "ROMS/";
  String filename = ROMs[(romPage * 8) + index];

  if (!filename.length()) { 
    Serial.println("Invalid ROM! (L)ist ROMs before loading.");
    return;
  }

  String path = directory + filename;

  File file = SD.open(path.c_str());

  if (file) {
    unsigned int i = 0;

    while(file.available()) {
      ROM[i] = file.read();
      i++;
    }

    romFile = ROMs[(romPage * 8) + index];

    Serial.print("Loaded ROM: ");
    Serial.println(romFile);
  } else {
    Serial.println("Invalid ROM!");
  }

  file.close();
}

void prevPage() {
  if (romPage > 0)  {
    romPage--;
  } else {
    romPage = 31;
  }

  listROMs();
}

void nextPage() {
  if (romPage < 31) {
    romPage++;
  } else {
    romPage = 0;
  }

  listROMs();
}

time_t syncTime() {
  return Teensy3Clock.get();
}

//
// INITIALIZATION
//

void initRAM() {
  unsigned int i;
  
  for (i = 0; i < (RAM_END - RAM_START + 1); i++) {
    RAM[i] = 0x00; 
  }
}

void initIO() {
  unsigned int i;
  
  for (i = 0; i < (IO_END - IO_START + 1); i++) {
    IO[i] = 0x00; 
  }
}

void initROM() {
  unsigned int i;

  // Fill ROM with NOPs by default
  for (i = 0; i < (ROM_END - ROM_START + 1); i++) {
    ROM[i] = 0xEA;
  }
}

void initPins() {
  pinMode(RESB, INPUT);
  pinMode(IRQB, INPUT);
  pinMode(NMIB, INPUT);
  pinMode(RDY, INPUT);
  pinMode(BE, INPUT);
  pinMode(SYNC, INPUT);
  pinMode(RWB, INPUT);
  pinMode(PHI2, OUTPUT);

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

  digitalWriteFast(PHI2, LOW);

  digitalWriteFast(OE1, LOW);
  digitalWriteFast(OE2, LOW);
  digitalWriteFast(OE3, LOW);
}

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

  readROMs();
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