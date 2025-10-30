#include <Arduino.h>
#include <TimeLib.h>
#include <Bounce2.h>
#include <SD.h>
#include <EEPROM.h>
#include <USBHost_t36.h>
#include <6502.h>

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

void onCommand(char command);
void onNumeric(uint8_t num);
void onKeyPress(uint8_t keycode);
void onKeyRelease(uint8_t keycode);
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
void loadROM(unsigned int index);
bool loadROMPath(String path);
bool readCarts();
void listCarts();
void loadCart(unsigned int index);
bool loadCartPath(String path);
void listIO();
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

unsigned int freqIndex = 20;      // 1 MHz
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
unsigned int romPage = 0;

String Carts[CART_MAX];
unsigned int cartPage = 0;

CPU cpu = CPU(read, write);
RTC rtc = RTC();
RAM ram = RAM(RAM_END - RAM_START + 1);
ROM rom = ROM(ROM_END - ROM_START + 1);
Cart cart = Cart(CART_END - CART_START + 1);
IO *io[8] = {
  new RAMCard(),
  new Empty(),
  new Empty(),
  new Empty(),
  new Empty(),
  new Empty(),
  new Empty(),
  new Empty()
};

//
// MAIN LOOPS
//

void setup() {
  initPins();
  initButtons();
  initSD();

  usb.begin();
  keyboard.attachRawPress(onKeyPress);
  keyboard.attachRawRelease(onKeyRelease);

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
  if (SerialUSB1.available()) {
    // TODO: Do something with SerialUSB1 receive
  }

  if (isRunning) {
    cpu.tick();

    for(int i = 0; i < 8; i++) {
      io[i]->tick();
    }
  }
}

//
// EVENTS
//

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
    case 'i':
    case 'I':
      listIO();
      break;
    case 'l':
    case 'L':
      toggleCart();
      break;
    case 'a':
    case 'A':
      toggleRAM();
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

void onNumeric(uint8_t num) {
  switch(inputCtx) {
    case INPUT_CTX_ROM:
      loadROM(num);
      break;
    case INPUT_CTX_CART:
      loadCart(num);
      break;
    case INPUT_CTX_IO:
      break;
  }
}

void onKeyPress(uint8_t keycode) {
  // If it's a modifier key for some effin' reason remap it to actual USB scancode
  if((keycode < 110) && (keycode > 102)) {
    switch (keycode)
		{
			case 103:
				keycode = 0xE0; // LEFT CTRL
			break;
			case 104:
				keycode = 0xE1; // LEFT SHIFT
			break;
			case 105:
				keycode = 0xE2; // LEFT ALT
			break;
			case 106:
				keycode = 0xE3; // LEFT META
			break;
			case 107:
				keycode = 0xE4; // RIGHT CTRL
			break;
			case 108:
				keycode = 0xE5; // RIGHT SHIFT
			break;
			case 109:
				keycode = 0xE6; // RIGHT ALT
			break;
			case 110:
				keycode = 0xE7; // RIGHT META
			break;
		}
  }

  #ifdef KEYBOARD_DEBUG
  Serial.print("Key Pressed: ");
  Serial.print(keycode);

  char output[64];

  sprintf(
    output, 
    " | %c%c%c%c%c%c%c%c | 0x%02X",
    BYTE_TO_BINARY(keycode),
    keycode
  );

  Serial.println(output);
  #endif
}

void onKeyRelease(uint8_t keycode) {
  // If it's a modifier key for some effin' reason remap it to actual USB scancode
  if((keycode < 110) && (keycode > 102)) {
    switch (keycode)
		{
			case 103:
				keycode = 0xE0; // LEFT CTRL
			break;
			case 104:
				keycode = 0xE1; // LEFT SHIFT
			break;
			case 105:
				keycode = 0xE2; // LEFT ALT
			break;
			case 106:
				keycode = 0xE3; // LEFT META
			break;
			case 107:
				keycode = 0xE4; // RIGHT CTRL
			break;
			case 108:
				keycode = 0xE5; // RIGHT SHIFT
			break;
			case 109:
				keycode = 0xE6; // RIGHT ALT
			break;
			case 110:
				keycode = 0xE7; // RIGHT META
			break;
		}
  }

  #ifdef KEYBOARD_DEBUG
  Serial.print("Key Released: ");
  Serial.print(keycode);
  
  char output[64];

  sprintf(
    output, 
    " | %c%c%c%c%c%c%c%c | 0x%02X",
    BYTE_TO_BINARY(keycode),
    keycode
  );

  Serial.println(output);
  #endif
}

