#include <Arduino.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <Bounce2.h>
#include <SD.h>
#include <EEPROM.h>
#include <USBHost_t36.h>
#include <QNEthernet.h>
#include <AsyncWebServer_Teensy41.h>
#include <6502.h>

using namespace qindesign::network;

Button intButton      = Button();
Button stepButton     = Button();
Button runStopButton  = Button();
#ifdef DEVBOARD_1
Button resetButton    = Button();
#endif

USBHost             usb;
USBHub              hub1(usb);
USBHIDParser        hid1(usb);
USBHIDParser        hid2(usb);
USBHIDParser        hid3(usb);

KeyboardController  keyboard(usb);
JoystickController  joystick(usb);

EthernetClient      ethClient;
AsyncWebServer      server(80);

void onCommand(char command);
void onNumeric(uint8_t num);
void onKeyboard(int key);
void onKeyboardRelease(int key);
void onJoystick();

void info();
void log();
void reset();
void tick();
void step();
void dump();
void snapshot();
void decreaseFrequency();
void increaseFrequency();
void toggleRunStop();
void toggleRAM();
void toggleROM();
void toggleCart();
void toggleIO();
void readROMs();
void listROMs();
void loadROM(uint index);
void loadROMFile(String filename);
void loadROMPath(String path);
void readCarts();
void listCarts();
void loadCart(uint index);
void loadCartFile(String filename);
void loadCartPath(String path);
void readPrograms();
void listPrograms();
void loadProgram(uint index);
void loadProgramFile(String filename);
void loadProgramPath(String path);
void listFiles();
void prevPage();
void nextPage();

uint8_t read(uint16_t addr, bool isDbg);
void write(uint16_t addr, uint8_t val);

void initPins();
void initButtons();
void initSD();
void initUSB();
void initEthernet();
void initServer();
void initFiles();

void buildMemoryMap();

time_t syncTime();
String formattedDateTime();
time_t lastSnapshot;

void onServerRoot(AsyncWebServerRequest *request);
void onServerInfo(AsyncWebServerRequest *request);
void onServerMemory(AsyncWebServerRequest *request);
void onServerStorage(AsyncWebServerRequest *request);
void onServerLoad(AsyncWebServerRequest *request);
void onServerControl(AsyncWebServerRequest *request);
void onServerNotFound(AsyncWebServerRequest *request);

uint8_t freqIndex = FREQ_SIZE - 1;
bool isRunning = false;
bool isStepping = false;
bool autoStart = false;

uint8_t inputCtx = INPUT_CTX_ROM;

uint16_t address = 0;
uint8_t data = 0;
bool readWrite = HIGH;

String ROMs[FILE_MAX];
uint romFilePage = 0;
String Carts[FILE_MAX];
uint cartFilePage = 0;
String Programs[FILE_MAX];
uint programFilePage = 0;

uint memoryPage = 0;

#ifdef MEM_EXTMEM
EXTMEM __attribute__((aligned(32))) uint8_t ramData1[RC_BLOCK_SIZE * RC_BLOCK_COUNT];
EXTMEM __attribute__((aligned(32))) uint8_t ramData2[RC_BLOCK_SIZE * RC_BLOCK_COUNT];
#else
__attribute__((aligned(32))) uint8_t ramData1[RC_BLOCK_SIZE * RC_BLOCK_COUNT];
__attribute__((aligned(32))) uint8_t ramData2[RC_BLOCK_SIZE * RC_BLOCK_COUNT];
#endif

static uint32_t cachedDelay = 0;
static uint8_t cachedFreqIndex = 0xFF;
static uint32_t cachedCpuFrequency = 1000000;
static uint8_t cachedCpuFreqIndex = 0xFF;

enum MemoryRegion : uint8_t {
  REGION_CART_CODE,
  REGION_ROM,
  REGION_RAM,
  REGION_IO,
  REGION_NONE
};
static MemoryRegion memoryMap[256]; // 256 possible high bytes

CPU cpu = CPU(read, write);
RAM ram = RAM();
ROM rom = ROM();
Cart cart = Cart();

RAMCard ramCard1 = RAMCard(ramData1);
RAMCard ramCard2 = RAMCard(ramData2);
RTCCard rtcCard = RTCCard();
StorageCard storageCard = StorageCard();
SerialCard serialCard = SerialCard();
GPIOCard gpioCard = GPIOCard();

// GPIO Attachments
GPIOKeyboardMatrixAttachment kbdMatrixAttachment = GPIOKeyboardMatrixAttachment(10);    // Priority 10
GPIOKeyboardEncoderAttachment kbdEncoderAttachment = GPIOKeyboardEncoderAttachment(20); // Priority 20
GPIOPS2Attachment ps2Attachment = GPIOPS2Attachment(30);                                // Priority 30
GPIOJoystickAttachment joystickAttachment = GPIOJoystickAttachment(false, 100);         // Port B, Priority 100

//
// MAIN
//

void setup() {
  Serial.begin(9600);       // Ignored by Teensy; Baud rate is USB rate 480Mbps
  SerialUSB1.begin(9600);   // Ignored by Teensy; Baud rate is USB rate 480Mbps

  setSyncProvider(syncTime);
  buildMemoryMap();
  
  // Attach peripherals to GPIO Card
  // Keyboard matrix (manual scanning) - highest priority for Port A rows
  gpioCard.attachToPortA(&kbdMatrixAttachment);
  gpioCard.attachToPortB(&kbdMatrixAttachment);
  
  // Keyboard encoder (ASCII on Port B) - medium priority
  gpioCard.attachToPortB(&kbdEncoderAttachment);
  
  // PS/2 keyboard (ASCII on Port A) - medium priority
  gpioCard.attachToPortA(&ps2Attachment);
  
  // Joystick (Port B) - lowest priority, fallback
  gpioCard.attachToPortB(&joystickAttachment);
  
  initPins();
  initButtons();
  initSD();
  initUSB();
  initEthernet();
  initServer();
  initFiles();

  readROMs();
  readCarts();
  readPrograms();

  info();

  if (autoStart) {
    delay(500);
    reset();
    toggleRunStop();
  }
}

