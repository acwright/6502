#include <Arduino.h>
#include <TimeLib.h>
#include <Bounce2.h>
#include <SD.h>
#include <EEPROM.h>
#include <USBHost_t36.h>
#include <6502.h>

#ifdef DEVBOARD_1
#include <SPI.h>
#endif

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
MouseController     mouse(usb);
JoystickController  joystick(usb);

void onTick();
void onCommand(char command);
void onNumeric(uint8_t num);
void onKeyboard(int key);
void onMouse();
void onJoystick();

void info();
void log();
void reset();
void step();
void snapshot();
void decreaseFrequency();
void increaseFrequency();
void toggleRunStop();
void toggleRAM();
void toggleROM();
void toggleCart();
bool readROMs();
void listROMs();
void loadROM(uint index);
bool loadROMPath(String path);
bool readCarts();
void listCarts();
void loadCart(uint index);
bool loadCartPath(String path);
bool readPrograms();
void listPrograms();
void loadProgram(uint index);
bool loadProgramPath(String path);
void listIO();
void configureIO(uint index);
void prevPage();
void nextPage();

uint8_t read(uint16_t addr, bool isDbg);
void write(uint16_t addr, uint8_t val);

void initPins();
void initButtons();
void initSD();

void setDataDirIn();
void setDataDirOut();
uint8_t readData();
void writeData(uint8_t data);
void writeAddress(uint16_t address);

time_t syncTime();
String formattedDateTime();

uint8_t freqIndex = 20;      // 1 MHz
bool isRunning = false;
bool isStepping = false;
bool autoStart = false;

uint8_t inputCtx = INPUT_CTX_ROM;
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
uint romPage = 0;
String Carts[CART_MAX];
uint cartPage = 0;
String Programs[PROG_MAX];
uint programPage = 0;
String programFile = "None";

CPU cpu = CPU(read, write);
RAM ram = RAM();
ROM rom = ROM();
Cart cart = Cart();
IO *io[8] = {
  new RAMCard(),
  new Emulator(),
  new Empty(),
  new Empty(),
  new SerialCard(),
  new Empty(),
  new Empty(),
  new Empty()
};

//
// MAIN
//