void onMouse() {
  int mouseX = mouse.getMouseX();
  int mouseY = mouse.getMouseY();
  int mouseW = mouse.getWheel();
  uint8_t mouseBtns = mouse.getButtons();

  // uint8_t mouseX    = _mouseX < 0 ? (abs(_mouseX) | 0b10000000) : _mouseX;  // If negative, set bit 7 (Left = -X)
  // uint8_t mouseY    = _mouseY < 0 ? (abs(_mouseY) | 0b10000000) : _mouseY;  // If negative, set bit 7 (Up = -Y)
  // uint8_t mouseW    = _mouseW < 0 ? (abs(_mouseW) | 0b10000000) : _mouseW;  // If negative, set bit 7 (Up = -W)

  mouse.mouseDataClear();

  #ifdef MOUSE_DEBUG
  if (mouseBtns > 0 || mouseX > 0 || mouseY > 0 || mouseW || 0) {
    Serial.print("Mouse: Buttons = ");
    Serial.print(mouseBtns);
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
    // XBOX Button Values
    // LFUNC (View): 0x8 1000
    // RFUNC (Menu): 0x4 0100
    // A: 0x10 0001 0000
    // B: 0x20 0010 0000
    // X: 0x40 0100 0000
    // Y: 0x80 1000 0000
    // UP: 0x100 0001 0000 0000
    // DOWN: 0x200 0010 0000 0000
    // LEFT: 0x400 0100 0000 0000
    // RIGHT: 0x800 1000 0000 0000
    // LBTN: 0x1000 0001 0000 0000 0000
    // RBTN: 0x2000 0010 0000 0000 0000
    // LABTN: 0x4000 0100 0000 0000 0000
    // RABTN: 0x8000 1000 0000 0000 0000

    // XBOX Analog Values
    // Left Analog X: 0:-32768(left) +32768(right) 
    // Left Analog Y: 1:-32768(down) +32768(up)
    // Right Analog X: 2:-32768(left) +32768(right) 
    // LTRIG: 3:0-1023
    // RTRIG: 4:0-1023
    // Right Analog Y: 5:-32768(down) +32768(up)

    // uint8_t joystickBtnL = buttons & 0xFF;
    // uint8_t joystickBtnH = (buttons >> 8) & 0xFF;
    // uint8_t joystickBtn  = (joystickBtnL & 0xF0) | (joystickBtnH & 0x0F);
    // uint8_t joystickLAnXL = joystick.getAxis(0) & 0xFF;
    // uint8_t joystickLAnXH = (joystick.getAxis(0) >> 8) & 0xFF;
    // uint8_t joystickLAnYL = joystick.getAxis(1) & 0xFF;
    // uint8_t joystickLAnYH = (joystick.getAxis(1) >> 8) & 0xFF;
    // uint8_t joystickRAnXL = joystick.getAxis(2) & 0xFF;
    // uint8_t joystickRAnXH = (joystick.getAxis(2) >> 8) & 0xFF;
    // uint8_t joystickRAnYL = joystick.getAxis(5) & 0xFF;
    // uint8_t joystickRAnYH = (joystick.getAxis(5) >> 8) & 0xFF;
    // uint8_t joystickLTrL = joystick.getAxis(3) & 0xFF;
    // uint8_t joystickLTrH = (joystick.getAxis(3) >> 8) & 0xFF;
    // uint8_t joystickRTrL = joystick.getAxis(4) & 0xFF;
    // uint8_t joystickRTrH = (joystick.getAxis(4) >> 8) & 0xFF;
  }

  joystick.joystickDataClear();

  #ifdef JOYSTICK_DEBUG
  Serial.printf("Joystick: Buttons = %x", buttons);

  uint64_t axis_mask  = joystick.axisMask();
  
  for (uint8_t i = 0; axis_mask != 0; i++, axis_mask >>= 1) {
    if (axis_mask & 1) {
      Serial.printf(" %d:%d", i, joystick.getAxis(i));
    }
  }
  Serial.println();
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
  Serial.println(ram.enabled ? "Enabled" : "Disabled");
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
  Serial.println(FREQS[freqIndex]);
  Serial.print("Date / Time: ");
  Serial.println(RTC::formattedDateTime());
  Serial.println();
  Serial.println("--------------------------------------------------------------");
  Serial.println("| (R)un / Stop          | (S)tep           | Rese(T)         |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("| Toggle R(A)M          | Toggle R(O)M     | Toggle Cart (L) |");
  Serial.println("--------------------------------------------------------------");
  Serial.println("| List RO(M)s / (C)arts |                  | Configure (I)O  |");
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
  
  for(int i = 0; i < 8; i++) {
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
    for(int i = 0; i < ticks; i++) {
      io[i]->tick();
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
    for(unsigned int i = 0; i < (RAM_END - RAM_START); i++) {
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

  inputCtx = INPUT_CTX_ROM;

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
    rom.file = filename;
    
    Serial.print("Loaded ROM: ");
    Serial.println(rom.file);
  } else {
    Serial.println("Invalid ROM!");
  }
}

bool loadROMPath(String path) {
  File file = SD.open(path.c_str());

  if (file) {
    unsigned int i = 0;

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

  inputCtx = INPUT_CTX_CART;

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
    cart.file = filename;
    cart.enabled = true;

    Serial.print("Loaded Cart: ");
    Serial.println(cart.file);
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
        rom.write(i, file.read());
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

void listIO() {
  inputCtx = INPUT_CTX_IO;

  Serial.println();

  // TODO: List IO
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
      if ((addr >= IO_BANKS[i]) && (addr <= (IO_BANKS[i] + IO_BANK_SIZE)) && !io[i]->passthrough()) {
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
      if ((addr >= IO_BANKS[i]) && (addr <= (IO_BANKS[i] + IO_BANK_SIZE)) && !io[i]->passthrough()) {
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

  pinMode(GPIO0, OUTPUT);
  pinMode(GPIO1, OUTPUT);
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO3, OUTPUT);

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