void loop() {
  usb.Task();

  intButton.update();
  stepButton.update();
  runStopButton.update();
  #ifdef DEVBOARD_1
  resetButton.update();
  #endif

  if (intButton.pressed()) {
    increaseFrequency();
  }
  if (stepButton.pressed()) {
    step();
  }
  if (runStopButton.pressed()) {
    toggleRunStop();
  }
  #ifdef DEVBOARD_1
  if (resetButton.pressed()) {
    reset();
  }
  #endif

  if (joystick.available()) {
    onJoystick();
  }
  if (Serial.available()) {
    onCommand(Serial.read());
  }

  if (isRunning) {
    tick();
  }
}

//
// EVENTS
//

void onCommand(char command) {
  switch (command) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7': {
      if (!SD.mediaPresent()) { Serial.println("SD Card Not Detected!"); break; }
      uint8_t index = String(command).toInt();
      onNumeric(index);
      switch(inputCtx) {
        case INPUT_CTX_ROM:
          Serial.print("Loaded ROM: ");
          Serial.println(rom.file);
          break;
        case INPUT_CTX_CART:
          Serial.print("Loaded Cart: ");
          Serial.println(cart.file);
          break;
        case INPUT_CTX_PROG:
          Serial.print("Loaded Program: ");
          Serial.println(ram.file);
          break;
        case INPUT_CTX_MEM:
          break;
        default:
          break;
      }
      break;
    }
    case 'a':
    case 'A':
      toggleRAM();
      Serial.print("RAM: ");
      Serial.println(ram.enabled ? "Enabled" : "Disabled");
      break;
    case 'c':
    case 'C':
      if (!SD.mediaPresent()) { Serial.println("SD Card Not Detected!"); break; }
      readCarts();
      listCarts();
      break;
    case 'd':
    case 'D':
      dump();
      break;
    case 'f':
    case 'F':
      info();
      break;
    case 'g':
    case 'G':
      log();
      break;
    case 'k':
    case 'K':
      tick();
      log();
      break;
    case 'l':
    case 'L':
      toggleCart();
      Serial.print("Cart: ");
      Serial.println(cart.enabled ? "Enabled" : "Disabled");
      break;
    case 'm':
    case 'M':
      if (!SD.mediaPresent()) { Serial.println("SD Card Not Detected!"); break; }
      readROMs();
      listROMs();
      break;
    case 'o':
    case 'O':
      toggleROM();
      Serial.print("ROM: ");
      Serial.println(rom.enabled ? "Enabled" : "Disabled");
      break;
    case 'p':
    case 'P':
      if (!SD.mediaPresent()) { Serial.println("SD Card Not Detected!"); break; }
      Serial.print("Creating snapshot... ");
      snapshot();
      Serial.print("Success! (");
      Serial.print(lastSnapshot);
      Serial.println(")");
      break;
    case 'r':
    case 'R':
      toggleRunStop();
      Serial.println(isRunning ? "Running…" : "Stopped");
      break;
    case 's':
    case 'S':
      if (!isRunning) {
        step();
        log();
      } else {
        Serial.println("Clock must be stopped before stepping!");
      }
      break;
    case 't':
    case 'T':
      Serial.print("Resetting... ");
      reset();
      Serial.println("Done");
      break;
    case 'u':
    case 'U':
      if (!SD.mediaPresent()) { Serial.println("SD Card Not Detected!"); break; }
      readPrograms();
      listPrograms();
      break;
    case '-':
      decreaseFrequency();
      Serial.print("Frequency: ");
      Serial.println(FREQ_LABELS[freqIndex]);
      break;
    case '+':
      increaseFrequency();
      Serial.print("Frequency: ");
      Serial.println(FREQ_LABELS[freqIndex]);
      break;
    case '[':
      switch(inputCtx) {
        case INPUT_CTX_MEM:
          prevPage();
          dump();
          break;
        default:          
          prevPage();
          listFiles();
          break;
      }
      break;
    case ']':
      switch(inputCtx) {
        case INPUT_CTX_MEM:
          nextPage();
          dump();
          break;
        default:          
          nextPage();
          listFiles();
          break;
      }
      break;
  }
}

void onNumeric(uint8_t num) {
  switch(inputCtx) {
    case INPUT_CTX_ROM:
      loadROM(num);
      break;
    case INPUT_CTX_CART:
      loadCart(num);
      break;
    case INPUT_CTX_PROG:
      loadProgram(num);
      break;
    case INPUT_CTX_MEM:
      break;
  }
}

void onKeyboard(int key) {
  // Fix some quirks in the USBHost_t36 lib and add our own
  // Rewrites are equivalent to CoolTerm TX
  switch (key) {
    case 0x7F: // BACKSPACE
      key = 0x08;
      break;
    // 0xC2 - 0xCD = F1-F12 (Not handled)
    case 0xD1: // INSERT
      key = 0x1A;
      break;
    case 0xD2: // HOME
      key = 0x01;
      break;
    case 0xD3: // PG UP (Not handled)
      break;
    case 0xD4: // DELETE
      key = 0x7F;
      break;
    case 0xD5: // END
      key = 0x04;
      break;
    case 0xD6: // PG DOWN (Not handled)
      break;
    case 0xD7: // RIGHT ARROW
      key = 0x1D;
      break;
    case 0xD8: // LEFT ARROW
      key = 0x1C;
      break;
    case 0xD9: // DOWN ARROW
      key = 0x1F;
      break;
    case 0xDA: // UP ARROW
      key = 0x1E;
      break;
  }

  if (key > 0x7F) { return; } // No extended ASCII

  // Route keyboard input to attachments
  kbdMatrixAttachment.updateKey((uint8_t)key, true);  // Update keyboard matrix
  kbdEncoderAttachment.updateKeyboard((uint8_t)key);  // Keyboard encoder on Port B
  ps2Attachment.updatePS2Keyboard((uint8_t)key);      // PS/2 port on Port A
  
  #ifdef KEYBOARD_DEBUG
  Serial.print("Key pressed: 0x");
  Serial.println(key, HEX);
  #endif
}

void onKeyboardRelease(int key) {
  // Apply same key mapping as press handler
  switch (key) {
    case 0x7F:
      key = 0x08;
      break;
    case 0xD1:
      key = 0x1A;
      break;
    case 0xD2:
      key = 0x01;
      break;
    case 0xD4:
      key = 0x7F;
      break;
    case 0xD5:
      key = 0x04;
      break;
    case 0xD7:
      key = 0x1D;
      break;
    case 0xD8:
      key = 0x1C;
      break;
    case 0xD9:
      key = 0x1F;
      break;
    case 0xDA:
      key = 0x1E;
      break;
  }

  if (key > 0x7F) { return; }

  // Release key from keyboard matrix
  kbdMatrixAttachment.updateKey((uint8_t)key, false);
  // Note: PS/2 and encoder use interrupt-driven single-key buffer,
  // so key release doesn't need separate handling
  
  #ifdef KEYBOARD_DEBUG
  Serial.print("Key Released: 0x");
  Serial.println(key, HEX);
  #endif
}

