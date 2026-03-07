#include <Arduino.h>
#include <SPI.h>

/* SPI CONTROLLER REGISTERS */
/* |    |      | 7    | 6  | 5  | 4   | 3   | 2       | 1          | 0      | */
/* | $0	| DATA |                        Data                                | */
/* | $1	| CTRL | BUSY	| CD | WP | SS1 | SS0 | AUTO-TX | SLOW CLOCK | SELECT | */

// PD0
#define D0 0     // 6502 Data Bus D0-D7
#define D1 1
#define D2 2
#define D3 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7

// PC0
#define RS0 A0    // 6502 Address Bus A0
#define PHI2 A1   // 6502 PHI2
#define CSB A2    // Chip Select (Active Low)
#define RWB A3    // 6502 Read / ~Write
#define CD A4     // SD Card - Card Detect
#define WP A5     // SD Card - Write Protect

#define CS0 8     // SD Card
#define CS1 9     // Flash Memory
#define CS2 10    // External SPI Header
#define MOSI 11
#define MISO 12
#define SCK 13

// CTRL Register Bit Masks
#define CTRL_BUSY       0x80  // Bit 7: Busy flag (read-only)
#define CTRL_CD         0x40  // Bit 6: Card Detect (read-only)
#define CTRL_WP         0x20  // Bit 5: Write Protect (read-only)
#define CTRL_SS1        0x10  // Bit 4: Slave Select bit 1
#define CTRL_SS0        0x08  // Bit 3: Slave Select bit 0
#define CTRL_AUTO_TX    0x04  // Bit 2: Auto-transmit on read
#define CTRL_SLOW_CLK   0x02  // Bit 1: Slow clock mode
#define CTRL_SELECT     0x01  // Bit 0: Chip Select enable

// SPI Clock speeds
#define SPI_FAST_SPEED  4000000   // 4 MHz for normal operation
#define SPI_SLOW_SPEED  400000    // 400 kHz for SD card initialization

// Global registers
volatile uint8_t dataRegister = 0x00;
volatile uint8_t ctrlRegister = 0x00;
volatile bool spiInProgress = false;

// Function prototypes
void onInterrupt();
void updateChipSelects();
void updateSpiSpeed();
uint8_t spiTransfer(uint8_t data);
void setDataBusOutput();
void setDataBusInput();
void writeDataBus(uint8_t data);
uint8_t readDataBus();

void setup() {
  // Configure 6502 bus pins
  pinMode(RS0, INPUT);
  pinMode(PHI2, INPUT);
  pinMode(CSB, INPUT);
  pinMode(RWB, INPUT);
  pinMode(CD, INPUT_PULLUP);
  pinMode(WP, INPUT_PULLUP);
  
  // Configure CS pins as outputs (inactive high)
  pinMode(CS0, OUTPUT);
  pinMode(CS1, OUTPUT);
  pinMode(CS2, OUTPUT);
  digitalWrite(CS0, HIGH);
  digitalWrite(CS1, HIGH);
  digitalWrite(CS2, HIGH);
  
  // Initialize data bus as inputs with pull-ups
  setDataBusInput();
  
  // Initialize SPI
  SPI.begin();
  SPI.beginTransaction(SPISettings(SPI_FAST_SPEED, MSBFIRST, SPI_MODE0));
  SPI.endTransaction();
  
  // Attach interrupt on PHI2 rising edge
  attachInterrupt(digitalPinToInterrupt(PHI2), onInterrupt, RISING);
}

void loop() {
  // Main loop can be used for other tasks if needed
}

void onInterrupt() {
  // Check if this chip is selected
  if (digitalRead(CSB) == HIGH) {
    setDataBusInput();
    return;
  }
  
  // Read control signals
  bool isWrite = (digitalRead(RWB) == LOW);
  bool regSelect = digitalRead(RS0);  // 0 = DATA, 1 = CTRL
  
  if (isWrite) {
    // 6502 is writing to our registers
    setDataBusInput();
    delayMicroseconds(1);  // Allow bus to settle
    uint8_t data = readDataBus();
    
    if (regSelect == LOW) {
      // Write to DATA register
      dataRegister = data;
      // Start SPI transfer
      spiInProgress = true;
      updateChipSelects();
      updateSpiSpeed();
      dataRegister = spiTransfer(data);
      spiInProgress = false;
    } else {
      // Write to CTRL register (only writable bits)
      // Preserve read-only bits (BUSY, CD, WP)
      ctrlRegister = (ctrlRegister & 0xE0) | (data & 0x1F);
      updateChipSelects();
      updateSpiSpeed();
    }
  } else {
    // 6502 is reading from our registers
    uint8_t outputData;
    
    if (regSelect == LOW) {
      // Read from DATA register
      outputData = dataRegister;
      
      // Check if AUTO-TX is enabled
      if (ctrlRegister & CTRL_AUTO_TX) {
        // Start SPI transfer with dummy byte $FF
        spiInProgress = true;
        updateChipSelects();
        updateSpiSpeed();
        dataRegister = spiTransfer(0xFF);
        spiInProgress = false;
      }
    } else {
      // Read from CTRL register
      // Build CTRL register with current status
      outputData = ctrlRegister & 0x1F;  // User-writable bits
      
      // Add BUSY flag
      if (spiInProgress) {
        outputData |= CTRL_BUSY;
      }
      
      // Add CD flag (inverted because pull-up)
      if (digitalRead(CD) == LOW) {
        outputData |= CTRL_CD;
      }
      
      // Add WP flag (inverted because pull-up)
      if (digitalRead(WP) == LOW) {
        outputData |= CTRL_WP;
      }
    }
    
    setDataBusOutput();
    writeDataBus(outputData);
  }
}

void updateChipSelects() {
  // Get slave select bits
  uint8_t slaveSelect = (ctrlRegister >> 3) & 0x03;  // Extract SS1:SS0
  bool selectActive = (ctrlRegister & CTRL_SELECT);
  
  // Deactivate all CS lines first
  digitalWrite(CS0, HIGH);
  digitalWrite(CS1, HIGH);
  digitalWrite(CS2, HIGH);
  
  // Activate selected CS if SELECT bit is set
  if (selectActive) {
    switch (slaveSelect) {
      case 0:
        digitalWrite(CS0, LOW);  // SD Card
        break;
      case 1:
        digitalWrite(CS1, LOW);  // Flash Memory
        break;
      case 2:
        digitalWrite(CS2, LOW);  // External SPI Header
        break;
      case 3:
        // Reserved / no device
        break;
    }
  }
}

void updateSpiSpeed() {
  uint32_t speed = (ctrlRegister & CTRL_SLOW_CLK) ? SPI_SLOW_SPEED : SPI_FAST_SPEED;
  SPI.beginTransaction(SPISettings(speed, MSBFIRST, SPI_MODE0));
  SPI.endTransaction();
}

uint8_t spiTransfer(uint8_t data) {
  uint32_t speed = (ctrlRegister & CTRL_SLOW_CLK) ? SPI_SLOW_SPEED : SPI_FAST_SPEED;
  SPI.beginTransaction(SPISettings(speed, MSBFIRST, SPI_MODE0));
  uint8_t result = SPI.transfer(data);
  SPI.endTransaction();
  return result;
}

void setDataBusOutput() {
  // Configure D0-D7 as outputs
  DDRD = 0xFF;
}

void setDataBusInput() {
  // Configure D0-D7 as inputs with pull-ups
  DDRD = 0x00;
  PORTD = 0xFF;
}

void writeDataBus(uint8_t data) {
  PORTD = data;
}

uint8_t readDataBus() {
  return PIND;
}