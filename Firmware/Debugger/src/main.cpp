#include <Arduino.h>
#include <ArduinoJson.h>
#include <TimerOne.h>
#include <Bounce2.h>
#include <SD.h>
#include <QNEthernet.h>
#include <TimeLib.h>
#include <EEPROM.h>
#include <USBHost_t36.h>

#if DEVBOARD
#include "devboard.h"
#elif RETROSHIELD
#include "retroshield.h"
#endif
#include "utilities.h"

using namespace qindesign::network;

#define RAM_START   0x0000
#define RAM_END     0x7FFF
#define IO_START    0x9C00
#define IO_END      0x9FFF
#define ROM_START   0x8000
#define ROM_CODE    0xA000
#define ROM_END     0xFFFF
                                  /*                            IO Registers                         */
                                  // | bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0 |  USB Keyboard input can be read by software using either a polling or interrupt strategy
#define KBD_DATA    0x9C00        // | DRDY  |                 ASCII KEY CODE                        |  Data Ready (Bit 7) -> [ Ascii Key Code (Bit 6-0) ] (Read from KBD_DATA to clear INT)
#define KBD_CTRL    0x9C01        // |                       NA                      | ARINT | KBINT |  (Bit 7-2 unused) -> Enable (1) / Disable (0) Arrow Key Interrupts (Bit 1) -> Enable (1) / Disable (0) Keyboard Interrupts (Bit 0)
#define KBD_STAT    0x9C02        // | KBINT | ARINT |       NA      |   UP  |  DOWN |  LEFT | RIGHT |  Interrupt Status (Bit 7-6) -> (Bit 5-4 unused) -> Arrow Keys Pressed (Bit 3-0)

#define MOUSE_X     0x9C03        // |  DIR  |                    VELOCITY                           |  Direction + Right (0) - Left (1) -> Velocity from 0-127
#define MOUSE_Y     0x9C04        // |  DIR  |                    VELOCITY                           |  Direction + Down (0) - Up (1) -> Velocity from 0-127
#define MOUSE_W     0x9C05        // |  DIR  |                    VELOCITY                           |  Direction + Down (0) - Up (1) -> Velocity from 0-127
#define MOUSE_BTN   0x9C06        // |                  NA                   |  MID  | RIGHT |  LEFT |  (Bit 7-3 unused) -> Middle, Right, Left Buttons Pressed (Bit 2-0)

#define TIMER_PERIOD              1 // 1 uS
#define FREQS                     (String[20])  {"1 Hz", "2 Hz", "4 Hz", "8 Hz", "16 Hz", "32 Hz", "64 Hz", "122 Hz", "244 Hz", "488 Hz", "976 Hz", "1.9 kHz", "3.9 kHz", "7.8 kHz", "15.6 kHz", "31.2 kHz", "62.5 kHz", "125 kHz", "250 kHz", "500 kHz"}
#define FREQ_DELAYS               (int[20])     {250000, 125000, 62500, 31250, 15625, 7812, 3906, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1, 0}

#define DEBOUNCE        5          // 5 milliseconds
#define IO_BUFFER_SIZE  255

#define DEBUG_NONE      0
#define DEBUG_6502      1
#define DEBUG_KEYBOARD  2
#define DEBUG_MOUSE     3
#define DEBUG_JOYSTICK  4

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
void onKeyPress(int key);
void onKeyRelease(int key);

void initPins();
void initButtons();
void initRAM();
void initIO();
void initROM();
void initSD();
void initEthernet();
void initUSB();

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
void snapshot();
void decreaseFrequency();
void increaseFrequency();
void toggleRunStop();
void toggleDebug();
void toggleRAM();
void toggleIO();
void toggleROM();
void listROMs();
void loadROM(unsigned int index);
void prevPage();
void nextPage();

String formatDateTime();
String formatDebugMode();

time_t syncTime();

unsigned int freqIndex = 19;      // 500 kHz
unsigned int freqCounter = 0;

byte debugMode = DEBUG_NONE;
bool isRunning = false;
bool RAMEnabled = true;
bool IOEnabled = true;
bool ROMEnabled = true;
String romFile = "None";

