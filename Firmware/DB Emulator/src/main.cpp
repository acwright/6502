#include <Arduino.h>
#include <TeensyTimerTool.h>
#include <TimeLib.h>
#include <Bounce2.h>
#include <SD.h>
#include <EEPROM.h>
#include <USBHost_t36.h>
#include <vrEmu6502.h>

#include "pins.h"
#include "macros.h"
#include "constants.h"

// #include "IO/terminal.h"
// #include "IO/mouse.h"
// #include "IO/joystick.h"
// #include "IO/rtc.h"
// #include "IO/keyboard.h"

uint8_t RAM[RAM_END - RAM_START + 1];
uint8_t ROM[ROM_END - ROM_START + 1];

Button intButton      = Button();
Button stepButton     = Button();
Button runStopButton  = Button();

USBHost             usb;
USBHub              hub1(usb);
USBHIDParser        hid1(usb);
USBHIDParser        hid2(usb);
USBHIDParser        hid3(usb);

void onTick();
void onCommand(char command);

void info();
void log();
void reset();
void step(); 
void snapshot();
void decreaseFrequency();
void increaseFrequency();
void toggleRunStop();
void toggleRAM();
void toggleIO();
void toggleROM();
bool readROMs();
void listROMs();
void loadROM(unsigned int index);
bool loadROMPath(String path);
bool readCarts();
void listCarts();
void loadCart(unsigned int index);
bool loadCartPath(String path);
void prevPage();
void nextPage();

uint8_t read(uint16_t addr, bool isDbg);
void write(uint16_t addr, uint8_t val);

void initPins();
void initButtons();
void initSD();
void initRAM();
void initROM();

void setDataDirIn();
void setDataDirOut();
uint8_t readData();
void writeData(uint8_t data);
void writeAddress(uint16_t address);

unsigned int freqIndex = 20;      // 1 MHz
bool isRunning = false;
bool isStepping = false;
bool autoStart = false;

bool RAMEnabled = true;
bool IOEnabled = true;
bool ROMEnabled = true;
String romFile = "None";
String cartFile = "None";
bool fileCtx = FILE_CTX_ROM;
uint8_t IOBank = 2;                  // By default, debugger IO bank is $8800

uint16_t address = 0;
uint8_t data = 0;
bool readWrite = HIGH;
bool be = HIGH;
bool rdy = HIGH;
bool sync = HIGH;
bool resb = HIGH;
bool irqb = HIGH;
bool nmib = HIGH;

String ROMs[ROM_MAX];
unsigned int romPage = 0;

String Carts[CART_MAX];
unsigned int cartPage = 0;

TeensyTimerTool::PeriodicTimer timer(TeensyTimerTool::TCK);

VrEmu6502 *cpu = vrEmu6502New(CPU_W65C02, read, write);

//
// MAIN LOOPS
//

