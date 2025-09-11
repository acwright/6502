#include <Arduino.h>
#include <TeensyTimerTool.h>
#include <TimeLib.h>
#include <Bounce2.h>
#include <SD.h>
#include <EEPROM.h>

#if DEVBOARD
#include "devboard.h"
#endif

#include "macros.h"
#include "utilities.h"
#include "constants.h"

#include "terminal.h"
#include "mouse.h"
#include "joystick.h"
#include "rtc.h"
#include "keyboard.h"

byte RAM[RAM_END - RAM_START + 1];
byte ROM[ROM_END - ROM_START + 1];

Button intButton      = Button();
Button stepButton     = Button();
Button runStopButton  = Button();

USBHost             usb;
USBHub              hub1(usb);
USBHIDParser        hid1(usb);
USBHIDParser        hid2(usb);
USBHIDParser        hid3(usb);

void onTimer();
void onTick();
void onClock();
void onRead();
void onWrite();
void onCommand(char command);

void info();
void log();
void reset();
void step(); 
void snapshot();
void decreaseFrequency();
void increaseFrequency();
void toggleRunStop();
void toggleCPU();
void toggleRAM();
void toggleIO();
void toggleIOBank();
void toggleROM();
bool readROMs();
void listROMs();
void loadROM(unsigned int index);
bool readCarts();
void listCarts();
void loadCart(unsigned int index);
void prevPage();
void nextPage();

byte readIO(word address);
void writeIO(word address, byte data);

void initButtons();
void initSD();
void initRAM();
void initROM();

unsigned int freqIndex = 20;      // 1 MHz
bool isRunning = false;
bool isStepping = false;
volatile bool isClockEdge = false;

bool CPUEnabled = false;
bool RAMEnabled = true;
bool IOEnabled = true;
bool ROMEnabled = true;
String romFile = "None";
String cartFile = "None";
bool fileCtx = FILE_CTX_ROM;
byte IOBank = 2;                  // By default, debugger IO bank is $8800

word address = 0;
byte data = 0;
bool readWrite = LOW;
bool be = LOW;
bool rdy = LOW;
bool sync = LOW;
bool resb = LOW;
bool irqb = LOW;
bool nmib = LOW;

String ROMs[ROM_MAX];
unsigned int romPage = 0;

String Carts[CART_MAX];
unsigned int cartPage = 0;

TeensyTimerTool::PeriodicTimer timer(TeensyTimerTool::TCK);

//
// MAIN LOOPS
//

void setup() {
  initPins();
  initButtons();
  initSD();
  initRAM();
  initROM();

  usb.begin();

  Terminal::begin();
  Mouse::begin();
  Joystick::begin();
  RTC::begin();
  Keyboard::begin();
  
  Serial.begin(9600);
  
  // Wait up to 2 sec for serial to connect
  int timeout = 2000;
  while (!Serial && timeout--) {
    delay(1);
  }

  info();
  reset();

  setAddrDirIn();
  setDataDirIn();

  timer.begin(onTick, FREQ_PERIODS[freqIndex]);
}

void loop() {
  // Clock the CPU if an edge has occurred
  if (isClockEdge) {
    onTick();
    isClockEdge = false;
  }

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

  // Update USB
  usb.Task();

  Terminal::update();
  Mouse::update();
  Joystick::update();
  RTC::update();
  Keyboard::update();
}

//
// EVENTS
//

FASTRUN void onTimer() {
  isClockEdge = true;
}

FASTRUN void onTick() {
  if (!isRunning) { return; }

  if (!digitalReadFast(PHI2)) {
    digitalWriteFast(PHI2, HIGH);

    onClock();
  } else {
    readWrite ? onRead() : onWrite();

    digitalWriteFast(PHI2, LOW);
  }
}

FASTRUN void onClock() {
  // Read the address and r/w at the beginning of cycle (low to high transition)
  address     = readAddress();
  readWrite   = digitalReadFast(RWB);

  // Read the control pins
  resb  = digitalReadFast(RESB);
  nmib  = digitalReadFast(NMIB);
  irqb  = digitalReadFast(IRQB);
  rdy   = digitalReadFast(RDY);

  #if DEVBOARD
  sync        = digitalReadFast(SYNC);
  be          = digitalReadFast(BE);
  #endif
}