void onJoystick() {
  uint32_t buttons = joystick.getButtons();
  
  if (joystick.joystickType() == JoystickController::XBOX360 || 
      joystick.joystickType() == JoystickController::XBOXONE) 
  {
    // Map Xbox controller to 8-bit joystick format
    // Xbox button layout: bit 0=A, 1=B, 2=X, 3=Y, 4=LB, 5=RB, 6=Back, 7=Start, etc.
    // DB Joystick: bit 0=A, 1=B, 2=X, 3=Y, 4=U, 5=D, 6=L, 7=R
    
    uint8_t mappedButtons = 0;
    
    // Map face buttons (A, B, X, Y from Xbox to DB format)
    if (buttons & 0x0001) mappedButtons |= 0x01;  // Xbox A -> DB A (bit 0)
    if (buttons & 0x0002) mappedButtons |= 0x02;  // Xbox B -> DB B (bit 1)
    if (buttons & 0x0004) mappedButtons |= 0x04;  // Xbox X -> DB X (bit 2)
    if (buttons & 0x0008) mappedButtons |= 0x08;  // Xbox Y -> DB Y (bit 3)
    
    // Map D-Pad or analog stick to directions
    // Get analog stick positions for directional control
    int axisX = joystick.getAxis(0);  // Left stick X
    int axisY = joystick.getAxis(1);  // Left stick Y
    
    // Check D-Pad buttons (typically bits 12-15 on Xbox controllers)
    bool dpadUp    = (buttons & 0x0100) != 0;
    bool dpadDown  = (buttons & 0x0200) != 0;
    bool dpadLeft  = (buttons & 0x0400) != 0;
    bool dpadRight = (buttons & 0x0800) != 0;
    
    // Combine D-Pad and analog stick (analog stick uses threshold)
    const int analogThreshold = 128;  // Threshold for analog stick direction
    
    if (dpadUp || axisY < -analogThreshold)    mappedButtons |= 0x10;  // UP (bit 4)
    if (dpadDown || axisY > analogThreshold)   mappedButtons |= 0x20;  // DOWN (bit 5)
    if (dpadLeft || axisX < -analogThreshold)  mappedButtons |= 0x40;  // LEFT (bit 6)
    if (dpadRight || axisX > analogThreshold)  mappedButtons |= 0x80;  // RIGHT (bit 7)
    
    joystickAttachment.updateJoystick(mappedButtons);
    
    #ifdef JOYSTICK_DEBUG
    Serial.print("Joystick: Raw=0x");
    Serial.print(buttons, HEX);
    Serial.print(" Mapped=0x");
    Serial.print(mappedButtons, HEX);
    Serial.print(" X=");
    Serial.print(axisX);
    Serial.print(" Y=");
    Serial.println(axisY);
    #endif
  }

  joystick.joystickDataClear();
}

//
// METHODS
//

