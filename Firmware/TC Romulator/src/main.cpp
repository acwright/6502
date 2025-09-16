#include <Arduino.h>
#include <SD.h>

#include "pins.h"
#include "constants.h"

byte ROM[ROM_END - ROM_START + 1];

void onCommand(char command);

void info();
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

void initPins();
void initTransceivers();
void initSD();
void initROM();

word readAddress();
void writeData(byte d);

bool isOutputting = false;

String romFile = "None";
String cartFile = "None";
bool fileCtx = FILE_CTX_ROM;

String ROMs[ROM_MAX];
unsigned int romPage = 0;

String Carts[CART_MAX];
unsigned int cartPage = 0;

//
// MAIN LOOPS
//

void setup() {
  initPins();
  initTransceivers();
  initROM();
  initSD();

  // Serial.begin(9600);

  // info();

  noInterrupts(); // This will cause Teensy to not be programmable without pushing the button
}

FASTRUN void loop() {
  if (
    digitalReadFast(PHI2) == HIGH && 
    digitalReadFast(A15) == HIGH &&
    (digitalReadFast(A14) == HIGH || digitalReadFast(A13) == HIGH) && 
    digitalReadFast(RWB) == HIGH &&
    !isOutputting
  ) {
    digitalWriteFast(OE1, LOW);
    digitalWriteFast(OE3, HIGH);
    
    word address      = readAddress();

    digitalWriteFast(OE1, HIGH);

    writeData(ROM[address - ROM_START]);

    digitalWriteFast(OE3, LOW);

    isOutputting = true;
  } else if (digitalReadFast(PHI2) == LOW && isOutputting) {
    isOutputting = false;

    digitalWriteFast(OE1, LOW);
    digitalWriteFast(OE3, HIGH);
  }

  // Check for key press
  // if (Serial.available()) 
  // {
  //   onCommand(Serial.read());
  // }
}

//
// EVENTS
//

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
  Serial.println("ee8ee 8eeee8   8eee8  8eee88 8ee8ee8 8   8 8     8eeee8 ee8ee 8eee88 8eee8  ");                        
  Serial.println("  8   8    e   8   8  8    8 8  8  8 8   8 8     8    8   8   8    8 8   8  ");
  Serial.println("  8e  8e       8eee8e 8    8 8e 8  8 8e  8 8e    8eeee8   8e  8    8 8eee8e ");
  Serial.println("  88  88       88   8 8    8 88 8  8 88  8 88    88   8   88  8    8 88   8 ");
  Serial.println("  88  88   e   88   8 8    8 88 8  8 88  8 88    88   8   88  8    8 88   8 ");
  Serial.println("  88  88eee8   88   8 8eeee8 88 8  8 88ee8 88eee 88   8   88  8eeee8 88   8");
  Serial.println();
  Serial.print("TC Emulator | Version: ");
  #ifdef VERSION
  Serial.print(VERSION);
  #endif
  Serial.println();
  Serial.println("---------------------------------");
  Serial.println("| Created by A.C. Wright © 2025 |");
  Serial.println("---------------------------------");
  Serial.println();
  Serial.print("ROM: ");
  Serial.println(romFile);
  Serial.print("Cart: ");
  Serial.println(cartFile);
  Serial.println();
  Serial.println("--------------------------------------------------------------");
  Serial.println("| List RO(M)s          | List (C)arts      | In(F)o          |");
  Serial.println("--------------------------------------------------------------");
  Serial.println();
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
// INITIALIZATION
//

void initPins() {
  pinMode(AD0, INPUT);
  pinMode(AD1, INPUT);
  pinMode(AD2, INPUT);
  pinMode(AD3, INPUT);
  pinMode(AD4, INPUT);
  pinMode(AD5, INPUT);
  pinMode(AD6, INPUT);
  pinMode(AD7, INPUT);
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);
  pinMode(A12, INPUT);
  pinMode(A13, INPUT);
  pinMode(A14, INPUT);
  pinMode(A15, INPUT);
  
  pinMode(RESB, INPUT);
  pinMode(IRQB, INPUT);
  pinMode(NMIB, INPUT);
  pinMode(RDY, INPUT);
  pinMode(BE, INPUT);
  pinMode(SYNC, INPUT);
  pinMode(RWB, INPUT);
  pinMode(PHI2, INPUT);

  pinMode(GPIO0, OUTPUT);
  pinMode(GPIO1, OUTPUT);
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO3, OUTPUT);
  pinMode(GPIO4, OUTPUT);
  pinMode(GPIO5, OUTPUT);
  pinMode(GPIO6, OUTPUT);
  pinMode(GPIO7, OUTPUT);

  pinMode(OE1, OUTPUT);
  pinMode(OE2, OUTPUT);
  pinMode(OE3, OUTPUT);
  pinMode(OE4, OUTPUT);
  pinMode(OE5, OUTPUT);

  pinMode(DIR1, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(DIR3, OUTPUT);
  pinMode(DIR4, OUTPUT);
  pinMode(DIR5, OUTPUT);
}

void initTransceivers() {
  digitalWriteFast(DIR1, HIGH); // A -> B
  digitalWriteFast(DIR2, HIGH); // A -> B
  digitalWriteFast(DIR3, LOW);  // B -> A
  digitalWriteFast(DIR4, HIGH); // A -> B
  digitalWriteFast(DIR5, LOW);  // B -> A

  digitalWriteFast(OE1, LOW);
  digitalWriteFast(OE2, LOW);
  digitalWriteFast(OE3, HIGH);
  digitalWriteFast(OE4, LOW);
  digitalWriteFast(OE5, HIGH);
}

void initSD() {
  SD.begin(BUILTIN_SDCARD);

  if (SD.exists("ROM.bin")) {
    loadROMPath("ROM.bin");
    romFile = "ROM.bin";
  }
  if (SD.exists("Cart.bin")) {
    loadCartPath("Cart.bin");
    cartFile = "Cart.bin";
  }
}

void initROM() {
  for (word a = 0x0000; a < (ROM_END - ROM_START + 1); a++) {
    ROM[a] = 0x00; // Fill the ROM with 0x00's by default
  }
}

// 
// UTILITIES
// 

FASTRUN word readAddress() {
  // AD0-AD7 GPIO6 bit 16-23
  // A8-A15 GPIO6 bit 24-31

  GPIO6_GDIR &= 0x0000FFFF; // Set bits 16-31  to input
  
  return (GPIO6_PSR & 0xFFFF0000) >> 16;
}

FASTRUN void writeData(byte data) {
  // AD0-AD7 GPIO6 bit 16-23
  // A8-A15 GPIO6 bit 24-31

  GPIO6_GDIR |= 0x00FF0000; // Set bits 16-23  to output
  GPIO6_DR = (GPIO6_DR & 0x0000FFFF) | (data << 16);
}