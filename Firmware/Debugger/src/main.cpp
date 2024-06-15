#include <Arduino.h>
#include <TimerOne.h>
#include <TimeLib.h>
#include <Bounce2.h>
#include <SD.h>
#include <EEPROM.h>
#include <USBHost_t36.h>

#if DEVBOARD
#include "devboard.h"
#elif RETROSHIELD
#include "retroshield.h"
#elif RETROSHIELDADAPTER
#include "retroshieldadapter.h"
#endif
#include "utilities.h"

/*      MEMORY MAP      */
/* -------------------- */
/* |                  | */
/* |        ROM       | */
/* |    $A000-$FFFF   | */
/* |                  | */
/* -------------------- */
/* |    IO 7 ($9C00)  | */
/* -------------------- */
/* |    IO 6 ($9800)  | */
/* -------------------- */
/* |    IO 5 ($9400)  | */
/* -------------------- */
/* |    IO 4 ($9000)  | */
/* -------------------- */
/* |    IO 3 ($8C00)  | */
/* -------------------- */
/* |    IO 2 ($8800)  | */
/* -------------------- */
/* |    IO 1 ($8400)  | */
/* -------------------- */
/* |    IO 0 ($8000)  | */
/* -------------------- */
/* |                  | */
/* |        RAM       | */
/* |    $0000-$7FFF   | */
/* |                  | */
/* -------------------- */

#define RAM_START   0x0000
#define RAM_END     0x7FFF
#define IO_START    0x8000
#define IO_END      0x9FFF
#define ROM_START   0x8000
#define ROM_CODE    0xA000
#define ROM_END     0xFFFF

                            /*                            IO REGISTERS                        */
                            // | bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0 |  
#define KBD_DATA    0       // | DRDY  |                 ASCII KEY CODE                        |  Keyboard Data:    Data Ready (Bit 7) -> [ Ascii Key Code (Bit 6-0) ] (Read from KBD_DATA to clear keyboard INT)
#define KBD_CTRL    1       // |                       NA                      | ARINT | KBINT |  Keyboard Control: (Bit 7-2 unused) -> Enable (1) / Disable (0) Arrow Key Interrupts (Bit 1) -> Enable (1) / Disable (0) Keyboard Interrupts (Bit 0)
#define KBD_STAT    2       // | KBINT | ARINT |       NA      |   UP  |  DOWN |  LEFT | RIGHT |  Keyboard Status:  Interrupt Status (Bit 7-6) -> (Bit 5-4 unused) -> Arrow Keys Pressed (Bit 3-0) (Read from KBD_STAT to clear arrow key INT)
                            /*                                                                    Notes:            USB Keyboard input can be read by 6502 using either a polling or interrupt strategy */

#define DSP_DATA    3       // |                           ASCII CODE                          |  Display Data:     [ Extended Ascii Code (Bit 7-0) ]
#define DSP_CTRL    4       // |                               NA                              |  Display Control:  Unused for now...
                            /*                                                                    Notes:            Outputs character to serial terminal. */

#define MOUSE_X     5       // |  DIR  |                    VELOCITY                           |  Mouse X:          Direction + Right (0) - Left (1) -> Velocity from 0-127
#define MOUSE_Y     6       // |  DIR  |                    VELOCITY                           |  Mouse Y:          Direction + Down (0) - Up (1) -> Velocity from 0-127
#define MOUSE_W     7       // |  DIR  |                    VELOCITY                           |  Mouse Wheel:      Direction + Down (0) - Up (1) -> Velocity from 0-127
#define MOUSE_BTN   8       // |                  NA                   |  MID  | RIGHT |  LEFT |  Mouse Buttons:    (Bit 7-3 unused) -> Middle, Right, Left Buttons Pressed (Bit 2-0)