void info() {
  Serial.println();
  Serial.println("888888 888888     88888                                              ");                        
  Serial.println("8    8 8    8     8     eeeeeee e   e e     eeeee eeeee eeeee eeeee  ");
  Serial.println("8e   8 8eeee8ee   8eeee 8  8  8 8   8 8     8   8   8   8  88 8   8  ");
  Serial.println("88   8 88     8   88    8e 8  8 8e  8 8e    8eee8   8e  8   8 8eee8e ");
  Serial.println("88   8 88     8   88    88 8  8 88  8 88    88  8   88  8   8 88   8 ");
  Serial.println("88eee8 88eeeee8   88eee 88 8  8 88ee8 88eee 88  8   88  8eee8 88   8");
  Serial.println();
  Serial.print("DB Emulator | Version: ");
  Serial.print(VERSION);
  Serial.println();
  Serial.println("---------------------------------");
  Serial.println("| Created by A.C. Wright © 2024 |");
  Serial.println("---------------------------------");
  Serial.println();
  Serial.print("RAM: ");
  Serial.print(ram.enabled ? "Enabled" : "Disabled");
  Serial.print(" (");
  Serial.print(ram.file);
  Serial.println(")");
  Serial.print("ROM: ");
  Serial.print(rom.enabled ? "Enabled" : "Disabled");
  Serial.print(" (");
  Serial.print(rom.file);
  Serial.println(")");
  Serial.print("Cart: ");
  Serial.print(cart.enabled ? "Enabled" : "Disabled");
  Serial.print(" (");
  Serial.print(cart.file);
  Serial.println(")");
  Serial.print("Frequency: ");
  Serial.println(FREQ_LABELS[freqIndex]);
  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Date / Time: ");
  Serial.println(formattedDateTime());
  Serial.println();
  Serial.println("--------------------------------------------------------------");
  Serial.println("| (R)un / Stop       | (S)tep / Tic(K)     | Rese(T)         |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("| Toggle R(A)M       | Toggle R(O)M        | Toggle Cart (L) |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("| (+/-) Clk Freq     | (D)ump / Sna(P)shot | In(F)o / Lo(G)  |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("|          List RO(M)s / (C)arts / (U)ser Programs           |");
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

  cpu.reset();

  digitalWriteFast(RESB, LOW);
  delay(100);
  digitalWriteFast(RESB, HIGH);
  delay(100);

  if (isCurrentlyRunning) {
    toggleRunStop();
  }
}

void tick() {
  // SYNC signal indicates opcode fetch cycle (65C02 behavior)
  // SYNC goes HIGH during opcode fetch (when step == 0)
  // Set SYNC before the tick to match hardware timing
  if (cpu.opcodeCycle() == 0) {
    digitalWriteFast(SYNC, HIGH);
  } else {
    digitalWriteFast(SYNC, LOW);
  }

  if (freqIndex != cachedCpuFreqIndex) {
    cachedCpuFrequency = (uint32_t)(1000000.0 / FREQ_PERIODS[freqIndex]);
    cachedCpuFreqIndex = freqIndex;
  }

  uint8_t interrupt = 0x00;
  if (digitalReadFast(IRQB) == LOW) interrupt |= 0x80;
  if (digitalReadFast(NMIB) == LOW) interrupt |= 0x40;

  cpu.tick();

  // Tick IO cards and accumulate interrupts
  interrupt |= rtcCard.tick(cachedCpuFrequency);
  interrupt |= storageCard.tick(cachedCpuFrequency);
  interrupt |= serialCard.tick(cachedCpuFrequency);
  interrupt |= gpioCard.tick(cachedCpuFrequency);
  
  // Single interrupt processing
  if (interrupt & 0x40) {
    cpu.nmiTrigger();
  }
  if (interrupt & 0x80) {
    cpu.irqTrigger();
  } else {
    cpu.irqClear();
  }
}

void step() {
  // Execute one complete instruction and handle ticks/interrupts
  uint8_t ticks = cpu.step();

  // SYNC signal indicates opcode fetch cycle (65C02 behavior)
  // After step() completes, we're at the start of the next instruction
  // Set SYNC HIGH for the next opcode fetch
  if (cpu.opcodeCycle() == 0) {
    digitalWriteFast(SYNC, HIGH);
  } else {
    digitalWriteFast(SYNC, LOW);
  }

  // Process interrupts for each tick of the instruction
  for(uint i = 0; i < ticks; i++) {
    uint8_t interrupt = 0x00;

    // Check for external interrupt
    if (digitalReadFast(IRQB) == LOW) {
      interrupt |= 0x80;
    }
    if (digitalReadFast(NMIB) == LOW) {
      interrupt |= 0x40;
    }

    // Calculate CPU frequency from the period (period is in microseconds)
    uint32_t cpuFrequency = (uint32_t)(1000000.0 / FREQ_PERIODS[freqIndex]);

    // Tick IO and check for interrupt
    interrupt |= rtcCard.tick(cpuFrequency);
    interrupt |= storageCard.tick(cpuFrequency);
    interrupt |= serialCard.tick(cpuFrequency);
    interrupt |= gpioCard.tick(cpuFrequency);

    if ((interrupt & 0x40) != 0x00) {
      cpu.nmiTrigger();
    }
    if ((interrupt & 0x80) != 0x00) {
      cpu.irqTrigger();
    } else {
      cpu.irqClear();
    }
  }
}

void dump() {
  inputCtx = INPUT_CTX_MEM;

  Serial.println();
  Serial.println("RAM Dump:");
  Serial.println();

  char ascii[16];

  for (size_t i = 0; i < MEM_PER_PAGE; i++) {
    size_t index = i % 16;
    uint8_t value = ram.data[i + (memoryPage * MEM_PER_PAGE)];

    if (index == 0) {
      Serial.print("$");
      Serial.printf("%04X ", (memoryPage * MEM_PER_PAGE) + i);
      Serial.print(": ");
    }

    Serial.printf("%02X ", value);
    ascii[index] = (value >= 0x20 && value <= 0x7E) ? (char)value : '.';

    if (index == 15) {
      Serial.print("| ");
      Serial.print(ascii);
      Serial.println(" |");
    }
  }

  Serial.println();
  Serial.print("Page: ");
  Serial.print(memoryPage);
  Serial.println(" | ([) Prev Page | Next Page (]) |");
  Serial.println();
}

void snapshot() {
  if (!SD.mediaPresent()) { return; }
  if (!SD.exists("Snapshots")) { SD.mkdir("Snapshots"); }

  lastSnapshot = now();
  String path = "Snapshots/";
  String IOpath = "Snapshots/";

  path.append(lastSnapshot);
  path.append(".bin");

  File snapshot = SD.open(path.c_str(), FILE_WRITE);

  if (snapshot) {
    for(uint i = 0; i < (RAM_END - RAM_START); i++) {
      snapshot.write(ram.read(i));
    }
    snapshot.close();
  }
}

void decreaseFrequency() {
  if (freqIndex > 0) {
    freqIndex--;
  } else {
    freqIndex = FREQ_SIZE - 1;
  }
}

void increaseFrequency() {
  if (freqIndex < (FREQ_SIZE - 1)) {
    freqIndex++;
  } else {
    freqIndex = 0;
  }
}

void toggleRunStop() {
  isRunning = !isRunning;
}

void toggleRAM() {
  ram.enabled = !ram.enabled;
}

void toggleROM() {
  rom.enabled = !rom.enabled;
}

void toggleCart() {
  cart.enabled = !cart.enabled;
}

void readROMs() {
  if (!SD.mediaPresent()) { return; }
  if (!SD.exists("ROMs")) { SD.mkdir("ROMs"); }

  inputCtx = INPUT_CTX_ROM;

  for (uint i = 0; i < FILE_MAX; i++) {
    ROMs[i] = "?";
  }

  File ROMDirectory = SD.open("ROMs");

  uint index = 0;

  while(true) {
    if (index >= FILE_MAX) {
      ROMDirectory.close();
      break;
    }

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
  Serial.println();
  Serial.println("ROMs:");
  Serial.println();

  for (uint j = (romFilePage * FILE_PER_PAGE); j < ((romFilePage * FILE_PER_PAGE) + FILE_PER_PAGE); j++) {
    Serial.print("(");
    Serial.print(j - (romFilePage * FILE_PER_PAGE));
    Serial.print(")");
    Serial.print(" - ");
    Serial.println(ROMs[j]);
  }

  Serial.println();
  Serial.print("Page: ");
  Serial.print(romFilePage);
  Serial.println(" | ([) Prev Page | Next Page (]) |");
  Serial.println();
}

void loadROM(uint index) {
  loadROMFile(ROMs[(romFilePage * FILE_PER_PAGE) + index]);
}

void loadROMFile(String filename) {
  if (!filename.length()) { return; }

  rom.file = filename;

  loadROMPath("ROMs/" + filename);
}

void loadROMPath(String path) {
  if (!SD.mediaPresent()) { return; }
  
  File file = SD.open(path.c_str());

  if (file) {
    uint i = 0;

    while(file.available()) {
      rom.write(i, file.read());
      i++;
    }
  }

  file.close();
}

void readCarts() {
  if (!SD.mediaPresent()) { return; }
  if (!SD.exists("Carts")) { SD.mkdir("Carts"); }

  inputCtx = INPUT_CTX_CART;

  for (uint i = 0; i < FILE_MAX; i++) {
    Carts[i] = "?";
  }

  File CartDirectory = SD.open("Carts");

  uint index = 0;

  while(true) {
    if (index >= FILE_MAX) {
      CartDirectory.close();
      break;
    }

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
}

void listCarts() {
  Serial.println();
  Serial.println("Carts:");
  Serial.println();

  for (uint j = (cartFilePage * FILE_PER_PAGE); j < ((cartFilePage * FILE_PER_PAGE) + FILE_PER_PAGE); j++) {
    Serial.print("(");
    Serial.print(j - (cartFilePage * FILE_PER_PAGE));
    Serial.print(")");
    Serial.print(" - ");
    Serial.println(Carts[j]);
  }

  Serial.println();
  Serial.print("Page: ");
  Serial.print(cartFilePage);
  Serial.println(" | ([) Prev Page | Next Page (]) |");
  Serial.println();
}

void loadCart(uint index) {
  loadCartFile(Carts[(cartFilePage * FILE_PER_PAGE) + index]);
}

void loadCartFile(String filename) {
  if (!filename.length()) { return; }

  cart.file = filename;

  loadCartPath("Carts/" + filename);
}

void loadCartPath(String path) {
  if (!SD.mediaPresent()) { return; }

  File file = SD.open(path.c_str());

  if (file) {
    uint i = 0;

    while(file.available()) {
      if (i < 0x4000) { 
        file.read(); // Skip the first 16K
      } else {
        cart.load(i, file.read());
      }
      i++;
    }
  }

  file.close();
}

void readPrograms() {
  if (!SD.mediaPresent()) { return; }
  if (!SD.exists("Programs")) { SD.mkdir("Programs"); }

  inputCtx = INPUT_CTX_PROG;

  for (uint i = 0; i < FILE_MAX; i++) {
    Programs[i] = "?";
  }

  File ProgramDirectory = SD.open("Programs");

  uint index = 0;

  while(true) {
    if (index >= FILE_MAX) {
      ProgramDirectory.close();
      break;
    }

    File file = ProgramDirectory.openNextFile();

    if (file) {
      String filename = String(file.name());
      if (!filename.startsWith(".")) {
        Programs[index] = filename;
        index++;
      }
      file.close();
    } else {
      ProgramDirectory.close();
      break;
    }
  }
}

void listPrograms() {
  Serial.println();
  Serial.println("Programs:");
  Serial.println();

  for (uint j = (programFilePage * FILE_PER_PAGE); j < ((programFilePage * FILE_PER_PAGE) + FILE_PER_PAGE); j++) {
    Serial.print("(");
    Serial.print(j - (programFilePage * FILE_PER_PAGE));
    Serial.print(")");
    Serial.print(" - ");
    Serial.println(Programs[j]);
  }

  Serial.println();
  Serial.print("Page: ");
  Serial.print(programFilePage);
  Serial.println(" | ([) Prev Page | Next Page (]) |");
  Serial.println();
}

void loadProgram(uint index) {
  loadProgramFile(Programs[(programFilePage * FILE_PER_PAGE) + index]);
}

void loadProgramFile(String filename) {
  if (!filename.length()) { return; }

  ram.file = filename;

  loadProgramPath("Programs/" + filename);
}

void loadProgramPath(String path) {
  if (!SD.mediaPresent()) { return; }

  File file = SD.open(path.c_str());

  if (file) {
    uint i = 0x0800; // Load programs into user space at 0x0800

    while(file.available()) {
      ram.write(i, file.read());
      i++;
    }
  }

  file.close();
}

void listFiles() {
  switch(inputCtx) {
    case INPUT_CTX_ROM:
      listROMs();
      break;
    case INPUT_CTX_CART:
      listCarts();
      break;
    case INPUT_CTX_PROG:
      listPrograms();
      break;
    case INPUT_CTX_MEM:
      break;
    default:
      break;
  }
}

void prevPage() {
  switch(inputCtx) {
    case INPUT_CTX_ROM:
      if (romFilePage > 0)  {
        romFilePage--;
      } else {
        romFilePage = ((FILE_MAX / FILE_PER_PAGE) - 1);
      }
      break;
    case INPUT_CTX_CART:
      if (cartFilePage > 0)  {
        cartFilePage--;
      } else {
        cartFilePage = ((FILE_MAX / FILE_PER_PAGE) - 1);
      }
      break;
    case INPUT_CTX_PROG:
      if (programFilePage > 0)  {
        programFilePage--;
      } else {
        programFilePage = ((FILE_MAX / FILE_PER_PAGE) - 1);
      }
      break;
    case INPUT_CTX_MEM:
      if (memoryPage > 0)  {
        memoryPage--;
      } else {
        memoryPage = ((MEM_MAX / MEM_PER_PAGE) - 1);
      }
      break;
    default:
      break;
  }
}

void nextPage() {
  switch(inputCtx) {
    case INPUT_CTX_ROM:
      if (romFilePage < ((FILE_MAX / FILE_PER_PAGE) - 1)) {
        romFilePage++;
      } else {
        romFilePage = 0;
      }
      break;
    case INPUT_CTX_CART:
      if (cartFilePage < ((FILE_MAX / FILE_PER_PAGE) - 1)) {
        cartFilePage++;
      } else {
        cartFilePage = 0;
      }
      break;
    case INPUT_CTX_PROG:
      if (programFilePage < ((FILE_MAX / FILE_PER_PAGE) - 1)) {
        programFilePage++;
      } else {
        programFilePage = 0;
      }
      break;
    case INPUT_CTX_MEM:
      if (memoryPage < ((MEM_MAX / MEM_PER_PAGE) - 1)) {
        memoryPage++;
      } else {
        memoryPage = 0;
      }
      break;
    default:
      break;
  }
}

//
// UTILITIES
//

static inline __attribute__((always_inline)) void setDataDirIn() {
  pinMode(D0, INPUT_PULLUP);
  pinMode(D1, INPUT_PULLUP);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D3, INPUT_PULLUP);
  pinMode(D4, INPUT_PULLUP);
  pinMode(D5, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);
}

static inline __attribute__((always_inline)) void setDataDirOut() {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
}

static inline __attribute__((always_inline)) void writeAddress(uint16_t address) {
  digitalWriteFast(A0,  (address >> 0)  & 1);
  digitalWriteFast(A1,  (address >> 1)  & 1);
  digitalWriteFast(A2,  (address >> 2)  & 1);
  digitalWriteFast(A3,  (address >> 3)  & 1);
  digitalWriteFast(A4,  (address >> 4)  & 1);
  digitalWriteFast(A5,  (address >> 5)  & 1);
  digitalWriteFast(A6,  (address >> 6)  & 1);
  digitalWriteFast(A7,  (address >> 7)  & 1);
  digitalWriteFast(A8,  (address >> 8)  & 1);
  digitalWriteFast(A9,  (address >> 9)  & 1);
  digitalWriteFast(A10, (address >> 10) & 1);
  digitalWriteFast(A11, (address >> 11) & 1);
  digitalWriteFast(A12, (address >> 12) & 1);
  digitalWriteFast(A13, (address >> 13) & 1);
  digitalWriteFast(A14, (address >> 14) & 1);
  digitalWriteFast(A15, (address >> 15) & 1);
}

static inline __attribute__((always_inline)) uint8_t readData() {
  uint8_t data = 0;
  data |= digitalReadFast(D0) << 0;
  data |= digitalReadFast(D1) << 1;
  data |= digitalReadFast(D2) << 2;
  data |= digitalReadFast(D3) << 3;
  data |= digitalReadFast(D4) << 4;
  data |= digitalReadFast(D5) << 5;
  data |= digitalReadFast(D6) << 6;
  data |= digitalReadFast(D7) << 7;
  return data;
}

static inline __attribute__((always_inline)) void writeData(uint8_t data) {
  digitalWriteFast(D0, (data >> 0) & 1);
  digitalWriteFast(D1, (data >> 1) & 1);
  digitalWriteFast(D2, (data >> 2) & 1);
  digitalWriteFast(D3, (data >> 3) & 1);
  digitalWriteFast(D4, (data >> 4) & 1);
  digitalWriteFast(D5, (data >> 5) & 1);
  digitalWriteFast(D6, (data >> 6) & 1);
  digitalWriteFast(D7, (data >> 7) & 1);
}

//
// READ / WRITE
//

FASTRUN uint8_t read(uint16_t addr, bool isDbg) {
  address = addr;
  data = 0x00;
  readWrite = HIGH;

  if (cachedFreqIndex != freqIndex) {
    cachedFreqIndex = freqIndex;
    cachedDelay = FREQ_PERIODS[freqIndex] <= 1 ? 
                  (FREQ_PERIODS[freqIndex] * 1000) / 2 : 
                  FREQ_PERIODS[freqIndex] / 2;
  }
  
  // WDC 65C02 read cycle timing:
  // - Address and R/W̅ must be stable before PHI2 rises
  // - Data is valid during PHI2 high
  digitalWriteFast(PHI2, LOW);
  delayNanoseconds(20); // tADS: Address setup time before PHI2 rise
  writeAddress(address);
  digitalWriteFast(RWB, readWrite);
  setDataDirIn();
  
  // RDY handling: 65C02 samples RDY during PHI1 (PHI2 LOW) on read cycles only
  // If RDY is LOW, wait here until it goes HIGH before continuing
  // This allows external hardware to stretch the read cycle
  if (!isDbg) {
    while (digitalReadFast(RDY) == LOW) {
      // Wait for RDY to go HIGH - CPU is halted during read cycle
      // Write cycles are not affected by RDY (handled elsewhere)
    }
  }
  
  FREQ_PERIODS[freqIndex] <= 1 ? delayNanoseconds(cachedDelay) : delayMicroseconds(cachedDelay);
  digitalWriteFast(PHI2, HIGH);
  FREQ_PERIODS[freqIndex] <= 1 ? delayNanoseconds(cachedDelay) : delayMicroseconds(cachedDelay);

  // Priority: Cart overrides ROM from $C000-$FFFF when enabled
  if (addr >= CART_CODE && addr <= CART_END && cart.enabled) {
    data = cart.read(addr - CART_START);
    return data;
  }

  // Fast lookup based on high byte for other regions
  switch (memoryMap[addr >> 8]) {
    case REGION_ROM:
      if (rom.enabled) {
        data = rom.read(addr - ROM_START);
      }
      break;
    case REGION_RAM:
      if (ram.enabled) {
        data = ram.read(addr - RAM_START);
      }
      break;
    case REGION_IO: {
      uint8_t ioSlot = floor((addr - IO_START) / IO_SLOT_SIZE);
      switch (ioSlot) {
        case 0: // IO 1 - RAM Card
          data = ramCard1.read(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)));
          break;
        case 1: // IO 2 - RAM Card
          data = ramCard2.read(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)));
          break;
        case 2: // IO 3 - RTC Card
          data = rtcCard.read(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)));
          break;
        case 3: // IO 4 - Storage Card
          data = storageCard.read(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)));
          break;
        case 4: // IO 5 - Serial Card
          data = serialCard.read(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)));
          break;
        case 5: // IO 6 - GPIO Card
          data = gpioCard.read(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)));
          break;
        case 6: // IO 7 - Sound Card
        case 7: // IO 8 - Video Card
        default:
          data = readData();
          break;
      }
      break;
    }
    default:
      break;
  }

  return data;
}

