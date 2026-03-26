#include <Arduino.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <Bounce2.h>
#include <SD.h>
#include <EEPROM.h>
#include <map>
#include <USBHost_t36.h>
#include <QNEthernet.h>
#include <AsyncWebServer_Teensy41.h>
#include <6502.h>

using namespace qindesign::network;

Button clkButton      = Button();
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
void onKeyDown(uint8_t keycode);
void onKeyUp(uint8_t keycode);
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
void unloadCart();
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
void initWWWCache();

void buildMemoryMap();

void startCpuTimer();
void stopCpuTimer();
void cpuTimerISR();

time_t syncTime();
String formattedDateTime();
time_t lastSnapshot;

void onServerRoot(AsyncWebServerRequest *request);
void onServerInfo(AsyncWebServerRequest *request);
void onServerMemory(AsyncWebServerRequest *request);
void onServerStorage(AsyncWebServerRequest *request);
void onServerLoad(AsyncWebServerRequest *request);
void onServerControl(AsyncWebServerRequest *request);
void onServerKeyboard(AsyncWebServerRequest *request);
void onServerNotFound(AsyncWebServerRequest *request);

volatile uint8_t freqIndex = FREQ_SIZE - 1;
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

#ifdef DEVBOARD_0
#define HWSERIAL Serial4
#endif
#ifdef DEVBOARD_1
#define HWSERIAL Serial7
#endif

static uint32_t cachedCpuFrequency = 1000000;
static uint8_t cachedCpuFreqIndex = 0xFF;

IntervalTimer cpuTimer;
volatile bool timerMode = false;
volatile uint8_t busPhase = 0;

// Use IntervalTimer for slow frequencies where blocking delays would freeze loop().
// At period >= TIMER_FREQ_THRESHOLD (µs), the ISR completes well within each half-cycle.
// Above that frequency, tight-loop with inline delays is used (loop() stays responsive).
#define TIMER_FREQ_THRESHOLD 32

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
SoundCard soundCard = SoundCard();
VideoCard videoCard = VideoCard();

// GPIO Attachments
GPIOKeyboardMatrixAttachment keyboardMatrixAttachment = GPIOKeyboardMatrixAttachment(10);     // Priority 10
GPIOKeyboardEncoderAttachment keyboardEncoderAttachment = GPIOKeyboardEncoderAttachment(20);  // Priority 20 (supports both Port A and B)
GPIOJoystickAttachment joystickAttachment = GPIOJoystickAttachment(false, 100);               // Port B, Priority 100

//
// MAIN
//