#define JOY_BTNL    9       // |   Y   |   X   |   B   |   A   | LFUNC | RFUNC |       NA      |  Joystick Btn L:           Y -> X -> B -> A -> L Function (View) -> R Function (Menu) -> (Bit 1-0 unused)
#define JOY_BTNH   10       // | RABTN | LABTN |  RBTN |  LBTN | RIGHT |  LEFT |  DOWN |   UP  |  Joystick Btn H:           Right Analog Btn -> Left Analog Btn -> Right Btn -> Left Btn -> D-Right -> D-Left -> D-Down -> D-Up
#define JOY_LAXL   11       // |                   LEFT ANALOG X (LOW BYTE)                    |  Joystick Left Analog X:   -32768(Left) to +32768(Right) (Low Byte)
#define JOY_LAXH   12       // |                   LEFT ANALOG X (HIGH BYTE)                   |  Joystick Left Analog X:   -32768(Left) to +32768(Right) (High Byte)
#define JOY_LAYL   13       // |                   LEFT ANALOG Y (LOW BYTE)                    |  Joystick Left Analog Y:   -32768(Down) to +32768(Up) (Low Byte)
#define JOY_LAYH   14       // |                   LEFT ANALOG Y (HIGH BYTE)                   |  Joystick Left Analog Y:   -32768(Down) to +32768(Up) (High Byte)
#define JOY_RAXL   15       // |                  RIGHT ANALOG X (LOW BYTE)                    |  Joystick Right Analog X:  -32768(Left) to +32768(Right) (Low Byte)
#define JOY_RAXH   16       // |                  RIGHT ANALOG X (HIGH BYTE)                   |  Joystick Right Analog X:  -32768(Left) to +32768(Right) (High Byte)
#define JOY_RAYL   17       // |                  RIGHT ANALOG Y (LOW BYTE)                    |  Joystick Right Analog Y:  -32768(Down) to +32768(Up) (Low Byte)
#define JOY_RAYH   18       // |                  RIGHT ANALOG Y (HIGH BYTE)                   |  Joystick Right Analog Y:  -32768(Down) to +32768(Up) (High Byte)
#define JOY_LTRL   19       // |                   LEFT TRIGGER (LOW BYTE)                     |  Joystick Left Trigger:    +0 to +1024 (Low Byte)
#define JOY_LTRH   20       // |                       NA                      |    LTRIG(H)   |  Joystick Left Trigger:    +0 to +1024 (High Byte) (Bit 7-2 unused)
#define JOY_RTRL   21       // |                  RIGHT TRIGGER (LOW BYTE)                     |  Joystick Right Trigger:   +0 to +1024 (Low Byte)
#define JOY_RTRH   22       // |                       NA                      |    RTRIG(H)   |  Joystick Right Trigger:   +0 to +1024 (High Byte) (Bit 7-2 unused)
                            /*                                                                    Notes:                    XBOX 360 and XBOX One controllers supported for now... */

#define RTC_SEC    32       // |                            SECONDS                            |  Real Time Clock:  Second (0-59)
#define RTC_MIN    33       // |                            MINUTES                            |  Real Time Clock:  Minute (0-59)
#define RTC_HR     34       // |                             HOURS                             |  Real Time Clock:  Hour (0-23)
#define RTC_DAY    35       // |                              DAY                              |  Real Time Clock:  Day (1-31)
#define RTC_MON    36       // |                             MONTH                             |  Real Time Clock:  Month (1-12)
#define RTC_YR     37       // |                             YEAR                              |  Real Time Clock:  Year (0-99) (Offset from 1970)
                            /*                                                                    Notes:            Real Time Clock IO registers are read-only. Teensy Loader automatically initializes the RTC to your PC's time while uploading. */
                            /*                                                                                      If a coin cell is connected to VBAT, the RTC will continue keeping time while power is turned off. See: https://www.pjrc.com/store/teensy41.html */

#define GPIO_DATA  40       // |  INT3 |  INT2 |  INT1 |  INT0 | GPIO3 | GPIO2 | GPIO1 | GPIO0 |  GPIO Data:    Interrupt Status (Bit 7-4) -> GPIO Data (Bit 3-0) (Read from GPIO_DATA to clear INTs)
#define GPIO_CTRL  41       // | INEN3 | INEN2 | INEN1 | INEN0 | GDIR3 | GDIR2 | GDIR1 | GDIR0 |  GPIO Control: Interrupt Enable (Bit 7-4) -> GPIO Direction (Bit 3-0) (LOW = IN | HIGH = OUT)
                            /*                                                                    Notes:        GPIO 3-0 available on Dev Board. GPIO 0 ONLY available on Retroshield. */