word address = 0;
byte data = 0;
byte readWrite = HIGH;

String ROMs[100];
unsigned int romPage = 0;

EthernetClient ethClient;

USBHost usb;
USBHub hub1(usb);
USBHIDParser hid1(usb);
USBHIDParser hid2(usb);
USBHIDParser hid3(usb);
KeyboardController keyboard(usb);
MouseController mouse(usb);
JoystickController joystick(usb);

//
// MAIN
//

void setup() {
  initPins();
  initButtons();
  initRAM();
  initIO();
  initROM();

  setSyncProvider(syncTime);

  Timer1.initialize(TIMER_PERIOD);
  Timer1.attachInterrupt(onTick);

  Serial.begin(9600);

  delay(1500);

  initSD();
  initEthernet();
  initUSB();

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

  usb.Task();

  if (mouse.available()) {
    byte buttons = mouse.getButtons();
    byte x = 0;
    byte y = 0;
    byte wheel = 0;

    int8_t mouseX = mouse.getMouseX();
    int8_t mouseY = mouse.getMouseY();
    int8_t mouseWheel = mouse.getWheel();

    x = mouseX < 0 ? (abs(mouseX) | 0b10000000) : mouseX; // If negative, set bit 7 (Left = -X)
    y = mouseY < 0 ? (abs(mouseY) | 0b10000000) : mouseY; // If negative, set bit 7 (Up = -Y)
    wheel = mouseWheel < 0 ? (abs(mouseWheel) | 0b10000000) : mouseWheel; // If negative, set bit 7 (Up = -W)

    IO[MOUSE_X - IO_START] = x;
    IO[MOUSE_Y - IO_START] = y;
    IO[MOUSE_W - IO_START] = wheel;
    IO[MOUSE_BTN - IO_START] = buttons;

    if (debugMode == DEBUG_MOUSE) {
      Serial.print("Mouse: Buttons = ");
      Serial.print(buttons);
      Serial.print(",  X = ");
      Serial.print(mouseX);
      Serial.print(",  Y = ");
      Serial.print(mouseY);
      Serial.print(",  Wheel = ");
      Serial.print(mouseWheel);
      Serial.println();
    }

    mouse.mouseDataClear();
  }

  if (joystick.available()) {
    uint64_t axis_mask = joystick.axisMask();
    uint32_t buttons = joystick.getButtons();

    if (debugMode == DEBUG_JOYSTICK) {
      Serial.printf("Joystick: Buttons = %x", buttons);

      for (uint8_t i = 0; axis_mask != 0; i++, axis_mask >>= 1) {
        if (axis_mask & 1) {
          Serial.printf(" %d:%d", i, joystick.getAxis(i));
        }
      }
      Serial.println();
    }

    joystick.joystickDataClear();
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

  if (debugMode == DEBUG_6502) {
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

      switch(address) {
        case KBD_DATA: {
          // Clear data ready bit
          IO[KBD_DATA-IO_START] &= 0b01111111;

          // Clear keyboard interrupt status bit
          IO[KBD_STAT-IO_START] &= 0b01111111;

          // Are there any more interrupts? If not, clear interrupt line (TODO: We should also check for other IO asserting interrupt...)
          if ((IO[KBD_STAT-IO_START] & 0b11000000) == 0) {
            digitalWriteFast(IRQB, HIGH);
          }

          break;
        }
        case KBD_STAT: {
          // Clear arrow key interrupt status bit
          IO[KBD_STAT-IO_START] &= 0b10111111;

          // Are there any more interrupts? If not, clear interrupt line (TODO: We should also check for other IO asserting interrupt...)
          if ((IO[KBD_STAT-IO_START] & 0b11000000) == 0) {
            digitalWriteFast(IRQB, HIGH);
          }

          break;
        }
      }
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
    case 'l':
    case 'L':
      listROMs();
      break;
    case 'b':
    case 'B':
      toggleDebug();
      break;
    case 'f':
    case 'F':
      info();
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

void onKeyPress(int key) {
  if (key > 127) { 
    // If bit 7 is set (non-ASCII key) we should set arrow keys
    bool isArrowKey = false;

    switch (key) {
      case 0xD7: { // RIGHT
        IO[KBD_STAT - IO_START] |= 0b00000001;
        isArrowKey = true;
        break;
      }
      case 0xD8: { // LEFT
        IO[KBD_STAT - IO_START] |= 0b00000010;
        isArrowKey = true;
        break;
      }
      case 0xD9: { // DOWN  
        IO[KBD_STAT - IO_START] |= 0b00000100;
        isArrowKey = true;
        break;
      }
      case 0xDA: { // UP
        IO[KBD_STAT - IO_START] |= 0b00001000;
        isArrowKey = true;
        break;
      }
    }

    // And set interrupt if arrow key interrupts are enabled
    if (isArrowKey && (IO[KBD_CTRL - IO_START] & 0x02) != 0) {
      digitalWriteFast(IRQB, LOW);

      // Set bit 6 of KBD_STAT register to indicate arrow key interrupt asserted
      IO[KBD_STAT - IO_START] |= 0b01000000;
    }
  } else {
    // Otherwise, we set the keyboard data register to key value and set data ready bit
    IO[KBD_DATA - IO_START] = (key | 0x80);

    // And set interrupt if keyboard interrupts are enabled
    if ((IO[KBD_CTRL - IO_START] & 0x01) != 0) {
      digitalWriteFast(IRQB, LOW);

      // Set bit 7 of KBD_STAT register to indicate keyboard interrupt asserted
      IO[KBD_STAT - IO_START] |= 0b10000000;
    } 
  }

  if (debugMode == DEBUG_KEYBOARD) {
    Serial.print("Key Pressed: ");
    Serial.print((char)key);

    char output[64];

    sprintf(
      output, 
      " | %c%c%c%c%c%c%c%c | 0x%02X",
      BYTE_TO_BINARY(key),
      key
    );

    Serial.println(output);
  }
}

void onKeyRelease(int key) {
  if (key > 127) { 
    // If bit 7 is set (non-ASCII key) we should clear arrow keys
    bool isArrowKey = false;

    switch (key) {
      case KEYD_RIGHT: { // RIGHT
        IO[KBD_STAT - IO_START] &= 0b11111110;
        isArrowKey = true;
        break;
      }
      case KEYD_LEFT: { // LEFT
        IO[KBD_STAT - IO_START] &= 0b11111101;
        isArrowKey = true;
        break;
      }
      case KEYD_DOWN: { // DOWN  
        IO[KBD_STAT - IO_START] &= 0b11111011;
        isArrowKey = true;
        break;
      }
      case KEYD_UP: { // UP
        IO[KBD_STAT - IO_START] &= 0b11110111;
        isArrowKey = true;
        break;
      }
    }

    // And set interrupt if arrow key interrupts are enabled
    if (isArrowKey && (IO[KBD_CTRL - IO_START] & 0x02) != 0) {
      digitalWriteFast(IRQB, LOW);

      // Set bit 6 of KBD_STAT register to indicate arrow key interrupt asserted
      IO[KBD_STAT - IO_START] |= 0b01000000;
    }
  }

  if (debugMode == DEBUG_KEYBOARD) {
    Serial.print("Key Released: ");
    Serial.print((char)key);

    char output[64];

    sprintf(
      output, 
      " | %c%c%c%c%c%c%c%c | 0x%02X",
      BYTE_TO_BINARY(key),
      key
    );

    Serial.println(output);
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
  Serial.println(formatDebugMode());
  Serial.print("Frequency: ");
  Serial.println(FREQS[freqIndex]);
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Date / Time: ");
  Serial.println(formatDateTime());
  Serial.println();
  Serial.println("----------------------------------------------------------");
  Serial.println("| (R)un / Stop        | (S)tep          | Rese(T)        |");
  Serial.println("----------------------------------------------------------");
  Serial.println("| Toggle R(A)M        | Toggle R(O)M    | (L)ist ROMs    |");
  Serial.println("----------------------------------------------------------");
  Serial.println("| Toggle (I)O         | Sna(P)shot RAM / IO              |");
  Serial.println("----------------------------------------------------------");
  Serial.println("| (+/-) Clk Frequency | Toggle De(B)ug  | In(F)o         |");
  Serial.println("----------------------------------------------------------");
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
  if (debugMode == DEBUG_NONE) {
    Serial.print("Tick…  ");
  }
  delay(100);
  onHigh();
  if (debugMode == DEBUG_NONE) {
    Serial.println("Tock…");
  }
  delay(100);
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
  String RAMpath = "Snapshots/";
  String IOpath = "Snapshots/";

  RAMpath.append(time);
  RAMpath.append(" - RAM.bin");
  IOpath.append(time);
  IOpath.append(" - IO.bin");

  File ramSnapshot = SD.open(RAMpath.c_str(), FILE_WRITE);

  if (ramSnapshot) {
    for(unsigned int i = 0; i < (RAM_END - RAM_START); i++) {
      ramSnapshot.write(RAM[i]);
    }
    ramSnapshot.close();
  }

  File ioSnapshot = SD.open(IOpath.c_str(), FILE_WRITE);

  if (ioSnapshot) {
    for(unsigned int i = 0; i < (IO_END - IO_START); i++) {
      ioSnapshot.write(IO[i]);
    }
    ioSnapshot.close();
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

  Serial.println(isRunning ? "Running…" : "Stopped");
}

void toggleDebug() {
  switch (debugMode) {
    case DEBUG_NONE:
      debugMode = DEBUG_6502;
      break;
    case DEBUG_6502:
      debugMode = DEBUG_KEYBOARD;
      break;
    case DEBUG_KEYBOARD:
      debugMode = DEBUG_MOUSE;
      break;
    case DEBUG_MOUSE:
      debugMode = DEBUG_JOYSTICK;
      break;
    case DEBUG_JOYSTICK:
      debugMode = DEBUG_NONE;
      break;
  }

  Serial.print("Debug: ");
  Serial.println(formatDebugMode());
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
  if (!SD.exists("ROMS")) {
    SD.mkdir("ROMS");
  }

  for (unsigned int i = 0; i < 100; i++) {
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

  for (unsigned int j = (romPage * 10); j < ((romPage * 10) + 10); j++) {
    Serial.print("(");
    Serial.print(j - (romPage * 10));
    Serial.print(")");
    Serial.print(" - ");
    Serial.println(ROMs[j]);
  }

  Serial.println();
  Serial.print("Page: ");
  Serial.print(romPage + 1);
  Serial.println(" | ([) Prev Page | Next Page (]) |");
  Serial.println();
}

void loadROM(unsigned int index) {
  String directory = "ROMS/";
  String filename = ROMs[(romPage * 10) + index];

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

    romFile = ROMs[index];

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
    romPage = 9;
  }

  listROMs();
}

void nextPage() {
  if (romPage < 9) {
    romPage++;
  } else {
    romPage = 0;
  }

  listROMs();
}

String formatDateTime() {
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

String formatDebugMode() {
  switch (debugMode) {
    case DEBUG_NONE:
      return "Disabled";
    case DEBUG_6502:
      return "6502";
    case DEBUG_KEYBOARD:
      return "Keyboard";
    case DEBUG_MOUSE:
      return "Mouse";
    case DEBUG_JOYSTICK:
      return "Joystick";
    default:
      return "Unknown";
  }
}

time_t syncTime() {
  return Teensy3Clock.get();
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
  pinMode(RESB, OUTPUT);
  pinMode(SYNC, INPUT);
  pinMode(RWB, INPUT);
  pinMode(RDY, OUTPUT);
  pinMode(BE, OUTPUT);
  pinMode(PHI2, OUTPUT);
  pinMode(NMIB, OUTPUT);
  pinMode(IRQB, OUTPUT);

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

void initUSB() {
  usb.begin();
  keyboard.attachPress(onKeyPress);
  keyboard.attachRelease(onKeyRelease);
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