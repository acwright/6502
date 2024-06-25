#include <Arduino.h>
#include <TeensyTimerTool.h>
#include <TimeLib.h>
#include <Bounce2.h>
#include <SD.h>
#include <EEPROM.h>

#if DEVBOARD
#include "devboard.h"
#elif RETROSHIELD
#include "retroshield.h"
#elif RETROSHIELD_ADAPTER
#include "retroshieldadapter.h"
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

void onTick();
void onClock();
void onCommand(char command);

void info();
void log();
void reset();
void step(); 
void snapshot();
void decreaseFrequency();
void increaseFrequency();
void toggleRunStop();
#if DEVBOARD
void toggleOutput();
#endif
void toggleRAM();
void toggleIO();
void toggleIOBank();
void toggleROM();
bool readROMs();
void listROMs();
void loadROM(unsigned int index);
void prevPage();
void nextPage();

byte readIO(word address);
void writeIO(word address, byte data);

void initPins();
void initButtons();
void initSD();
void initRAM();
void initROM();

unsigned int freqIndex = 20;      // 1 MHz
bool isRunning = false;
bool isStepping = false;

bool OutputEnabled = true;
bool RAMEnabled = true;
bool IOEnabled = true;
bool ROMEnabled = true;
String romFile = "None";
byte IOBank = 7;                  // By default, debugger IO bank is $9C00

volatile word address = 0;
volatile byte data = 0;
volatile bool readWrite = HIGH;
volatile bool be = HIGH;
volatile bool rdy = HIGH;
volatile bool sync = LOW;
volatile bool resb = HIGH;
volatile bool irqb = HIGH;
volatile bool nmib = HIGH;

String ROMs[ROM_MAX];
unsigned int romPage = 0;

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
  
  if (!isRunning || digitalReadFast(PHI2) == LOW) {
    // Do secondary processing during clock LOW...
    Terminal::update();
    Mouse::update();
    Joystick::update();
    RTC::update();
    Keyboard::update();
  }
}

//
// EVENTS
//

FASTRUN void onTick() {
  // This is called in the timer interrupt so extra care should be taken!
  
  if (!isRunning) { return; }

  if (!digitalReadFast(PHI2)) {
    digitalWriteFast(PHI2, HIGH);
    onClock();
  } else {
    digitalWriteFast(PHI2, LOW);

    // Delay for data hold time
    delayNanoseconds(20);

    setDataDirIn();
  }
}