#define TIMER_PERIOD              1 // 1 uS
#define FREQS                     (String[20])  { "1 Hz", "2 Hz", "4 Hz", "8 Hz", "16 Hz", "32 Hz", "64 Hz", "122 Hz", "244 Hz", "488 Hz", "976 Hz", "1.9 kHz", "3.9 kHz", "7.8 kHz", "15.6 kHz", "31.2 kHz", "62.5 kHz", "125 kHz", "250 kHz", "500 kHz" }
#define FREQ_DELAYS               (int[20])     { 250000, 125000, 62500, 31250, 15625, 7812, 3906, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1, 0 }

#define DEBOUNCE        5          // 5 milliseconds

#define DEBUG_NONE      0
#define DEBUG_KEYBOARD  1
#define DEBUG_MOUSE     2
#define DEBUG_JOYSTICK  3

#define IO_BANK_SIZE    0x3FF

#define PAGE_SIZE       1024
#define RAM_PAGES       32
#define IO_PAGES        8
#define ROM_PAGES       32
#define ROM_MAX         256

#define VERSION         "1.0.0"

const word IO_BANKS[8] { 0x8000, 0x8400, 0x8800, 0x8C00, 0x9000, 0x9400, 0x9800, 0x9C00 };

byte RAM[RAM_END - RAM_START + 1];
byte IO[IO_END - IO_START + 1];
byte ROM[ROM_END - ROM_START + 1];

Button intButton = Button();
Button stepButton = Button();
Button runStopButton = Button();

void onTick();
void onCycleBegin();
void onCycleEnd();
void onCommand(char command);
void onKey(char key);
void onKeyPress(int key);
void onKeyRelease(int key);
void onMouseAvailable();
void onJoystickAvailable();

void initPins();
void initButtons();
void initRAM();
void initIO();
void initROM();
void initSD();
void initUSB();

void setAddrDirIn();
void setDataDirIn();
void setDataDirOut();
word readAddress();
byte readData();
void writeData(byte d);

void reset();
void info();
void log();
void step(); 
void snapshot();
void decreaseFrequency();
void increaseFrequency();
void toggleRunStop();
void toggleDebug();
void toggleKeyboardCapture();
void toggleRAM();
void toggleIO();
void toggleIOBank();
void toggleROM();
void readROMs();
void listROMs();
void loadROM(unsigned int index);
void prevPage();
void nextPage();

String formatDateTime();
String formatDebugMode();

time_t syncTime();

unsigned int freqIndex = 19;      // 500 kHz
volatile unsigned int freqCounter = 0;
volatile unsigned int stepCounter = 0;
volatile bool isEdge = false;
volatile bool isRunning = false;
volatile bool isStepping = false;
volatile bool isLogging = false;

bool RAMEnabled = true;
bool IOEnabled = true;
bool ROMEnabled = true;
String romFile = "None";
byte IOBank = 7;                  // By default, debugger IO bank is $9C00

byte debugMode = DEBUG_NONE;
bool keyboardCaptureEnabled = false;

word address = 0;
byte data = 0;
bool readWrite = LOW;
bool be = HIGH;
bool rdy = HIGH;
bool sync = LOW;
bool sob = HIGH;
bool resb = HIGH;
bool irqb = HIGH;
bool nmib = HIGH;

String ROMs[ROM_MAX];
unsigned int romPage = 0;

USBHost usb;
USBHub hub1(usb);
USBHIDParser hid1(usb);
USBHIDParser hid2(usb);
USBHIDParser hid3(usb);
KeyboardController keyboard(usb);
MouseController mouse(usb);
JoystickController joystick(usb);

//
// MAIN LOOPS
//