FASTRUN void write(uint16_t addr, uint8_t val) {
  address = addr;
  data = val;
  readWrite = LOW;

  if (cachedFreqIndex != freqIndex) {
    cachedFreqIndex = freqIndex;
    cachedDelay = FREQ_PERIODS[freqIndex] <= 1 ? 
                  (FREQ_PERIODS[freqIndex] * 1000) / 2 : 
                  FREQ_PERIODS[freqIndex] / 2;
  }

  // WDC 65C02 write cycle timing:
  // - Address and R/W̅ must be stable before PHI2 rises
  // - Data must be valid during PHI2 high
  digitalWriteFast(PHI2, LOW);
  delayNanoseconds(20); // tADS: Address setup time before PHI2 rise
  writeAddress(address);
  digitalWriteFast(RWB, readWrite);
  setDataDirOut(); // Set data direction to output before driving bus
  writeData(data);     // Data ready before PHI2 goes high
  FREQ_PERIODS[freqIndex] <= 1 ? delayNanoseconds(cachedDelay) : delayMicroseconds(cachedDelay);
  digitalWriteFast(PHI2, HIGH);
  FREQ_PERIODS[freqIndex] <= 1 ? delayNanoseconds(cachedDelay) : delayMicroseconds(cachedDelay);

  // Priority: Cart overrides ROM from $C000-$FFFF when enabled
  if (addr >= CART_CODE && addr <= CART_END && cart.enabled) {
    cart.write(addr - CART_START, data);
    return;
  }

  // Fast lookup based on high byte for other regions
  switch (memoryMap[addr >> 8]) {
    case REGION_ROM:
      if (rom.enabled) {
        rom.write(addr - ROM_START, data);
      }
      break;
    case REGION_RAM:
      if (ram.enabled) {
        ram.write(addr - RAM_START, data);
      }
      break;
    case REGION_IO: {
      uint8_t ioSlot = floor((addr - IO_START) / IO_SLOT_SIZE);
      switch (ioSlot) {
        case 0: // IO 1 - RAM Card
          ramCard1.write(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)), data);
          break;
        case 1: // IO 2 - RAM Card
          ramCard2.write(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)), data);
          break;
        case 2: // IO 3 - RTC Card
          rtcCard.write(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)), data);
          break;
        case 3: // IO 4 - Storage Card
          storageCard.write(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)), data);
          break;
        case 4: // IO 5 - Serial Card
          serialCard.write(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)), data);
          break;
        case 5: // IO 6 - GPIO Card
          gpioCard.write(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)), data);
          break;
        case 6: // IO 7 - Sound Card
        case 7: // IO 8 - Video Card
        default:
          break;
      }
      break;
    }
    default:
      break;
  }
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
  digitalWriteFast(PHI2, HIGH);
  
  pinMode(IRQB, INPUT_PULLUP);
  pinMode(NMIB, INPUT_PULLUP);
  pinMode(RDY, INPUT_PULLUP);
  pinMode(BE, INPUT_PULLUP); // Unused but set to input with pullup to prevent floating

  pinMode(CLK_SWB, INPUT_PULLUP);
  pinMode(STEP_SWB, INPUT_PULLUP);
  pinMode(RS_SWB, INPUT_PULLUP);

  #ifdef DEVBOARD_0
  pinMode(OE1, OUTPUT);
  pinMode(OE2, OUTPUT);
  pinMode(OE3, OUTPUT);

  pinMode(GPIO0, INPUT);
  pinMode(GPIO1, INPUT);
  pinMode(GPIO2, INPUT);
  pinMode(GPIO3, INPUT);

  digitalWriteFast(OE1, HIGH);
  digitalWriteFast(OE2, HIGH);
  digitalWriteFast(OE3, HIGH);
  #endif

  #ifdef DEVBOARD_1
  pinMode(RESET_SWB, INPUT_PULLUP);

  // These pins are currently unused on the dev board but 
  // should be set to inputs to avoid floating and potential interference
  pinMode(MOSI1, INPUT);
  pinMode(MISO1, INPUT);
  pinMode(SCK1, INPUT);
  pinMode(CS, INPUT);
  pinMode(RX, INPUT);
  pinMode(TX, INPUT);
  #endif
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

  #ifdef DEVBOARD_1
  resetButton.attach(RESET_SWB, INPUT_PULLUP);
  resetButton.interval(DEBOUNCE);
  resetButton.setPressedState(LOW);
  #endif
}