void setup() {
  Serial.begin(115200);

  setSyncProvider(syncTime);
  
  initPins();
  initButtons();
  initSD();

  #ifdef DEVBOARD_1
  SPI1.begin();
  #endif

  usb.begin();
  keyboard.attachPress(onKeyboard);

  info();
  reset();

  if (autoStart) {
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

  if (mouse.available()) {
    onMouse();
  }
  if (joystick.available()) {
    onJoystick();
  }
  if (Serial.available()) {
    onCommand(Serial.read());
  }

  if (isRunning) {
    cpu.tick();
    onTick();
  }
}

//
// EVENTS
//

void onTick() {
  uint8_t interrupt = 0x00;
  for(uint i = 0; i < 8; i++) {
    interrupt |= io[i]->tick();
  }

  if ((interrupt & 0x40) != 0x00) {
    cpu.nmiTrigger();
  }
  if ((interrupt & 0x80) != 0x00) {
    cpu.irqTrigger();
  } else {
    cpu.irqClear();
  }
}

void onCommand(char command) {
  switch (command) {
    case '0': 
      onNumeric(0);
      break;
    case '1':
      onNumeric(1);
      break;
    case '2':
      onNumeric(2);
      break;
    case '3':
      onNumeric(3);
      break;
    case '4':
      onNumeric(4);
      break;
    case '5':
      onNumeric(5);
      break;
    case '6':
      onNumeric(6);
      break;
    case '7':
      onNumeric(7);
      break;
    case 'a':
    case 'A':
      toggleRAM();
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
    case 'i':
    case 'I':
      listIO();
      break;
    case 'l':
    case 'L':
      toggleCart();
      break;
    case 'm':
    case 'M':
      listROMs();
      break;
    case 'o':
    case 'O':
      toggleROM();
      break;
    case 'p':
    case 'P':
      snapshot();
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
    case 'u':
    case 'U':
      listPrograms();
      break;
    case '-':
      decreaseFrequency();
      break;
    case '+':
      increaseFrequency();
      break;
    case '[':
      prevPage();
      break;
    case ']':
      nextPage();
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
    case INPUT_CTX_IO:
      configureIO(num);
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

  for (int i = 0; i < 8; i++) {
    uint8_t id = io[i]->id();
    if (id == IO_EMULATOR) {
      Emulator *emu = (Emulator *)io[i];
      emu->updateKeyboard((uint8_t)key);
    }
  }
  

  #ifdef KEYBOARD_DEBUG
  Serial.print("Keyboard: ");
  Serial.print(key);

  char output[64];

  sprintf(
    output, 
    " | %c%c%c%c%c%c%c%c | 0x%02X",
    BYTE_TO_BINARY(key /*& 0b01111111*/),
    key /*& 0b01111111*/
  );

  Serial.println(output);
  #endif
}

void onMouse() {
  int mouseX = mouse.getMouseX();
  int mouseY = mouse.getMouseY();
  int mouseW = mouse.getWheel();
  uint8_t mouseButtons = mouse.getButtons();

  for (int i = 0; i < 8; i++) {
    uint8_t id = io[i]->id();
    if (id == IO_EMULATOR) {
      Emulator *emu = (Emulator *)io[i];
      emu->updateMouse(mouseX, mouseY, mouseW, mouseButtons);
    }
  }

  mouse.mouseDataClear();

  #ifdef MOUSE_DEBUG
  if (mouseBtns > 0 || mouseX > 0 || mouseY > 0 || mouseW || 0) {
    Serial.print("Mouse: Buttons = ");
    Serial.print(mouseButtons);
    Serial.print(",  X = ");
    Serial.print(mouseX);
    Serial.print(",  Y = ");
    Serial.print(mouseY);
    Serial.print(",  Wheel = ");
    Serial.print(mouseW);
    Serial.println();
  }
  #endif
}

void onJoystick() {
  uint32_t buttons = joystick.getButtons();
  
  if (joystick.joystickType() == JoystickController::XBOX360 || 
      joystick.joystickType() == JoystickController::XBOXONE) 
  {
    for (int i = 0; i < 8; i++) {
      uint8_t id = io[i]->id();
      if (id == IO_EMULATOR) {
        Emulator *emu = (Emulator *)io[i];
        emu->updateJoystick(buttons);
      }
    } 
  }

  joystick.joystickDataClear();

  #ifdef JOYSTICK_DEBUG
  Serial.print("Joystick: Buttons = ");
  Serial.println(buttons, HEX);
  #endif
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
  Serial.print("6502 DB Emulator | Version: 1.0");
  Serial.println();
  Serial.println("---------------------------------");
  Serial.println("| Created by A.C. Wright © 2024 |");
  Serial.println("---------------------------------");
  Serial.println();
  Serial.print("RAM: ");
  Serial.print(programFile);
  Serial.print(" (");
  Serial.print(ram.enabled ? "Enabled" : "Disabled");
  Serial.println(")");
  Serial.print("ROM: ");
  Serial.print(rom.file);
  Serial.print(" (");
  Serial.print(rom.enabled ? "Enabled" : "Disabled");
  Serial.println(")");
  Serial.print("Cart: ");
  Serial.print(cart.file);
  Serial.print(" (");
  Serial.print(cart.enabled ? "Enabled" : "Disabled");
  Serial.println(")");
  Serial.print("Frequency: ");
  Serial.println(FREQS[freqIndex]);
  Serial.print("Date / Time: ");
  Serial.println(formattedDateTime());
  Serial.println();
  Serial.println("--------------------------------------------------------------");
  Serial.println("| (R)un / Stop          | (S)tep           | Rese(T)         |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("| Toggle R(A)M          | Toggle R(O)M     | Toggle Cart (L) |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("| List RO(M)s / (C)arts / (U)ser Programs  | Configure (I)O  |");
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

  cpu.reset();
  
  for(uint i = 0; i < 8; i++) {
    io[i]->reset();
  }

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

    uint8_t ticks = cpu.step();
    for(uint i = 0; i < ticks; i++) {
      onTick();
    }

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
    for(uint i = 0; i < (RAM_END - RAM_START); i++) {
      snapshot.write(ram.read(i));
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
}

void increaseFrequency() {
  if (freqIndex < 20) {
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

void toggleRAM() {
  ram.enabled = !ram.enabled;

  Serial.print("RAM: ");
  Serial.println(ram.enabled ? "Enabled" : "Disabled");
}

void toggleROM() {
  rom.enabled = !rom.enabled;

  Serial.print("ROM: ");
  Serial.println(rom.enabled ? "Enabled" : "Disabled");
}

void toggleCart() {
  cart.enabled = !cart.enabled;

  Serial.print("Cart: ");
  Serial.println(cart.enabled ? "Enabled" : "Disabled");
}

bool readROMs() {
  if (!SD.mediaPresent()) { 
    Serial.println("SD card not detected!");
    return false;
  }

  if (!SD.exists("ROMs")) {
    SD.mkdir("ROMs");
  }

  for (uint i = 0; i < ROM_MAX; i++) {
    ROMs[i] = "?";
  }

  File ROMDirectory = SD.open("ROMs");

  uint index = 0;

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

  inputCtx = INPUT_CTX_ROM;

  Serial.println();

  for (uint j = (romPage * 8); j < ((romPage * 8) + 8); j++) {
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

void loadROM(uint index) {
  String directory = "ROMs/";
  String filename = ROMs[(romPage * 8) + index];
  
  if (!filename.length()) {
    Serial.println("Invalid ROM! List RO(M)s before loading.");
    return;
  }

  String path = directory + filename;

  if (loadROMPath(path)) {
    rom.file = filename;
    rom.enabled = true;
    cart.enabled = false;
    
    Serial.print("Loaded ROM: ");
    Serial.println(rom.file);
  } else {
    Serial.println("Invalid ROM!");
  }
}

bool loadROMPath(String path) {
  if (!SD.mediaPresent()) { 
    Serial.println("SD card not detected!");
    return false;
  }
  
  File file = SD.open(path.c_str());

  if (file) {
    uint i = 0;

    while(file.available()) {
      rom.load(i, file.read());
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

  for (uint i = 0; i < CART_MAX; i++) {
    Carts[i] = "?";
  }

  File CartDirectory = SD.open("Carts");

  uint index = 0;

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

  inputCtx = INPUT_CTX_CART;

  Serial.println();

  for (uint j = (cartPage * 8); j < ((cartPage * 8) + 8); j++) {
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

void loadCart(uint index) {
  String directory = "Carts/";
  String filename = Carts[(cartPage * 8) + index];

  if (!filename.length()) { 
    Serial.println("Invalid Cart! List (C)arts before loading.");
    return;
  }

  String path = directory + filename;

  if (loadCartPath(path)) {
    cart.file = filename;
    cart.enabled = true;

    Serial.print("Loaded Cart: ");
    Serial.println(cart.file);
  } else {
    Serial.println("Invalid Cart!");
  }
}

bool loadCartPath(String path) {
  if (!SD.mediaPresent()) { 
    Serial.println("SD card not detected!");
    return false;
  }

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

    file.close();

    return true;
  } else {
    file.close();

    return false;
  }
}

bool readPrograms() {
  if (!SD.mediaPresent()) { 
    Serial.println("SD card not detected!");
    return false;
  }

  if (!SD.exists("Programs")) {
    SD.mkdir("Programs");
  }

  for (uint i = 0; i < PROG_MAX; i++) {
    Programs[i] = "?";
  }

  File ProgramDirectory = SD.open("Programs");

  uint index = 0;

  while(true) {
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

  return true;
}

void listPrograms() {
  if (!readPrograms()) { return; }

  inputCtx = INPUT_CTX_PROG;

  Serial.println();

  for (uint j = (programPage * 8); j < ((programPage * 8) + 8); j++) {
    Serial.print("(");
    Serial.print(j - (programPage * 8));
    Serial.print(")");
    Serial.print(" - ");
    Serial.println(Programs[j]);
  }

  Serial.println();
  Serial.print("Page: ");
  Serial.print(programPage);
  Serial.println(" | ([) Prev Page | Next Page (]) |");
  Serial.println();
}

void loadProgram(uint index) {
  String directory = "Programs/";
  String filename = Programs[(programPage * 8) + index];

  if (!filename.length()) { 
    Serial.println("Invalid Programs! List (U)ser Programs before loading.");
    return;
  }

  String path = directory + filename;

  if (loadProgramPath(path)) {
    programFile = filename;

    Serial.print("Loaded Program: ");
    Serial.println(programFile);
  } else {
    Serial.println("Invalid Program!");
  }
}

bool loadProgramPath(String path) {
  if (!SD.mediaPresent()) { 
    Serial.println("SD card not detected!");
    return false;
  }

  File file = SD.open(path.c_str());

  if (file) {
    uint i = 0;

    while(file.available()) {
      ram.load(i, file.read());
      i++;
    }

    file.close();

    return true;
  } else {
    file.close();

    return false;
  }
}

void listIO() {
  inputCtx = INPUT_CTX_IO;

  Serial.println();

  for (uint i = 0; i < 8; i++) {
    Serial.print("(");
    Serial.print(i);
    Serial.print(") IO ");
    Serial.print(i + 1);
    Serial.print(" $");
    Serial.print(IO_BANKS[i], HEX);
    Serial.print(" - ");
    Serial.println(io[i]->description());
  }

  Serial.println();
}

void configureIO(uint index) {
  uint8_t currentID = io[index]->id();
  uint8_t nextID;

  if (currentID < 4) {
    nextID = currentID + 1;
  } else {
    nextID = 0;
  }

  delete io[index];

  switch (nextID) {
    case IO_EMPTY:
      io[index] = new Empty();
      break;
    case IO_SERIAL_CARD:
      io[index] = new SerialCard();
      break;
    case IO_STORAGE_CARD:
      io[index] = new StorageCard();
      break;
    case IO_RAM_CARD:
      io[index] = new RAMCard();
      break;
    case IO_EMULATOR:
      io[index] = new Emulator();
      break;
    default:
      break;
  }

  Serial.print("(");
  Serial.print(index);
  Serial.print(") IO");
  Serial.print(index + 1);
  Serial.print(" $");
  Serial.print(IO_BANKS[index], HEX);
  Serial.print(" - ");
  Serial.println(io[index]->description());
}

void prevPage() {
  switch(inputCtx) {
    case INPUT_CTX_ROM:
      if (romPage > 0)  {
        romPage--;
      } else {
        romPage = 31;
      }
      listROMs();
      break;
    case INPUT_CTX_CART:
      if (cartPage > 0)  {
        cartPage--;
      } else {
        cartPage = 31;
      }
      listCarts();
      break;
    case INPUT_CTX_PROG:
      if (programPage > 0)  {
        programPage--;
      } else {
        programPage = 31;
      }
      listPrograms();
      break;
    default:
      break;
  }
}

void nextPage() {
  switch(inputCtx) {
    case INPUT_CTX_ROM:
      if (romPage < 31) {
        romPage++;
      } else {
        romPage = 0;
      }
      listROMs();
      break;
    case INPUT_CTX_CART:
      if (cartPage < 31) {
        cartPage++;
      } else {
        cartPage = 0;
      }
      listCarts();
      break;
    case INPUT_CTX_PROG:
      if (programPage < 31) {
        programPage++;
      } else {
        programPage = 0;
      }
      listPrograms();
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

  bool useNanoseconds = FREQ_PERIODS[freqIndex] <= 1;
  uint32_t delay = 0;

  if (useNanoseconds) {
    delay = (FREQ_PERIODS[freqIndex] * 1000) / 2;
  } else {
    delay = FREQ_PERIODS[freqIndex] / 2;
  }
  
  digitalWriteFast(PHI2, LOW);
  delayNanoseconds(20);
  writeAddress(address);
  digitalWriteFast(RWB, readWrite);
  setDataDirIn();
  useNanoseconds ? delayNanoseconds(delay) : delayMicroseconds(delay);
  digitalWriteFast(PHI2, HIGH);
  useNanoseconds ? delayNanoseconds(delay) : delayMicroseconds(delay);
  data = readData();

  if ((addr >= CART_CODE) && (addr <= CART_END) && cart.enabled) { // Cart
    data = cart.read(addr - CART_START);
  } else if ((addr >= ROM_CODE) && (addr <= ROM_END) && rom.enabled) { // ROM
    data = rom.read(addr - ROM_START);
  } else if ((addr >= RAM_START) && (addr <= RAM_END) && ram.enabled) { // RAM
    data = ram.read(addr - RAM_START);
  } else if ((addr >= IO_START) && (addr <= IO_END)) { // IO
    for (int i = 0; i < 8; i++) {
      if ((addr >= IO_BANKS[i]) && (addr <= (IO_BANKS[i] + IO_BANK_SIZE - 1)) && !io[i]->passthrough()) {
        data = io[i]->read(addr - IO_BANKS[i]);
      }
    }
  }

  return data;
}

FASTRUN void write(uint16_t addr, uint8_t val) {
  address = addr;
  data = val;
  readWrite = LOW;

  bool useNanoseconds = FREQ_PERIODS[freqIndex] <= 1;
  uint32_t delay = 0;

  if (useNanoseconds) {
    delay = (FREQ_PERIODS[freqIndex] * 1000) / 2;
  } else {
    delay = FREQ_PERIODS[freqIndex] / 2;
  }

  digitalWriteFast(PHI2, LOW);
  delayNanoseconds(20);
  writeAddress(address);
  digitalWriteFast(RWB, readWrite);
  useNanoseconds ? delayNanoseconds(delay) : delayMicroseconds(delay);
  digitalWriteFast(PHI2, HIGH);
  setDataDirOut();
  writeData(data);
  useNanoseconds ? delayNanoseconds(delay) : delayMicroseconds(delay);

  if ((addr >= CART_CODE) && (addr <= CART_END) && cart.enabled) { // Cart
    cart.write(addr - CART_START, data);
  } else if ((addr >= ROM_CODE) && (addr <= ROM_END) && rom.enabled) { // ROM
    rom.write(addr - ROM_START, data);
  } else if ((addr >= RAM_START) && (addr <= RAM_END) && ram.enabled) { // RAM
    ram.write(addr - RAM_START, data);
  } else if ((addr >= IO_START) && (addr <= IO_END)) { // IO
    for (int i = 0; i < 8; i++) {
      if ((addr >= IO_BANKS[i]) && (addr <= (IO_BANKS[i] + IO_BANK_SIZE - 1)) && !io[i]->passthrough()) {
        io[i]->write(addr - IO_BANKS[i], data);
      }
    }
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
  pinMode(BE, INPUT_PULLUP);

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

  pinMode(MOSI1, OUTPUT);
  pinMode(MISO1, INPUT_PULLUP);
  pinMode(SCK1, OUTPUT);
  pinMode(CS0, OUTPUT);
  pinMode(CS1, OUTPUT);
  pinMode(CS2, OUTPUT);

  digitalWriteFast(CS0, HIGH);
  digitalWriteFast(CS1, HIGH);
  digitalWriteFast(CS2, HIGH);
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
    if (SD.exists("ROM.bin")) {
      loadROMPath("ROM.bin");
      rom.file = "ROM.bin";
      autoStart = true;
    }
    if (SD.exists("Cart.bin")) {
      loadCartPath("Cart.bin");
      cart.file = "Cart.bin";
      cart.enabled = true;
      autoStart = true;
    }

    for (int i = 0; i < 8; i++) {
      uint8_t id = io[i]->id();
      if (id == IO_STORAGE_CARD) {
        StorageCard *sc = (StorageCard *)io[i];
        sc->begin();
      }
    }
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

//
// TIME
//

time_t syncTime() {
  return Teensy3Clock.get();
}

String formattedDateTime() {
  String time;

  time.append(month());
  time.append("/");
  time.append(day());
  time.append("/");
  time.append(year());
  time.append(" ");
  time.append(hour());
  time.append(":");
  time.append(minute() < 10 ? "0": "");
  time.append(minute());
  time.append(":");
  time.append(second() < 10 ? "0": "");
  time.append(second());

  return time;
}