FASTRUN void onRead() {
  if ((address >= IO_BANKS[IOBank]) && (address <= IO_BANKS[IOBank] + IO_BANK_SIZE) && IOEnabled) { // IO
    digitalWriteFast(OE1, HIGH);
    setDataDirOut();
    data = readIO(address);
    writeData(data);
  } else if ((address >= ROM_CODE) && (address <= ROM_END) && ROMEnabled) { // ROM
    digitalWriteFast(OE1, HIGH);
    setDataDirOut();
    data = ROM[address - ROM_START];
    writeData(data);
  } else if ((address >= RAM_START) && (address <= RAM_END) && RAMEnabled) { // RAM
    digitalWriteFast(OE1, HIGH);
    setDataDirOut();
    data = RAM[address - RAM_START];
    writeData(data);
  } else {
    digitalWriteFast(OE1, LOW);
  }
}

FASTRUN void onWrite() {
  if ((address >= IO_START) && (address <= IO_END) && IOEnabled) { // IO
    digitalWriteFast(OE1, HIGH);
    setDataDirIn();
    data = readData();
    writeIO(address, data);
  } else if ((address >= RAM_START) && (address <= RAM_END) && RAMEnabled) { // RAM
    digitalWriteFast(OE1, HIGH);
    setDataDirIn();
    data = readData();
    RAM[address - RAM_START] = data;
  } else {
    digitalWriteFast(OE1, LOW);
  }
}