void initSD() {
  SD.begin(BUILTIN_SDCARD);

  if (SD.mediaPresent()) {
    if (SD.exists("ROMs/BIOS.bin")) {
      loadROMPath("ROMs/BIOS.bin");
      rom.file = "BIOS.bin";
      autoStart = true;
    }
  }
}

void initUSB() {
  usb.begin();
  keyboard.attachPress(onKeyboard);
  keyboard.attachRelease(onKeyboardRelease);
}

void initEthernet() {
  Ethernet.begin();
  Ethernet.waitForLocalIP(5000);

  MDNS.begin("6502");
  MDNS.addService("_http", "_tcp", 80);
}

void initServer() {
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  
  server.on("/", HTTP_GET, onServerRoot);
  server.on("/info", HTTP_GET, onServerInfo);
  server.on("/memory", HTTP_GET, onServerMemory);
  server.on("/storage", HTTP_GET, onServerStorage);
  server.on("/load", HTTP_GET, onServerLoad);
  server.on("/control", HTTP_GET, onServerControl);
  server.onNotFound(onServerNotFound);
  server.begin();
}

void initFiles() {
  for (size_t i = 0; i < FILE_MAX; i++) {
    Carts[i] = "?";
    Programs[i] = "?";
    ROMs[i] = "?";
  }
}