void setup() {
  initPins();
  initButtons();
  initRAM();
  initIO();
  initROM();

  setSyncProvider(syncTime);
  setSyncInterval(1);

  Timer1.initialize(TIMER_PERIOD);
  Timer1.attachInterrupt(onTick);

  initSD();
  initUSB();

  Serial.begin(9600);
  
  // Wait up to 1 sec for serial to connect
  int timeout = 1000;
  while (!Serial && timeout--) {
    delay(1);
  }

  info();
  reset();
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

  if (Serial.available()) 
  {
    byte key = Serial.read();

    if (keyboardCaptureEnabled && key != 0x1B) {
      onKey(key);
    } else {
      onCommand(key);
    }
  }

  // Update USB
  usb.Task();

  if (mouse.available()) {
    onMouseAvailable();
  }

  if (joystick.available()) {
    onJoystickAvailable();
  }

  // Update GPIO
  if ((IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00000001) != 1) { // GPIO0 is set to INPUT
    if (digitalReadFast(GPIO0)) {
      IO[(IO_BANKS[IOBank] + GPIO_DATA) - IO_START] |= 1;

      // Set interrupt if enabled
      if ((IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00010000) >> 4) {
        digitalWriteFast(IRQB, LOW);
      }
    } else {
      IO[(IO_BANKS[IOBank] + GPIO_DATA) - IO_START] &= ~1;
    }
  }

  #ifdef DEVBOARD
  if ((IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00000010) != 1) { // GPIO1 is set to INPUT
    if (digitalReadFast(GPIO1)) {
      IO[(IO_BANKS[IOBank] + GPIO_DATA) - IO_START] |= (1 << 1);

      // Set interrupt if enabled
      if ((IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00100000) >> 5) {
        digitalWriteFast(IRQB, LOW);
      }
    } else {
      IO[(IO_BANKS[IOBank] + GPIO_DATA) - IO_START] &= ~(1 << 1);
    }
  }
  if ((IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00000100) != 1) { // GPIO2 is set to INPUT
    if (digitalReadFast(GPIO2)) {
      IO[(IO_BANKS[IOBank] + GPIO_DATA) - IO_START] |= (1 << 2);

      // Set interrupt if enabled
      if ((IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b01000000) >> 6) {
        digitalWriteFast(IRQB, LOW);
      }
    } else {
      IO[(IO_BANKS[IOBank] + GPIO_DATA) - IO_START] &= ~(1 << 2);
    }
  }
  if ((IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00001000) != 1) { // GPIO3 is set to INPUT
    if (digitalReadFast(GPIO3)) {
      IO[(IO_BANKS[IOBank] + GPIO_DATA) - IO_START] |= (1 << 3);

      // Set interrupt if enabled
      if ((IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b10000000) >> 7) {
        digitalWriteFast(IRQB, LOW);
      }
    } else {
      IO[(IO_BANKS[IOBank] + GPIO_DATA) - IO_START] &= ~(1 << 3);
    }
  }
  #endif

  // Sync the clock
  now();
}

//
// EVENTS
//

void onTick()
{
  // This is called in the timer interrupt so extra care should be taken!

  if (freqCounter <= 0) {
    if (isRunning) {
      if (!digitalReadFast(PHI2)) {
        digitalWriteFast(PHI2, HIGH);
        onCycleBegin();
      } else {
        onCycleEnd();
        digitalWriteFast(PHI2, LOW);
      }
    }

    freqCounter = FREQ_DELAYS[freqIndex];
  } else {
    freqCounter--;
  }

  if (stepCounter <= 0) {
    if (isStepping) {
      if (!digitalReadFast(PHI2)) {
        digitalWriteFast(PHI2, HIGH);
        onCycleBegin();
      } else {
        onCycleEnd();
        digitalWriteFast(PHI2, LOW);
        isStepping = false; // We are done stepping...
        isLogging = true;   // Log on next tick
      }
    }

    // Handle step logging
    if (isLogging) {
      log();
      isLogging = false;
    }

    stepCounter = FREQ_DELAYS[2]; // Use 4 Hz for step counter
  } else {
    stepCounter--;
  }
}