void onCommand(char command) {
  switch (command) {
    case '0':
      fileCtx ? loadCart(0) : loadROM(0);
      break;
    case '1':
      fileCtx ? loadCart(1) : loadROM(1);
      break;
    case '2':
      fileCtx ? loadCart(2) : loadROM(2);
      break;
    case '3':
      fileCtx ? loadCart(3) : loadROM(3);
      break;
    case '4':
      fileCtx ? loadCart(4) : loadROM(4);
      break;
    case '5':
      fileCtx ? loadCart(5) : loadROM(5);
      break;
    case '6':
      fileCtx ? loadCart(6) : loadROM(6);
      break;
    case '7':
      fileCtx ? loadCart(7) : loadROM(7);
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
    #if DEVBOARD
    case 'u':
    case 'U':
      toggleCPU();
      break;
    #endif
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
    case 'm':
    case 'M':
      listROMs();
      break;
    case 'c':
    case 'C':
      listCarts();
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
  Serial.println("eeee  eeeee eeeeee eeee   8""""                                              ");                        
  Serial.println("8  8  8     8    8    8   8     eeeeeee e   e e     eeeee eeeee eeeee eeeee  ");
  Serial.println("8     8eeee 8    8    8   8eeee 8  8  8 8   8 8     8   8   8   8  88 8   8  ");
  Serial.println("8eeee     8 8    8 eee8   88    8e 8  8 8e  8 8e    8eee8   8e  8   8 8eee8e ");
  Serial.println("8   8     8 8    8 8      88    88 8  8 88  8 88    88  8   88  8   8 88   8 ");
  Serial.println("8eee8 eeee8 8eeee8 8eee   88eee 88 8  8 88ee8 88eee 88  8   88  8eee8 88   8 ");
  Serial.println();
  Serial.print("6502 Emulator | Version: ");
  #ifdef VERSION
  Serial.print(VERSION);
  #endif
  Serial.println();
  Serial.println("---------------------------------");
  Serial.println("| Created by A.C. Wright © 2024 |");
  Serial.println("---------------------------------");
  Serial.println();
  Serial.print("CPU: ");
  Serial.print(CPUEnabled ? "Enabled" : "Disabled");
  Serial.println(" (Not yet implemented)");
  Serial.print("RAM: ");
  Serial.println(RAMEnabled ? "Enabled" : "Disabled");
  Serial.print("ROM: ");
  Serial.print(ROMEnabled ? "Enabled" : "Disabled");
  Serial.print(" ROM(");
  Serial.print(romFile);
  Serial.print(") |");
  Serial.print(" Cart(");
  Serial.print(cartFile);
  Serial.println(")");
  Serial.print("IO: ");
  Serial.print(IOEnabled ? "Enabled" : "Disabled");
  Serial.print(" ($");
  Serial.print(IO_BANKS[IOBank], HEX);
  Serial.println(")");
  Serial.print("Frequency: ");
  Serial.println(FREQS[freqIndex]);
  Serial.print("Date / Time: ");
  Serial.println(RTC::formattedDateTime());
  Serial.println();
  Serial.println("--------------------------------------------------------------");
  Serial.println("| (R)un / Stop          | (S)tep           | Rese(T)         |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("| Toggle CP(U)          | Toggle R(A)M     | Toggle R(O)M    |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("| List RO(M)s / (C)arts | Change IO Ban(K) | Toggle (I)O     |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("| (+/-) Clk Frequency   | Sna(P)shot       | In(F)o / Lo(G)  |");
  Serial.println("--------------------------------------------------------------");
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

  digitalWriteFast(RESB, LOW);
  delay(100);
  digitalWriteFast(RESB, HIGH);
  delay(100);

  Serial.println("Done");

  if (isCurrentlyRunning) {
    toggleRunStop();
  }
}

void step() {
  if (isStepping) { return; }

  if (!isRunning) {
    isStepping = true;
    readWrite ? onRead() : onWrite();
    digitalWriteFast(PHI2, LOW);
    delay(100);
    digitalWriteFast(PHI2, HIGH);
    onClock();
    log();
    isStepping = false;
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
    freqIndex = 20;
  }

  Serial.print("Frequency: ");
  Serial.println(FREQS[freqIndex]);

  timer.setPeriod(FREQ_PERIODS[freqIndex]);
}

void increaseFrequency() {
  if (freqIndex < 20) {
    freqIndex++;
  } else {
    freqIndex = 0;
  }

  Serial.print("Frequency: ");
  Serial.println(FREQS[freqIndex]);

  timer.setPeriod(FREQ_PERIODS[freqIndex]);
}

void toggleRunStop() {
  isRunning = !isRunning;

  if (!isRunning && digitalReadFast(PHI2) == LOW) {
    digitalWriteFast(PHI2, HIGH);
    onClock();
  }

  Serial.println(isRunning ? "Running…" : "Stopped");
}

void toggleCPU() {
  CPUEnabled = !CPUEnabled;

  Serial.print("CPU: ");
  Serial.println(CPUEnabled ? "Enabled" : "Disabled");
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

bool readROMs() {
  if (!SD.mediaPresent()) { 
    Serial.println("SD card not detected!");
    return false;
  }

  if (!SD.exists("ROMs")) {
    SD.mkdir("ROMs");
  }

  for (unsigned int i = 0; i < ROM_MAX; i++) {
    ROMs[i] = "?";
  }

  File ROMDirectory = SD.open("ROMs");

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

  return true;
}

void listROMs() {
  if (!readROMs()) { return; }

  fileCtx = FILE_CTX_ROM;

  Serial.println();

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
  String directory = "ROMs/";
  String filename = ROMs[(romPage * 8) + index];

  if (!filename.length()) {
    Serial.println("Invalid ROM! List RO(M)s before loading.");
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
    cartFile = "None";
    
    Serial.print("Loaded ROM: ");
    Serial.println(romFile);
  } else {
    Serial.println("Invalid ROM!");
  }

  file.close();
}

bool readCarts() {
  if (!SD.mediaPresent()) { 
    Serial.println("SD card not detected!");
    return false;
  }

  if (!SD.exists("Carts")) {
    SD.mkdir("Carts");
  }

  for (unsigned int i = 0; i < CART_MAX; i++) {
    Carts[i] = "?";
  }

  File CartDirectory = SD.open("Carts");

  unsigned int index = 0;

  while(true) {
    File file = CartDirectory.openNextFile();

    if (file) {
      String filename = String(file.name());
      if (!filename.startsWith(".")) {
        Carts[index] = filename;
        index++;
      }
      file.close();
    } else {
      CartDirectory.close();
      break;
    }
  }

  return true;
}

void listCarts() {
  if (!readCarts()) { return; }

  fileCtx = FILE_CTX_CART;

  Serial.println();

  for (unsigned int j = (cartPage * 8); j < ((cartPage * 8) + 8); j++) {
    Serial.print("(");
    Serial.print(j - (cartPage * 8));
    Serial.print(")");
    Serial.print(" - ");
    Serial.println(Carts[j]);
  }

  Serial.println();
  Serial.print("Page: ");
  Serial.print(cartPage);
  Serial.println(" | ([) Prev Page | Next Page (]) |");
  Serial.println();
}

void loadCart(unsigned int index) {
  String directory = "Carts/";
  String filename = Carts[(cartPage * 8) + index];

  if (!filename.length()) { 
    Serial.println("Invalid Cart! List (C)arts before loading.");
    return;
  }

  String path = directory + filename;

  File file = SD.open(path.c_str());

  if (file) {
    unsigned int i = 0;

    while(file.available()) {
      if (i < 0x4000) { 
        file.read(); // Skip the first 16K
      } else {
        ROM[i] = file.read();
      }
      i++;
    }

    cartFile = Carts[(cartPage * 8) + index];

    Serial.print("Loaded Cart: ");
    Serial.println(cartFile);
  } else {
    Serial.println("Invalid Cart!");
  }

  file.close();
}

void prevPage() {
  if (fileCtx == FILE_CTX_ROM) {
    if (romPage > 0)  {
      romPage--;
    } else {
      romPage = 31;
    }
    listROMs();
  } else {
    if (cartPage > 0)  {
      cartPage--;
    } else {
      cartPage = 31;
    }
    listCarts();
  }
}

void nextPage() {
  if (fileCtx == FILE_CTX_ROM) {
    if (romPage < 31) {
      romPage++;
    } else {
      romPage = 0;
    }
    listROMs();
  } else {
    if (cartPage < 31) {
      cartPage++;
    } else {
      cartPage = 0;
    }
    listCarts();
  }
}

//
// IO
//

byte readIO(word address) {
  if ((address - IO_BANKS[IOBank]) >= TERM_START && (address - IO_BANKS[IOBank]) <= TERM_END) {
    return Terminal::read(address - IO_BANKS[IOBank] - TERM_START);
  } else if ((address - IO_BANKS[IOBank]) >= MOUSE_START && (address - IO_BANKS[IOBank]) <= MOUSE_END) {
    return Mouse::read(address - IO_BANKS[IOBank] - MOUSE_START);
  } else if ((address - IO_BANKS[IOBank]) >= JOY_START && (address - IO_BANKS[IOBank]) <= JOY_END) {
    return Joystick::read(address - IO_BANKS[IOBank] - JOY_START);
  } else if ((address - IO_BANKS[IOBank]) >= RTC_START && (address - IO_BANKS[IOBank]) <= RTC_END) {
    return RTC::read(address - IO_BANKS[IOBank] - RTC_START);
  } else if ((address - IO_BANKS[IOBank]) >= KBD_START && (address - IO_BANKS[IOBank]) <= KBD_END) {
    return Keyboard::read(address - IO_BANKS[IOBank] - KBD_START);
  }

  return 0;
}

void writeIO(word address, byte data) {
  if ((address - IO_BANKS[IOBank]) >= TERM_START && (address - IO_BANKS[IOBank]) <= TERM_END) {
    Terminal::write(address - IO_BANKS[IOBank] - TERM_START, data);
  } else if ((address - IO_BANKS[IOBank]) >= MOUSE_START && (address - IO_BANKS[IOBank]) <= MOUSE_END) {
    Mouse::write(address - IO_BANKS[IOBank] - MOUSE_START, data);
  } else if ((address - IO_BANKS[IOBank]) >= JOY_START && (address - IO_BANKS[IOBank]) <= JOY_END) {
    Joystick::write(address - IO_BANKS[IOBank] - JOY_START, data);
  } else if ((address - IO_BANKS[IOBank]) >= RTC_START && (address - IO_BANKS[IOBank]) <= RTC_END) {
    RTC::write(address - IO_BANKS[IOBank] - RTC_START, data);
  } else if ((address - IO_BANKS[IOBank]) >= KBD_START && (address - IO_BANKS[IOBank]) <= KBD_END) {
    Keyboard::write(address - IO_BANKS[IOBank] - KBD_START, data);
  }
}

//
// INITIALIZATION
//

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

void initRAM() {
  for (word a = 0x0000; a < (RAM_END - RAM_START + 1); a++) {
    RAM[a] = 0x00; 
  }
}

void initROM() {
  for (word a = 0x0000; a < (ROM_END - ROM_START + 1); a++) {
    ROM[a] = 0xEA; // Fill the ROM with 0xEA's by default
  }
}