void buildMemoryMap() {
  for (int i = 0; i < 256; i++) {
    uint16_t testAddr = i << 8;
    // Note: Cart region overlaps ROM and is checked dynamically in read/write
    // so we map the ROM region here, and cart takes priority at runtime
    if (testAddr >= ROM_CODE && testAddr <= ROM_END) {
      memoryMap[i] = REGION_ROM;
    } else if (testAddr >= RAM_START && testAddr <= RAM_END) {
      memoryMap[i] = REGION_RAM;
    } else if (testAddr >= IO_START && testAddr <= IO_END) {
      memoryMap[i] = REGION_IO;
    } else {
      memoryMap[i] = REGION_NONE;
    }
  }
}

//
// TIME
//

time_t syncTime() {
  return Teensy3Clock.get();
}

String formattedDateTime() {
  // Use static buffer and snprintf for better performance
  static char buffer[32];
  
  snprintf(buffer, sizeof(buffer), "%d/%d/%d %d:%02d:%02d",
           month(), day(), year(),
           hour(), minute(), second());
  
  return String(buffer);
}

//
// WEB SERVER
//

FASTRUN void onServerRoot(AsyncWebServerRequest *request) {
  request->send(
    "text/html",
    sizeof(HTML) / sizeof(char), 
    [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t 
  {
    size_t length = min(maxLen, (sizeof(HTML) / sizeof(char)) - index);

    memcpy(buffer, HTML + index, length);

    return length;
  });
}

FASTRUN void onServerInfo(AsyncWebServerRequest *request) {
  String response;
  JsonDocument doc;

  doc["address"]            = address;
  doc["cartEnabled"]        = cart.enabled;
  doc["cartFile"]           = cart.file;
  doc["cpuAccumulator"]     = cpu.accumulator();
  doc["cpuOpcodeCycle"]     = cpu.opcodeCycle();
  doc["cpuPC"]              = cpu.programCounter();
  doc["cpuStackPointer"]    = cpu.stackPointer();
  doc["cpuStatus"]          = cpu.status();
  doc["cpuX"]               = cpu.x();
  doc["cpuY"]               = cpu.y();
  doc["data"]               = data;
  doc["freqLabel"]          = FREQ_LABELS[freqIndex];
  doc["freqPeriod"]         = FREQ_PERIODS[freqIndex];
  doc["ipAddress"]          = Ethernet.localIP();
  doc["isRunning"]          = isRunning;
  doc["lastSnapshot"]       = lastSnapshot;
  doc["programFile"]        = ram.file;
  doc["ramEnabled"]         = ram.enabled;
  doc["romEnabled"]         = rom.enabled;
  doc["romFile"]            = rom.file;
  doc["rtc"]                = now();
  doc["rw"]                 = readWrite ? 1 : 0;
  doc["version"]            = VERSION;
  
  serializeJson(doc, response);

  request->send(200, "application/json", response);
}

/* Notes: We are paginating RAM/ROM responses due to limitations in the AsyncWebServer_Teensy41 lib.              */
/* There is a bug in the implementation of beginChunkedResponse() (improper formatting) and all other response    */
/* types besides beginResponseStream() will corrupt or add garbage to the data. So we are limited to 1024 byte    */
/* blocks.                                                                                                        */
/* All 32 blocks of ROM can be inspected but only top 24k is valid ROM space.                                     */

FASTRUN void onServerMemory(AsyncWebServerRequest *request) {
  String target;
  size_t page;
  bool formatted = false;

  if (request->hasParam("target")) {
    target = request->getParam("target")->value();

    if (target != "ram" && target != "rom") {
      request->send(400);
      return;
    }
  } else {
    request->send(400);
    return;
  }
  if (request->hasParam("page")) {
    page = size_t(request->getParam("page")->value().toInt());

    if (target == "ram" && page >= ((RAM_END - RAM_START + 1) / PAGE_SIZE)) {
      request->send(400);
      return;
    }
    if (target == "rom" && page >= ((ROM_END - ROM_START + 1) / PAGE_SIZE)) {
      request->send(400);
      return;
    }
  } else {
    request->send(400);
    return;
  }
  if (request->hasParam("formatted")) {
    formatted = true;
  }

  AsyncResponseStream *response = request->beginResponseStream(
    formatted ? "text/plain" : "application/octet-stream; charset=binary"
  );

  for (size_t i = 0; i < PAGE_SIZE; i++) {
    if (target == "ram") {
      formatted ? response->printf("%02X ", ram.data[i + (page * PAGE_SIZE)]) : response->write(ram.data[i + (page * PAGE_SIZE)]);
    } else if (target == "rom") {
      formatted ? response->printf("%02X ", rom.data[i + (page * PAGE_SIZE)]) : response->write(rom.data[i + (page * PAGE_SIZE)]);
    }
  }
  
  request->send(response);
}

FASTRUN void onServerStorage(AsyncWebServerRequest *request) {
  String target;
  size_t page;

  String response;
  JsonDocument doc;

  if (request->hasParam("target")) {
    target = request->getParam("target")->value();

    if (target != "cart" && target != "program" && target != "rom") {
      request->send(400);
      return;
    }
  } else {
    request->send(400);
    return;
  }
  if (request->hasParam("page")) {
    page = size_t(request->getParam("page")->value().toInt());

    if (page >= (FILE_MAX / FILE_PER_PAGE)) {
      request->send(400);
      return;
    }
  } else {
    request->send(400);
    return;
  }

  if (target == "cart") {
    readCarts();

    for (uint i = (page * FILE_PER_PAGE); i < ((page * FILE_PER_PAGE) + FILE_PER_PAGE); i++) {
      doc[i - (page * FILE_PER_PAGE)] = Carts[i];
    }
  } else if (target == "program") {
    readPrograms();

    for (uint i = (page * FILE_PER_PAGE); i < ((page * FILE_PER_PAGE) + FILE_PER_PAGE); i++) {
      doc[i - (page * FILE_PER_PAGE)] = Programs[i];
    }
  } else if (target == "rom") {
    readROMs();

    for (uint i = (page * FILE_PER_PAGE); i < ((page * FILE_PER_PAGE) + FILE_PER_PAGE); i++) {
      doc[i - (page * FILE_PER_PAGE)] = ROMs[i];
    }
  }

  serializeJson(doc, response);

  request->send(200, "application/json", response);
}

FASTRUN void onServerLoad(AsyncWebServerRequest *request) {
  String target;
  String filename;

  if (request->hasParam("target")) {
    target = request->getParam("target")->value();

    if (target != "cart" && target != "program" && target != "rom") {
      request->send(400);
      return;
    }
  } else {
    request->send(400);
    return;
  }
  if (request->hasParam("filename")) {
    filename = request->getParam("filename")->value();
  } else {
    request->send(400);
    return;
  }

  if (target == "cart") {
    loadCartFile(filename);
  } else if (target == "program") {
    loadProgramFile(filename);
  } else if (target == "rom") {
    loadROMFile(filename);
  }

  request->send(200);
}

FASTRUN void onServerControl(AsyncWebServerRequest *request) {
  String command;

  if (request->hasParam("command")) {
    command = request->getParam("command")->value();
  } else {
    request->send(400);
    return;
  }

  if (command == "a" || command == "A") {         // Toggle RAM
    toggleRAM();
  } else if (command == "k" || command == "K") {  // Tick
    tick();
  } else if (command == "l" || command == "L") {  // Toggle Cart
    toggleCart();
  } else if (command == "o" || command == "O") {  // Toggle ROM
    toggleROM();
  } else if (command == "p" || command == "P") {  // Snapshot
    snapshot();
  } else if (command == "r" || command == "R") {  // Run / Stop
    toggleRunStop();
  } else if (command == "s" || command == "S") {  // Step
    step();
  } else if (command == "t" || command == "T") {  // Reset
    reset();
  } else if (command == "-") {                    // Decrease Frequency
    decreaseFrequency();
  } else if (command == "+") {                    // Increase Frequency
    increaseFrequency();
  } else {                                        // Bad Request
    request->send(400);
    return;
  }

  request->send(200);
}

FASTRUN void onServerNotFound(AsyncWebServerRequest *request) {
  request->send(404);
}