void onCycleBegin() {
  // Read the address and r/w at the beginning of cycle (low to high transition)
  address = readAddress();
  readWrite = digitalReadFast(RWB);

  #ifdef DEVBOARD
  sync = digitalReadFast(SYNC);
  be = digitalReadFast(BE);
  #elif RETROSHIELD
  sob = digitalReadFast(SOB);
  #elif RETROSHIELDADAPTER
  sync = digitalReadFast(SYNC);
  #endif

  if (readWrite == HIGH) { // READING
    // Check if in IO space first since it overlaps ROM space...
    if ((address >= IO_START) && (address <= IO_END)) { // IO
      // Only output data if address is in selected IOBank area and IO is enabled
      if ((address >= IO_BANKS[IOBank]) && (address <= IO_BANKS[IOBank] + IO_BANK_SIZE) && IOEnabled) {
        setDataDirOut();
        writeData(IO[address - IO_START]);
      } else {
        setDataDirIn();
      }

      // Handle IO post processing
      switch(address - IO_BANKS[IOBank]) {
        case KBD_DATA: {
          // Read from KBD_DATA clears data ready bit
          IO[(IO_BANKS[IOBank] + KBD_DATA) - IO_START] &= 0b01111111;

          // Read from KBD_DATA clears keyboard interrupt status bit
          IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] &= 0b01111111;

          // Are there any more interrupts? If not, clear interrupt line (TODO: We should also check for other IO asserting interrupt...)
          if ((IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] & 0b11000000) == 0) {
            digitalWriteFast(IRQB, HIGH);
          }

          break;
        }
        case KBD_STAT: {
          // Read from KBD_STAT clears arrow key interrupt status bit
          IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] &= 0b10111111;

          // Are there any more interrupts? If not, clear interrupt line (TODO: We should also check for other IO asserting interrupt...)
          if ((IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] & 0b11000000) == 0) {
            digitalWriteFast(IRQB, HIGH);
          }

          break;
        }
        case GPIO_DATA: {
          // Read from GPIO_DATA clears interrupt status bits
          IO[(IO_BANKS[IOBank] + GPIO_DATA) - IO_START] &= 0b00001111;

          // Clear interrupt line (TODO: We should also check for other IO asserting interrupt...)
          digitalWriteFast(IRQB, HIGH);

          break;
        }
        default:
          break;
      }
    } else if ((address >= RAM_START) && (address <= RAM_END) && RAMEnabled) { // RAM
      setDataDirOut();
      writeData(RAM[address - RAM_START]);
    } else if ((address >= ROM_CODE) && (address <= ROM_END) && ROMEnabled) { // ROM
      setDataDirOut();
      writeData(ROM[address - ROM_START]);
    } else {
      setDataDirIn();
    }
  } else { // WRITING
    setDataDirIn();
  }
}

void onCycleEnd() {
  // Read data at end of cycle (just before high to low transition)
  data = readData();

  // Read the control pins
  resb = digitalReadFast(RESB);
  nmib = digitalReadFast(NMIB);
  irqb = digitalReadFast(IRQB);
  rdy = digitalReadFast(RDY);

  // Always capture IO and RAM read / writes for debugging
  if ((address >= IO_START) && (address <= IO_END)) { // IO
    IO[address - IO_START] = data;
  } else if ((address >= RAM_START) && (address <= RAM_END)) { // RAM
    RAM[address - RAM_START] = data;
  }

  if (readWrite == LOW) { // WRITING
    // Handle IO post processing
    switch(address - IO_BANKS[IOBank]) {
      case DSP_DATA: {
        Serial.write(data);
      }
      case GPIO_DATA: {
        if (IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00000001) {
          digitalWriteFast(GPIO0, IO[(IO_BANKS[IOBank] + GPIO_DATA) - IO_START] & 0b00000001);
        }

        #ifdef DEVBOARD
        if ((IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00000010) >> 1) {
          digitalWriteFast(GPIO1, (IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00000010) >> 1);
        }
        if ((IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00000100) >> 2) {
          digitalWriteFast(GPIO2, (IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00000100) >> 2);
        }
        if ((IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00001000) >> 3) {
          digitalWriteFast(GPIO3, (IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00001000) >> 3);
        }
        #endif
      }
      case GPIO_CTRL: {
        pinMode(GPIO0, IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00000001);

        #ifdef DEVBOARD
        pinMode(GPIO1, (IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00000010) >> 1);
        pinMode(GPIO2, (IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00000100) >> 2);
        pinMode(GPIO3, (IO[(IO_BANKS[IOBank] + GPIO_CTRL) - IO_START] & 0b00001000) >> 3);
        #endif
      }
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
    case 'b':
    case 'B':
      toggleDebug();
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
    case 0x1B:
      toggleKeyboardCapture();
      break;
  }
}

void onKey(char key) {
  // Set the keyboard data register to key value and set data ready bit
  IO[(IO_BANKS[IOBank] + KBD_DATA) - IO_START] = (key | 0x80);

  // And set interrupt if keyboard interrupts are enabled
  if ((IO[(IO_BANKS[IOBank] + KBD_CTRL) - IO_START] & 0x01) != 0) {
    digitalWriteFast(IRQB, LOW);

    // Set bit 7 of KBD_STAT register to indicate keyboard interrupt asserted
    IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] |= 0b10000000;
  } 
}