FASTRUN void onClock() {
  // Read the address and r/w at the beginning of cycle (low to high transition)
  address     = readAddress();
  readWrite   = digitalReadFast(RWB);
  #if DEVBOARD
  sync        = digitalReadFast(SYNC);
  be          = digitalReadFast(BE);
  #elif RETROSHIELD_ADAPTER
  sync        = digitalReadFast(SYNC);
  #endif

  if (readWrite == HIGH) { // READING
    if ((address >= IO_BANKS[IOBank]) && (address <= IO_BANKS[IOBank] + IO_BANK_SIZE) && IOEnabled) { // IO
      data = readIO(address);
      setDataDirOut();
      writeData(data);
    } else if ((address >= ROM_CODE) && (address <= ROM_END) && ROMEnabled) { // ROM
      data = ROM[address - ROM_START];
      setDataDirOut();
      writeData(data);
    } else if ((address >= RAM_START) && (address <= RAM_END) && RAMEnabled) { // RAM
      data = RAM[address - RAM_START];
      setDataDirOut();
      writeData(data);
    }
  } else { // WRITING
    // Delay for write data setup time
    delayNanoseconds(150);

    data = readData();

    if ((address >= IO_START) && (address <= IO_END) && IOEnabled) { // IO
      writeIO(address, data);
    } else if ((address >= RAM_START) && (address <= RAM_END) && RAMEnabled) { // RAM
      RAM[address - RAM_START] = data;
    }

    // Read the control pins
    resb  = digitalReadFast(RESB);
    nmib  = digitalReadFast(NMIB);
    irqb  = digitalReadFast(IRQB);
    rdy   = digitalReadFast(RDY);
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
    #if DEVBOARD
    case 'u':
    case 'U':
      toggleOutput();
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
  Serial.println("eeee  eeeee eeeeee eeee   eeeeeee eeeeee eeeee e eeeee eeeeee eeeee   ");                        
  Serial.println("8  8  8     8    8    8   8  8  8 8    8 8   8 8   8   8    8 8   8   ");
  Serial.println("8     8eeee 8    8    8   8e 8  8 8    8 8e  8 8e  8e  8    8 8eee8e  ");
  Serial.println("8eeee     8 8    8 eee8   88 8  8 8    8 88  8 88  88  8    8 88   8  ");
  Serial.println("8   8     8 8    8 8      88 8  8 8    8 88  8 88  88  8    8 88   8  ");
  Serial.println("8eee8 eeee8 8eeee8 8eee   88 8  8 8eeee8 88  8 88  88  8eeee8 88   8  ");
  Serial.println();
  Serial.print("6502 Monitor | Version: ");
  #ifdef VERSION
  Serial.print(VERSION);
  #endif
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
  Serial.print("Date / Time: ");
  Serial.println(RTC::formattedDateTime());
  Serial.println();
  Serial.println("-------------------------------------------------------------");
  Serial.println("| (R)un / Stop         | (S)tep           | Rese(T)         |");
  Serial.println("-------------------------------------------------------------");
  Serial.println("| Toggle R(A)M         | Toggle (I)O      | Toggle R(O)M    |");
  Serial.println("-------------------------------------------------------------");
  #if DEVBOARD
  Serial.println("| (L)ist ROMs          | Change IO Ban(K) | Toggle Outp(U)t |");
  #else
  Serial.println("| (L)ist ROMs          | Change IO Ban(K) |                 |");
  #endif
  Serial.println("-------------------------------------------------------------");
  Serial.println("| (+/-) Clk Frequency  | Sna(P)shot       | In(F)o / Lo(G)  |");
  Serial.println("-------------------------------------------------------------");
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

#if DEVBOARD
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
#endif

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

  return true;
}

void listROMs() {
  if (!readROMs()) { return; }

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

//
// IO
//

byte readIO(word address) {
  if ((address - IO_BANKS[IOBank]) >= TERM_START && (address - IO_BANKS[IOBank]) <= TERM_END) {
    return Terminal::read(address - IO_BANKS[IOBank]);
  } else if ((address - IO_BANKS[IOBank]) >= MOUSE_START && (address - IO_BANKS[IOBank]) <= MOUSE_END) {
    return Mouse::read(address - IO_BANKS[IOBank]);
  } else if ((address - IO_BANKS[IOBank]) >= JOY_START && (address - IO_BANKS[IOBank]) <= JOY_END) {
    return Joystick::read(address - IO_BANKS[IOBank]);
  } else if ((address - IO_BANKS[IOBank]) >= RTC_START && (address - IO_BANKS[IOBank]) <= RTC_END) {
    return RTC::read(address - IO_BANKS[IOBank]);
  } else if ((address - IO_BANKS[IOBank]) >= KBD_START && (address - IO_BANKS[IOBank]) <= KBD_END) {
    return Keyboard::read(address - IO_BANKS[IOBank]);
  }

  return 0;
}

void writeIO(word address, byte data) {
  if ((address - IO_BANKS[IOBank]) >= TERM_START && (address - IO_BANKS[IOBank]) <= TERM_END) {
    Terminal::write(address - IO_BANKS[IOBank], data);
  } else if ((address - IO_BANKS[IOBank]) >= MOUSE_START && (address - IO_BANKS[IOBank]) <= MOUSE_END) {
    Mouse::write(address - IO_BANKS[IOBank], data);
  } else if ((address - IO_BANKS[IOBank]) >= JOY_START && (address - IO_BANKS[IOBank]) <= JOY_END) {
    Joystick::write(address - IO_BANKS[IOBank], data);
  } else if ((address - IO_BANKS[IOBank]) >= RTC_START && (address - IO_BANKS[IOBank]) <= RTC_END) {
    RTC::write(address - IO_BANKS[IOBank], data);
  } else if ((address - IO_BANKS[IOBank]) >= KBD_START && (address - IO_BANKS[IOBank]) <= KBD_END) {
    Keyboard::write(address - IO_BANKS[IOBank], data);
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