void setup() {
  Serial.begin(115200);       // Ignored by Teensy; Baud rate is USB data rate 480Mbps
  SerialUSB1.begin(115200);   // Ignored by Teensy; Baud rate is USB data rate 480Mbps
  SerialUSB2.begin(115200);   // Ignored by Teensy; Baud rate is USB data rate 480Mbps
  HWSERIAL.begin(115200);

  setSyncProvider(syncTime);
  buildMemoryMap();
  
  // Attach peripherals to GPIO Card
  // Keyboard matrix (manual scanning) - highest priority for Port A rows
  gpioCard.attachToPortA(&keyboardMatrixAttachment);
  gpioCard.attachToPortB(&keyboardMatrixAttachment);
  
  // Keyboard encoder (ASCII on both Port A and Port B) - medium priority
  gpioCard.attachToPortA(&keyboardEncoderAttachment);
  gpioCard.attachToPortB(&keyboardEncoderAttachment);
  
  // Joystick (Port B) - lowest priority, fallback
  gpioCard.attachToPortB(&joystickAttachment);
  
  initPins();
  initButtons();
  initSD();
  initWWWCache();
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

  clkButton.update();
  stepButton.update();
  runStopButton.update();
  #ifdef DEVBOARD_1
  resetButton.update();
  #endif

  if (clkButton.pressed()) {
    decreaseFrequency();
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

  if (isRunning && !timerMode) {
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
      if (!SD.mediaPresent()) { Serial.println("SD Card Not Detected!"); break; }
      readPrograms();
      listPrograms();
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

    case 'm':
    case 'M':
      if (!SD.mediaPresent()) { Serial.println("SD Card Not Detected!"); break; }
      readROMs();
      listROMs();
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
      unloadCart();
      Serial.print("Cart: ");
      Serial.println(cart.file);
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

void onKeyDown(uint8_t keycode) {
  // Fix some quirks in the USBHost_t36 library
  // Remapped following this: https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2
  switch (keycode) {
    case 0x67: // LEFT CTRL
      keycode = 0xE0;
      break;
    case 0x68: // LEFT SHIFT
      keycode = 0xE1;
      break;
    case 0x69: // LEFT ALT
      keycode = 0xE2;
      break;
    case 0x6A: // LEFT META
      keycode = 0xE3;
      break;
    case 0x6B: // RIGHT CTRL
      keycode = 0xE4;
      break;
    case 0x6C: // RIGHT SHIFT
      keycode = 0xE5;
      break;
    case 0x6D: // RIGHT ALT
      keycode = 0xE6;
      break;
    case 0x6E: // RIGHT META
      keycode = 0xE7;
      break;
    default:
      break;
  }

  // Route keyboard input to attachments
  keyboardMatrixAttachment.updateKey(keycode, true);      // Update keyboard matrix
  keyboardEncoderAttachment.updateKeyPortB(keycode, true); // USB/browser keyboard → Port B (matrix encoder)
  
  #ifdef KEYBOARD_DEBUG
  Serial.print("Key pressed: 0x");
  Serial.println(keycode, HEX);
  Serial.println(keycode);
  #endif
}

void onKeyUp(uint8_t keycode) {
  // Fix some quirks in the USBHost_t36 library
  // Remapped following this: https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2
  switch (keycode) {
    case 0x67: // LEFT CTRL
      keycode = 0xE0;
      break;
    case 0x68: // LEFT SHIFT
      keycode = 0xE1;
      break;
    case 0x69: // LEFT ALT
      keycode = 0xE2;
      break;
    case 0x6A: // LEFT META
      keycode = 0xE3;
      break;
    case 0x6B: // RIGHT CTRL
      keycode = 0xE4;
      break;
    case 0x6C: // RIGHT SHIFT
      keycode = 0xE5;
      break;
    case 0x6D: // RIGHT ALT
      keycode = 0xE6;
      break;
    case 0x6E: // RIGHT META
      keycode = 0xE7;
      break;
    default:
      break;
  }

  // Release key from keyboard matrix
  keyboardMatrixAttachment.updateKey(keycode, false);       // Update keyboard matrix
  keyboardEncoderAttachment.updateKeyPortB(keycode, false); // USB/browser keyboard → Port B (matrix encoder)
  
  #ifdef KEYBOARD_DEBUG
  Serial.print("Key Released: 0x");
  Serial.println(keycode, HEX);
  Serial.println(keycode);
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
  Serial.println(ram.file);
  Serial.print("ROM: ");
  Serial.println(rom.file);
  Serial.print("Cart: ");
  Serial.println(cart.file);
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
  Serial.println("| (+/-) Clk Freq     | (D)ump / Sna(P)shot | In(F)o / Lo(G)  |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("| Unload Cart (U)    | List RO(M)s / (C)arts / Progr(A)ms    |");
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
  soundCard.reset();
  videoCard.reset();

  if (isCurrentlyRunning) {
    toggleRunStop();
  }
}

void tick() {
  if (freqIndex != cachedCpuFreqIndex) {
    cachedCpuFrequency = (uint32_t)(1000000.0 / FREQ_PERIODS[freqIndex]);
    cachedCpuFreqIndex = freqIndex;
  }

  uint8_t interrupt = 0x00;

  cpu.tick();

  // Tick IO cards and accumulate interrupts
  interrupt |= rtcCard.tick(cachedCpuFrequency);
  interrupt |= storageCard.tick(cachedCpuFrequency);
  interrupt |= serialCard.tick(cachedCpuFrequency);
  interrupt |= gpioCard.tick(cachedCpuFrequency);
  interrupt |= soundCard.tick(cachedCpuFrequency);
  interrupt |= videoCard.tick(cachedCpuFrequency);
  
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

void startCpuTimer() {
  double period = FREQ_PERIODS[freqIndex];
  if (period >= TIMER_FREQ_THRESHOLD) {
    busPhase = 0;
    timerMode = true;
    cpuTimer.begin(cpuTimerISR, period / 2.0);
  } else {
    timerMode = false;
  }
}

void stopCpuTimer() {
  if (timerMode) {
    cpuTimer.end();
    timerMode = false;
  }
}

FASTRUN void cpuTimerISR() {
  if (busPhase == 0) {
    if (freqIndex != cachedCpuFreqIndex) {
      cachedCpuFrequency = (uint32_t)(1000000.0 / FREQ_PERIODS[freqIndex]);
      cachedCpuFreqIndex = freqIndex;
    }

    cpu.tick();

    // Tick IO cards and accumulate interrupts
    uint8_t interrupt = 0x00;

    interrupt |= rtcCard.tick(cachedCpuFrequency);
    interrupt |= storageCard.tick(cachedCpuFrequency);
    interrupt |= serialCard.tick(cachedCpuFrequency);
    interrupt |= gpioCard.tick(cachedCpuFrequency);
    interrupt |= soundCard.tick(cachedCpuFrequency);
    interrupt |= videoCard.tick(cachedCpuFrequency);

    if (interrupt & 0x40) {
      cpu.nmiTrigger();
    }
    if (interrupt & 0x80) {
      cpu.irqTrigger();
    } else {
      cpu.irqClear();
    }

    busPhase = 1;
  } else {
    busPhase = 0;
  }
}

void step() {
  // Execute one complete instruction and handle ticks/interrupts
  uint8_t ticks = cpu.step();

  // Process interrupts for each tick of the instruction
  for(uint i = 0; i < ticks; i++) {
    uint8_t interrupt = 0x00;

    // Calculate CPU frequency from the period (period is in microseconds)
    uint32_t cpuFrequency = (uint32_t)(1000000.0 / FREQ_PERIODS[freqIndex]);

    // Tick IO and check for interrupt
    interrupt |= rtcCard.tick(cpuFrequency);
    interrupt |= storageCard.tick(cpuFrequency);
    interrupt |= serialCard.tick(cpuFrequency);
    interrupt |= gpioCard.tick(cpuFrequency);
    interrupt |= soundCard.tick(cpuFrequency);
    interrupt |= videoCard.tick(cpuFrequency);

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
  if (isRunning) stopCpuTimer();

  if (freqIndex > 0) {
    freqIndex--;
  } else {
    freqIndex = FREQ_SIZE - 1;
  }

  if (isRunning) startCpuTimer();
}

void increaseFrequency() {
  if (isRunning) stopCpuTimer();

  if (freqIndex < (FREQ_SIZE - 1)) {
    freqIndex++;
  } else {
    freqIndex = 0;
  }

  if (isRunning) startCpuTimer();
}

void toggleRunStop() {
  if (isRunning) {
    stopCpuTimer();
    isRunning = false;
  } else {
    isRunning = true;
    startCpuTimer();
  }
}

void unloadCart() {
  cart.file = "None";
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
// READ / WRITE
//

FASTRUN uint8_t read(uint16_t addr, bool isDbg) {
  address = addr;
  data = 0x00;
  readWrite = HIGH;

  // Priority: Cart overrides ROM from $C000-$FFFF when loaded
  if (addr >= CART_CODE && addr <= CART_END && cart.file != "None") {
    data = cart.read(addr - CART_START);
    return data;
  }

  // Fast lookup based on high byte for other regions
  switch (memoryMap[addr >> 8]) {
    case REGION_ROM:
      data = rom.read(addr - ROM_START);
      break;
    case REGION_RAM:
      data = ram.read(addr - RAM_START);
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
          data = soundCard.read(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)));
          break;
        case 7: // IO 8 - Video Card
          data = videoCard.read(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)));
          break;
        default:
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

  // Priority: Cart overrides ROM from $C000-$FFFF when loaded
  if (addr >= CART_CODE && addr <= CART_END && cart.file != "None") {
    cart.write(addr - CART_START, data);
    return;
  }

  // Fast lookup based on high byte for other regions
  switch (memoryMap[addr >> 8]) {
    case REGION_ROM:
      rom.write(addr - ROM_START, data);
      break;
    case REGION_RAM:
      ram.write(addr - RAM_START, data);
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
          soundCard.write(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)), data);
          break;
        case 7: // IO 8 - Video Card
          videoCard.write(addr - (IO_START + (IO_SLOT_SIZE * ioSlot)), data);
          break;
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
  // 6502 bus pins — unused in emulation-only mode, set as inputs
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

  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D7, INPUT);

  pinMode(RESB, INPUT);
  pinMode(SYNC, INPUT);
  pinMode(RWB, INPUT);
  pinMode(PHI2, INPUT);
  pinMode(IRQB, INPUT);
  pinMode(NMIB, INPUT);
  pinMode(RDY, INPUT);
  pinMode(BE, INPUT);

  // Switch inputs
  pinMode(CLK_SWB, INPUT_PULLUP);
  pinMode(STEP_SWB, INPUT_PULLUP);
  pinMode(RS_SWB, INPUT_PULLUP);

  #ifdef DEVBOARD_0
  pinMode(OE1, INPUT);
  pinMode(OE2, INPUT);
  pinMode(OE3, INPUT);
  pinMode(GPIO0, INPUT);
  pinMode(GPIO1, INPUT);
  #endif

  #ifdef DEVBOARD_1
  pinMode(RESET_SWB, INPUT_PULLUP);
  pinMode(MOSI1, INPUT);
  pinMode(MISO1, INPUT);
  pinMode(SCK1, INPUT);
  pinMode(CS, INPUT);
  #endif
}

void initButtons() {
  clkButton.attach(CLK_SWB, INPUT_PULLUP);
  clkButton.interval(DEBOUNCE);
  clkButton.setPressedState(LOW);

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
  keyboard.attachRawPress(onKeyDown);
  keyboard.attachRawRelease(onKeyUp);
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
  server.on("/keyboard", HTTP_GET, onServerKeyboard);
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

String getContentType(String path) {
  if (path.endsWith(".html") || path.endsWith(".htm")) return "text/html; charset=utf-8";
  if (path.endsWith(".css"))  return "text/css; charset=utf-8";
  if (path.endsWith(".js"))   return "application/javascript; charset=utf-8";
  if (path.endsWith(".json")) return "application/json";
  if (path.endsWith(".png"))  return "image/png";
  if (path.endsWith(".jpg") || path.endsWith(".jpeg")) return "image/jpeg";
  if (path.endsWith(".gif"))  return "image/gif";
  if (path.endsWith(".svg"))  return "image/svg+xml";
  if (path.endsWith(".ico"))  return "image/x-icon";
  if (path.endsWith(".woff")) return "font/woff";
  if (path.endsWith(".woff2")) return "font/woff2";
  return "application/octet-stream";
}

// ---------------------------------------------------------------------------
// WWW file cache — all files from WWW/ are loaded into RAM at startup so that
// HTTP serving never touches the SD card (SD is not safe for concurrent access
// from multiple async request handlers).
// ---------------------------------------------------------------------------

struct CachedFile {
  uint8_t* data;
  size_t   size;
  String   contentType;
};

static std::map<String, CachedFile> wwwCache;

static void cacheDir(const String& sdPath, const String& uriPrefix) {
  File dir = SD.open(sdPath.c_str());
  if (!dir || !dir.isDirectory()) return;

  File entry = dir.openNextFile();
  while (entry) {
    String name = String(entry.name());
    String fullPath = sdPath + "/" + name;
    String uri      = uriPrefix + "/" + name;

    if (entry.isDirectory()) {
      entry.close();
      cacheDir(fullPath, uri);
    } else {
      size_t sz = entry.size();
      uint8_t* buf = new(std::nothrow) uint8_t[sz];
      if (buf) {
        size_t got = 0;
        while (got < sz) {
          int n = entry.read(buf + got, sz - got);
          if (n <= 0) break;
          got += n;
        }
        if (got == sz) {
          wwwCache[uri] = { buf, sz, getContentType(fullPath) };
        } else {
          delete[] buf;
        }
      }
      entry.close();
    }
    entry = dir.openNextFile();
  }
  dir.close();
}

void initWWWCache() {
  if (!SD.mediaPresent()) return;
  cacheDir("WWW", "");
}

void sendSDFile(AsyncWebServerRequest *request, String path) {
  // Derive the URI key from the path: strip the leading "WWW" prefix.
  String uri = path.substring(3); // "WWW/assets/main.js" -> "/assets/main.js"

  auto it = wwwCache.find(uri);
  if (it == wwwCache.end()) {
    request->send(404);
    return;
  }

  const uint8_t* data      = it->second.data;
  const size_t   totalSize = it->second.size;
  const String&  ct        = it->second.contentType;

  // Serve directly from the in-memory cache using AsyncProgmemResponse.
  // The data pointer remains valid for the lifetime of the program.
  request->send(request->beginResponse(200, ct, data, totalSize));
}

FASTRUN void onServerRoot(AsyncWebServerRequest *request) {
  sendSDFile(request, "WWW/index.html"); // key: "/index.html"
}

FASTRUN void onServerInfo(AsyncWebServerRequest *request) {
  String response;
  JsonDocument doc;

  doc["address"]            = address;
  doc["cartEnabled"]        = cart.file != "None";
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
  doc["romFile"]            = rom.file;
  doc["rtc"]                = now();
  doc["rw"]                 = readWrite ? 1 : 0;
  doc["version"]            = VERSION;
  doc["soundEnabled"]       = true;
  doc["videoEnabled"]       = true;
  doc["avStreamConnected"]  = (bool)SerialUSB2.dtr();
  
  serializeJson(doc, response);

  request->send(200, "application/json", response);
}

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

  if (command == "k" || command == "K") {         // Tick
    tick();
  } else if (command == "p" || command == "P") {  // Snapshot
    snapshot();
  } else if (command == "r" || command == "R") {  // Run / Stop
    toggleRunStop();
  } else if (command == "s" || command == "S") {  // Step
    step();
  } else if (command == "t" || command == "T") {  // Reset
    reset();
  } else if (command == "u" || command == "U") {  // Unload Cart
    unloadCart();
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

FASTRUN void onServerKeyboard(AsyncWebServerRequest *request) {
  if (!request->hasParam("action") || !request->hasParam("keycode")) {
    request->send(400);
    return;
  }

  String action = request->getParam("action")->value();
  if (action != "down" && action != "up") {
    request->send(400);
    return;
  }

  uint8_t kc = (uint8_t)strtoul(request->getParam("keycode")->value().c_str(), nullptr, 16);

  if (action == "down") {
    onKeyDown(kc);
  } else {
    onKeyUp(kc);
  }

  request->send(200);
}

FASTRUN void onServerNotFound(AsyncWebServerRequest *request) {
  // Serve static files from the in-RAM WWW cache (loaded at startup from SD).
  String uri = request->url();
  String path = "WWW" + uri;

  if (wwwCache.count(uri)) {
    sendSDFile(request, path);
    return;
  }

  request->send(404);
}