void onKeyPress(int key) {
  if (key > 127) { 
    // If bit 7 is set (non-ASCII key) we should set arrow keys
    bool isArrowKey = false;

    switch (key) {
      case 0xD7: { // RIGHT
        IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] |= 0b00000001;
        isArrowKey = true;
        break;
      }
      case 0xD8: { // LEFT
        IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] |= 0b00000010;
        isArrowKey = true;
        break;
      }
      case 0xD9: { // DOWN  
        IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] |= 0b00000100;
        isArrowKey = true;
        break;
      }
      case 0xDA: { // UP
        IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] |= 0b00001000;
        isArrowKey = true;
        break;
      }
    }

    // And set interrupt if arrow key interrupts are enabled
    if (isArrowKey && (IO[(IO_BANKS[IOBank] + KBD_CTRL) - IO_START] & 0x02) != 0) {
      digitalWriteFast(IRQB, LOW);

      // Set bit 6 of KBD_STAT register to indicate arrow key interrupt asserted
      IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] |= 0b01000000;
    }
  } else {
    // Otherwise, we set the keyboard data register to key value and set data ready bit
    IO[(IO_BANKS[IOBank] + KBD_DATA) - IO_START] = (key | 0x80);

    // And set interrupt if keyboard interrupts are enabled
    if ((IO[(IO_BANKS[IOBank] + KBD_CTRL) - IO_START] & 0x01) != 0) {
      digitalWriteFast(IRQB, LOW);

      // Set bit 7 of KBD_STAT register to indicate keyboard interrupt asserted
      IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] |= 0b10000000;
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
        IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] &= 0b11111110;
        isArrowKey = true;
        break;
      }
      case KEYD_LEFT: { // LEFT
        IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] &= 0b11111101;
        isArrowKey = true;
        break;
      }
      case KEYD_DOWN: { // DOWN  
        IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] &= 0b11111011;
        isArrowKey = true;
        break;
      }
      case KEYD_UP: { // UP
        IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] &= 0b11110111;
        isArrowKey = true;
        break;
      }
    }

    // And set interrupt if arrow key interrupts are enabled
    if (isArrowKey && (IO[(IO_BANKS[IOBank] + KBD_CTRL) - IO_START] & 0x02) != 0) {
      digitalWriteFast(IRQB, LOW);

      // Set bit 6 of KBD_STAT register to indicate arrow key interrupt asserted
      IO[(IO_BANKS[IOBank] + KBD_STAT) - IO_START] |= 0b01000000;
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

void onMouseAvailable() {
  byte buttons = mouse.getButtons();
  byte x = 0;
  byte y = 0;
  byte wheel = 0;

  int8_t mouseX = mouse.getMouseX();
  int8_t mouseY = mouse.getMouseY();
  int8_t mouseWheel = mouse.getWheel();

  x = mouseX < 0 ? (abs(mouseX) | 0b10000000) : mouseX;                 // If negative, set bit 7 (Left = -X)
  y = mouseY < 0 ? (abs(mouseY) | 0b10000000) : mouseY;                 // If negative, set bit 7 (Up = -Y)
  wheel = mouseWheel < 0 ? (abs(mouseWheel) | 0b10000000) : mouseWheel; // If negative, set bit 7 (Up = -W)

  IO[(IO_BANKS[IOBank] + MOUSE_X) - IO_START] = x;
  IO[(IO_BANKS[IOBank] + MOUSE_Y) - IO_START] = y;
  IO[(IO_BANKS[IOBank] + MOUSE_W) - IO_START] = wheel;
  IO[(IO_BANKS[IOBank] + MOUSE_BTN) - IO_START] = buttons;

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

void onJoystickAvailable() {
  uint64_t axis_mask = joystick.axisMask();
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

    IO[(IO_BANKS[IOBank] + JOY_BTNL) - IO_START] = buttons & 0xFF;
    IO[(IO_BANKS[IOBank] + JOY_BTNH) - IO_START] = (buttons >> 8) & 0xFF;
    IO[(IO_BANKS[IOBank] + JOY_LAXL) - IO_START] = joystick.getAxis(0) & 0xFF;
    IO[(IO_BANKS[IOBank] + JOY_LAXH) - IO_START] = (joystick.getAxis(0) >> 8) & 0xFF;
    IO[(IO_BANKS[IOBank] + JOY_LAYL) - IO_START] = joystick.getAxis(1) & 0xFF;
    IO[(IO_BANKS[IOBank] + JOY_LAYH) - IO_START] = (joystick.getAxis(1) >> 8) & 0xFF;
    IO[(IO_BANKS[IOBank] + JOY_RAXL) - IO_START] = joystick.getAxis(2) & 0xFF;
    IO[(IO_BANKS[IOBank] + JOY_RAXH) - IO_START] = (joystick.getAxis(2) >> 8) & 0xFF;
    IO[(IO_BANKS[IOBank] + JOY_RAYL) - IO_START] = joystick.getAxis(5) & 0xFF;
    IO[(IO_BANKS[IOBank] + JOY_RAYH) - IO_START] = (joystick.getAxis(5) >> 8) & 0xFF;
    IO[(IO_BANKS[IOBank] + JOY_LTRL) - IO_START] = joystick.getAxis(3) & 0xFF;
    IO[(IO_BANKS[IOBank] + JOY_LTRH) - IO_START] = (joystick.getAxis(3) >> 8) & 0xFF;
    IO[(IO_BANKS[IOBank] + JOY_RTRL) - IO_START] = joystick.getAxis(4) & 0xFF;
    IO[(IO_BANKS[IOBank] + JOY_RTRH) - IO_START] = (joystick.getAxis(4) >> 8) & 0xFF;
  }

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
  Serial.print(VERSION);
  Serial.print(" | Platform: ");
  #if DEVBOARD
  Serial.println("Dev Board");
  #elif RETROSHIELD
  Serial.println("Retroshield");
  #elif RETROSHIELDADAPTER
  Serial.println("Retroshield Adapter");
  #endif
  Serial.println();
  Serial.println("---------------------------------");
  Serial.println("| Created by A.C. Wright © 2024 |");
  Serial.println("---------------------------------");
  Serial.println();
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
  Serial.print("Debug: ");
  Serial.println(formatDebugMode());
  Serial.print("Keyboard Capture: ");
  Serial.println(keyboardCaptureEnabled ? "Enabled" : "Disabled");
  Serial.print("Frequency: ");
  Serial.println(FREQS[freqIndex]);
  Serial.print("Date / Time: ");
  Serial.println(formatDateTime());
  Serial.println();
  Serial.println("Hit (ESC) to begin / end keyboard capture mode...");
  Serial.println();
  Serial.println("------------------------------------------------------------");
  Serial.println("| (R)un / Stop         | (S)tep           | Rese(T)        |");
  Serial.println("------------------------------------------------------------");
  Serial.println("| Toggle R(A)M         | Toggle (I)O      | Toggle R(O)M   |");
  Serial.println("------------------------------------------------------------");
  Serial.println("| (L)ist ROMs          | Change IO Ban(K) | Sna(P)shot     |");
  Serial.println("------------------------------------------------------------");
  Serial.println("| (+/-) Clk Frequency  | Toggle De(B)ug   | In(F)o / Lo(G) |");
  Serial.println("------------------------------------------------------------");
  Serial.println();
}

void log() {
  char output[128];

  sprintf(
    output, 
    "| %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c | %c%c%c%c%c%c%c%c | 0x%04X | 0x%02X | %c | RDY: %01X | BE: %01X | RESB: %01X | NMIB: %01X | IRQB: %01X | SOB: %01X | SYNC: %01X |", 
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
    sob,
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
  digitalWriteFast(PHI2, LOW);
  delay(100);
  digitalWriteFast(RESB, HIGH);

  Serial.println("Done");

  if (isCurrentlyRunning) {
    toggleRunStop();
  }
}

void step() {
  if (!isRunning) {
    isStepping = true;
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

  if (!isRunning && digitalReadFast(PHI2)) { 
    onCycleEnd();
    digitalWriteFast(PHI2, LOW);
  }

  Serial.println(isRunning ? "Running…" : "Stopped");
}

void toggleDebug() {
  switch (debugMode) {
    case DEBUG_NONE:
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

void toggleKeyboardCapture() {
  keyboardCaptureEnabled = !keyboardCaptureEnabled;

  Serial.print("Keyboard Capture: ");
  Serial.println(keyboardCaptureEnabled ? "Enabled" : "Disabled");
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

void readROMs() {
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
}

void listROMs() {
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
  time_t now = Teensy3Clock.get();

  IO[(IO_BANKS[IOBank] + RTC_SEC) - IO_START] = byte(second(now));
  IO[(IO_BANKS[IOBank] + RTC_MIN) - IO_START] = byte(minute(now));
  IO[(IO_BANKS[IOBank] + RTC_HR) - IO_START]  = byte(hour(now));
  IO[(IO_BANKS[IOBank] + RTC_DAY) - IO_START] = byte(day(now));
  IO[(IO_BANKS[IOBank] + RTC_MON) - IO_START] = byte(month(now));
  IO[(IO_BANKS[IOBank] + RTC_YR) - IO_START]  = byte(year(now) - 1970); // Offset from 1970 (0-99)

  return now;
}

//
// INITIALIZATION
//

void initRAM() {
  unsigned int i;
  
  for (i = 0; i < (RAM_END - RAM_START + 1); i++) {
    RAM[i] = 0x00; 
  }
}

void initIO() {
  unsigned int i;
  
  for (i = 0; i < (IO_END - IO_START + 1); i++) {
    IO[i] = 0x00; 
  }
}

void initROM() {
  unsigned int i;

  // Fill ROM with NOPs by default
  for (i = 0; i < (ROM_END - ROM_START + 1); i++) {
    ROM[i] = 0xEA;
  }
}

void initPins() {
  #if DEVBOARD
  pinMode(RESB, OUTPUT);
  pinMode(IRQB, OUTPUT);
  pinMode(NMIB, OUTPUT);
  pinMode(RDY, OUTPUT);
  pinMode(BE, OUTPUT);
  pinMode(PHI2, OUTPUT);
  
  pinMode(SYNC, INPUT);
  pinMode(RWB, INPUT);

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
  digitalWriteFast(PHI2, LOW);

  digitalWriteFast(OE1, HIGH);
  digitalWriteFast(OE2, HIGH);
  digitalWriteFast(OE3, HIGH);
  #elif RETROSHIELD
  pinMode(RESB, OUTPUT);
  pinMode(IRQB, OUTPUT);
  pinMode(NMIB, OUTPUT);
  pinMode(RDY, OUTPUT);
  pinMode(SOB, OUTPUT);
  pinMode(PHI2, OUTPUT);

  pinMode(RWB, INPUT);

  pinMode(INT_SWB, INPUT);
  pinMode(STEP_SWB, INPUT);
  pinMode(RS_SWB, INPUT);

  pinMode(GPIO0, INPUT);

  setAddrDirIn();
  setDataDirIn();

  digitalWriteFast(RESB, HIGH);
  digitalWriteFast(IRQB, HIGH);
  digitalWriteFast(NMIB, HIGH);
  digitalWriteFast(RDY, HIGH);
  digitalWriteFast(SOB, HIGH);
  digitalWriteFast(PHI2, LOW);
  #elif RETROSHIELDADAPTER
  pinMode(RESB, OUTPUT);
  pinMode(IRQB, OUTPUT);
  pinMode(NMIB, OUTPUT);
  pinMode(RDY, OUTPUT);
  pinMode(PHI2, OUTPUT);

  pinMode(SYNC, INPUT);
  pinMode(RWB, INPUT);

  pinMode(INT_SWB, INPUT);
  pinMode(STEP_SWB, INPUT);
  pinMode(RS_SWB, INPUT);

  pinMode(GPIO0, INPUT);

  setAddrDirIn();
  setDataDirIn();

  digitalWriteFast(RESB, HIGH);
  digitalWriteFast(IRQB, HIGH);
  digitalWriteFast(NMIB, HIGH);
  digitalWriteFast(RDY, HIGH);
  digitalWriteFast(PHI2, LOW);
  #endif
}

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

  readROMs();
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