void setup() {
  Serial.begin(115200);

  initPins();
  initButtons();
  initRAM();
  initROM();
  initSD();

  usb.begin();

  // Terminal::begin();
  // Mouse::begin();
  // Joystick::begin();
  // RTC::begin();
  // Keyboard::begin();
  
  info();
  reset();

  timer.begin(onTick, FREQ_PERIODS[freqIndex]);

  if (autoStart) {
    toggleRunStop();
  }
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

FASTRUN void onTick() {
  if (!isRunning) { return; }

  vrEmu6502Tick(cpu);
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
  Serial.println("eeee  eeeee eeeeee eeee   888888 888888     88888                                              ");                        
  Serial.println("8  8  8     8    8    8   8    8 8    8     8     eeeeeee e   e e     eeeee eeeee eeeee eeeee  ");
  Serial.println("8     8eeee 8    8    8   8e   8 8eeee8ee   8eeee 8  8  8 8   8 8     8   8   8   8  88 8   8  ");
  Serial.println("8eeee     8 8    8 eee8   88   8 88     8   88    8e 8  8 8e  8 8e    8eee8   8e  8   8 8eee8e ");
  Serial.println("8   8     8 8    8 8      88   8 88     8   88    88 8  8 88  8 88    88  8   88  8   8 88   8 ");
  Serial.println("8eee8 eeee8 8eeee8 8eee   88eee8 88eeeee8   88eee 88 8  8 88ee8 88eee 88  8   88  8eee8 88   8");
  Serial.println();
  Serial.print("6502 DB Emulator | Version: ");
  #ifdef VERSION
  Serial.print(VERSION);
  #endif
  Serial.println();
  Serial.println("---------------------------------");
  Serial.println("| Created by A.C. Wright © 2024 |");
  Serial.println("---------------------------------");
  Serial.println();
  Serial.print("RAM: ");
  Serial.println(RAMEnabled ? "Enabled" : "Disabled");
  Serial.print("ROM: ");
  Serial.print(ROMEnabled ? "Enabled" : "Disabled");
  Serial.print(" | ROM(");
  Serial.print(romFile);
  Serial.print(") |");
  Serial.print(" Cart(");
  Serial.print(cartFile);
  Serial.println(")");
  Serial.print("IO: ");
  Serial.println(IOEnabled ? "Enabled" : "Disabled");
  Serial.print("Frequency: ");
  Serial.println(FREQS[freqIndex]);
  // Serial.print("Date / Time: ");
  // Serial.println(RTC::formattedDateTime());
  Serial.println();
  Serial.println("--------------------------------------------------------------");
  Serial.println("| (R)un / Stop          | (S)tep           | Rese(T)         |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("| Toggle (I)O           | Toggle R(A)M     | Toggle R(O)M    |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("| List RO(M)s / (C)arts |                  |                 |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("| (+/-) Clk Frequency   | Sna(P)shot       | In(F)o / Lo(G)  |");
  Serial.println("--------------------------------------------------------------");
  Serial.println();
}

void log() {
  char output[128];

  sprintf(
    output, 
    "| %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c | %c%c%c%c%c%c%c%c | 0x%04X | 0x%02X | %c |", 
    BYTE_TO_BINARY((address >> 8) & 0xFF),
    BYTE_TO_BINARY(address & 0xFF), 
    BYTE_TO_BINARY(data),
    address, 
    data, 
    readWrite ? 'R' : 'W'
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

  vrEmu6502Reset(cpu);

  Serial.println("Done");

  if (isCurrentlyRunning) {
    toggleRunStop();
  }
}

void step() {
  if (isStepping) { return; }

  if (!isRunning) {
    isStepping = true;
    vrEmu6502InstCycle(cpu);
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
    freqIndex = 21;
  }

  Serial.print("Frequency: ");
  Serial.println(FREQS[freqIndex]);

  timer.setPeriod(FREQ_PERIODS[freqIndex]);
}

void increaseFrequency() {
  if (freqIndex < 21) {
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

  Serial.println(isRunning ? "Running…" : "Stopped");
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

  if (loadROMPath(path)) {
    romFile = ROMs[(romPage * 8) + index];
    cartFile = "None";
    
    Serial.print("Loaded ROM: ");
    Serial.println(romFile);
  } else {
    Serial.println("Invalid ROM!");
  }
}

bool loadROMPath(String path) {
  File file = SD.open(path.c_str());

  if (file) {
    unsigned int i = 0;

    while(file.available()) {
      ROM[i] = file.read();
      i++;
    }

    file.close();

    return true;
  } else {
    file.close();

    return false;
  }
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

  if (loadCartPath(path)) {
    cartFile = Carts[(cartPage * 8) + index];

    Serial.print("Loaded Cart: ");
    Serial.println(cartFile);
  } else {
    Serial.println("Invalid Cart!");
  }
}

bool loadCartPath(String path) {
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

    file.close();

    return true;
  } else {
    file.close();

    return false;
  }
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
// READ / WRITE
//

uint8_t read(uint16_t addr, bool isDbg) {
  address = addr;
  readWrite = HIGH;

  if ((addr >= ROM_CODE) && (addr <= ROM_END) && ROMEnabled) { // ROM
    data = ROM[addr - ROM_START];
    return ROM[addr - ROM_START];
  } else if ((addr >= RAM_START) && (addr <= RAM_END) && RAMEnabled) { // RAM
    data = RAM[addr - RAM_START];
    return RAM[addr - RAM_START];
  } else {
    // For some reason this extra clock pulse is needed only for 65C22 ??
    digitalWriteFast(PHI2, HIGH);
    delayMicroseconds(FREQ_PERIODS[freqIndex]);

    digitalWriteFast(PHI2, LOW);
    writeAddress(address);
    digitalWriteFast(RWB, readWrite);
    setDataDirIn();
    delayMicroseconds(FREQ_PERIODS[freqIndex]);
    digitalWriteFast(PHI2, HIGH);
    delayMicroseconds(FREQ_PERIODS[freqIndex]);
    data = readData();
    digitalWriteFast(PHI2, LOW);
    delayMicroseconds(FREQ_PERIODS[freqIndex]);
    setDataDirOut();

    writeAddress(0xFFFF); // Write address back to ROM space due to above extra clock pulse

    return data;
  }

  return 0x00;

  // if ((address - IO_BANKS[IOBank]) >= TERM_START && (address - IO_BANKS[IOBank]) <= TERM_END) {
  //   return Terminal::read(address - IO_BANKS[IOBank] - TERM_START);
  // } else if ((address - IO_BANKS[IOBank]) >= MOUSE_START && (address - IO_BANKS[IOBank]) <= MOUSE_END) {
  //   return Mouse::read(address - IO_BANKS[IOBank] - MOUSE_START);
  // } else if ((address - IO_BANKS[IOBank]) >= JOY_START && (address - IO_BANKS[IOBank]) <= JOY_END) {
  //   return Joystick::read(address - IO_BANKS[IOBank] - JOY_START);
  // } else if ((address - IO_BANKS[IOBank]) >= RTC_START && (address - IO_BANKS[IOBank]) <= RTC_END) {
  //   return RTC::read(address - IO_BANKS[IOBank] - RTC_START);
  // } else if ((address - IO_BANKS[IOBank]) >= KBD_START && (address - IO_BANKS[IOBank]) <= KBD_END) {
  //   return Keyboard::read(address - IO_BANKS[IOBank] - KBD_START);
  // }
}

void write(uint16_t addr, uint8_t val) {
  address = addr;
  data = val;
  readWrite = LOW;

  if ((addr >= ROM_CODE) && (addr <= ROM_END)) { // ROM
    return;
  } else if ((addr >= RAM_START) && (addr <= RAM_END) && RAMEnabled) { // RAM
    RAM[addr - RAM_START] = data;
  } else {
    // For some reason this extra clock pulse is needed only for 65C22 ??
    digitalWriteFast(PHI2, HIGH);
    delayMicroseconds(FREQ_PERIODS[freqIndex]);

    digitalWriteFast(PHI2, LOW);
    writeAddress(address);
    digitalWriteFast(RWB, readWrite);
    setDataDirOut();
    writeData(data);
    delayMicroseconds(FREQ_PERIODS[freqIndex]);
    digitalWriteFast(PHI2, HIGH);
    delayMicroseconds(FREQ_PERIODS[freqIndex]);
    digitalWriteFast(PHI2, LOW);
    delayMicroseconds(FREQ_PERIODS[freqIndex]);

    writeAddress(0xFFFF); // Write address back to ROM space due to above extra clock pulse
  }

  // if ((address - IO_BANKS[IOBank]) >= TERM_START && (address - IO_BANKS[IOBank]) <= TERM_END) {
  //   Terminal::write(address - IO_BANKS[IOBank] - TERM_START, data);
  // } else if ((address - IO_BANKS[IOBank]) >= MOUSE_START && (address - IO_BANKS[IOBank]) <= MOUSE_END) {
  //   Mouse::write(address - IO_BANKS[IOBank] - MOUSE_START, data);
  // } else if ((address - IO_BANKS[IOBank]) >= JOY_START && (address - IO_BANKS[IOBank]) <= JOY_END) {
  //   Joystick::write(address - IO_BANKS[IOBank] - JOY_START, data);
  // } else if ((address - IO_BANKS[IOBank]) >= RTC_START && (address - IO_BANKS[IOBank]) <= RTC_END) {
  //   RTC::write(address - IO_BANKS[IOBank] - RTC_START, data);
  // } else if ((address - IO_BANKS[IOBank]) >= KBD_START && (address - IO_BANKS[IOBank]) <= KBD_END) {
  //   Keyboard::write(address - IO_BANKS[IOBank] - KBD_START, data);
  // }
}

//
// INITIALIZATION
//

void initPins() {
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(A6, OUTPUT);
  pinMode(A7, OUTPUT);
  pinMode(A8, OUTPUT);
  pinMode(A9, OUTPUT);
  pinMode(A10, OUTPUT);
  pinMode(A11, OUTPUT);
  pinMode(A12, OUTPUT);
  pinMode(A13, OUTPUT);
  pinMode(A14, OUTPUT);
  pinMode(A15, OUTPUT);
  
  writeAddress(0xFFFF);
  setDataDirIn();

  pinMode(RESB, OUTPUT);
  pinMode(SYNC, OUTPUT);
  pinMode(RWB, OUTPUT);
  pinMode(PHI2, OUTPUT);

  digitalWriteFast(RESB, HIGH);
  digitalWriteFast(SYNC, HIGH);
  digitalWriteFast(RWB, HIGH);
  digitalWriteFast(PHI2, LOW);

  pinMode(OE1, OUTPUT);
  pinMode(OE2, OUTPUT);
  pinMode(OE3, OUTPUT);

  digitalWriteFast(OE1, HIGH);
  digitalWriteFast(OE2, HIGH);
  digitalWriteFast(OE3, HIGH);
  
  pinMode(IRQB, INPUT_PULLUP);
  pinMode(NMIB, INPUT_PULLUP);
  pinMode(RDY, INPUT_PULLUP);
  pinMode(BE, INPUT_PULLUP);

  pinMode(CLK_SWB, INPUT);
  pinMode(STEP_SWB, INPUT);
  pinMode(RS_SWB, INPUT);

  pinMode(GPIO0, OUTPUT);
  pinMode(GPIO1, OUTPUT);
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO3, OUTPUT);
}

void initButtons() {
  intButton.attach(CLK_SWB, INPUT_PULLUP);
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

  if (SD.exists("ROM.bin")) {
    loadROMPath("ROM.bin");
    romFile = "ROM.bin";
    autoStart = true;
  }
  if (SD.exists("Cart.bin")) {
    loadCartPath("Cart.bin");
    cartFile = "Cart.bin";
    autoStart = true;
  }
}

void initRAM() {
  for (uint16_t a = 0x0000; a < (RAM_END - RAM_START + 1); a++) {
    RAM[a] = 0x00; 
  }
}

void initROM() {
  for (uint16_t a = 0x0000; a < (ROM_END - ROM_START + 1); a++) {
    ROM[a] = 0xEA;
  }
}

//
// UTILITIES
//

void setDataDirIn() {
  pinMode(D0, INPUT_PULLUP);
  pinMode(D1, INPUT_PULLUP);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D3, INPUT_PULLUP);
  pinMode(D4, INPUT_PULLUP);
  pinMode(D5, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);
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

void writeAddress(uint16_t address) {
  digitalWriteFast(A0, address & 1);
  address = address >> 1;
  digitalWriteFast(A1, address & 1);
  address = address >> 1;
  digitalWriteFast(A2, address & 1);
  address = address >> 1;
  digitalWriteFast(A3, address & 1);
  address = address >> 1;
  digitalWriteFast(A4, address & 1);
  address = address >> 1;
  digitalWriteFast(A5, address & 1);
  address = address >> 1;
  digitalWriteFast(A6, address & 1);
  address = address >> 1;
  digitalWriteFast(A7, address & 1);
  address = address >> 1;
  digitalWriteFast(A8, address & 1);
  address = address >> 1;
  digitalWriteFast(A9, address & 1);
  address = address >> 1;
  digitalWriteFast(A10, address & 1);
  address = address >> 1;
  digitalWriteFast(A11, address & 1);
  address = address >> 1;
  digitalWriteFast(A12, address & 1);
  address = address >> 1;
  digitalWriteFast(A13, address & 1);
  address = address >> 1;
  digitalWriteFast(A14, address & 1);
  address = address >> 1;
  digitalWriteFast(A15, address & 1);
}

uint8_t readData() {
  uint8_t data = 0;

  data = data | digitalReadFast(D7);
  data = data << 1;
  data = data | digitalReadFast(D6);
  data = data << 1;
  data = data | digitalReadFast(D5);
  data = data << 1;
  data = data | digitalReadFast(D4);
  data = data << 1;
  data = data | digitalReadFast(D3);
  data = data << 1;
  data = data | digitalReadFast(D2);
  data = data << 1;
  data = data | digitalReadFast(D1);
  data = data << 1;
  data = data | digitalReadFast(D0);

  return data;
}

void writeData(uint8_t data) {
  digitalWriteFast(D0, data & 1);
  data = data >> 1;
  digitalWriteFast(D1, data & 1);
  data = data >> 1;
  digitalWriteFast(D2, data & 1);
  data = data >> 1;
  digitalWriteFast(D3, data & 1);
  data = data >> 1;
  digitalWriteFast(D4, data & 1);
  data = data >> 1;
  digitalWriteFast(D5, data & 1);
  data = data >> 1;
  digitalWriteFast(D6, data & 1);
  data = data >> 1;
  digitalWriteFast(D7, data & 1);
}