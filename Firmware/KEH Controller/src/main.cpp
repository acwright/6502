#define CIRCULAR_BUFFER_INT_SAFE

#include <Arduino.h>
#include <CircularBuffer.hpp>

// ============================================================================
// Keyboard Matrix Mapping 
// ============================================================================
// Rows: PA0-PA7 (VIA Port A)
// Columns: PB0-PB7 (VIA Port B)
//
//      PB0    PB1    PB2    PB3    PB4    PB5    PB6    PB7
// PA0:  `      1      2      3      4      5      6      7
// PA1:  8      9      0      -      =      BS     ESC    TAB
// PA2:  q      w      e      r      t      y      u      i
// PA3:  o      p      [      ]      \      INS    CAPS   a
// PA4:  s      d      f      g      h      j      k      l
// PA5:  ;      '      ENTER  DEL    SHIFT  z      x      c
// PA6:  v      b      n      m      ,      .      /      UP
// PA7:  CTRL   GUI    ALT    SPACE  FN     LEFT   DOWN   RIGHT
//
// Notes:
// - BS = Backspace (0x08), ESC = Escape (0x1B), TAB (0x09)
// - INS = Insert (0x1A), DEL = Delete (0x7F), ENTER (0x0D)
// - Arrow keys: UP (0x1E), LEFT (0x1C), DOWN (0x1F), RIGHT (0x1D)
// - Function keys F1-F10 are mapped as FN + number keys 1-0
// ============================================================================

// ATMega1284 PORT A / 6522 VIA PORT A
#define VIA_PA0 24
#define VIA_PA1 25
#define VIA_PA2 26
#define VIA_PA3 27
#define VIA_PA4 28
#define VIA_PA5 29
#define VIA_PA6 30
#define VIA_PA7 31

// ATMega1284 PORT B / 6522 VIA PORT B
#define VIA_PB0 0
#define VIA_PB1 1
#define VIA_PB2 2
#define VIA_PB3 3
#define VIA_PB4 4
#define VIA_PB5 5
#define VIA_PB6 6
#define VIA_PB7 7

#define VIA_CA1 8
#define VIA_CA2 9
#define PS2CLK  10
#define PS2DATA 11
#define VIA_CB1 12
#define VIA_CB2 13

#define BUFFER_SIZE 16

CircularBuffer<uint8_t, BUFFER_SIZE> ps2Buffer;  // Buffer for PS/2 keyboard data
CircularBuffer<uint8_t, BUFFER_SIZE> matrixBuffer;  // Buffer for matrix keyboard data

// PS/2 keyboard state
bool ps2Enabled = false;
bool altPressed = false;
bool shiftPressed = false;
bool ctrlPressed = false;
bool capsLockOn = false;
bool breakCodeNext = false;
bool extendedCodeNext = false;

// Matrix keyboard state
bool matrixEnabled = false;
bool matrixShiftPressed = false;
bool matrixCtrlPressed = false;
bool matrixAltPressed = false;
bool matrixFnPressed = false;
bool matrixCapsLockOn = false;

// Matrix key debouncing
uint8_t matrixKeyState[8][8] = {0};  // Current state of each key
uint8_t matrixKeyLast[8][8] = {0};   // Last state of each key
unsigned long matrixLastScan = 0;
#define MATRIX_SCAN_INTERVAL 10  // Scan every 10ms
#define MATRIX_DEBOUNCE_COUNT 2  // Key must be stable for 2 scans

void onInterrupt();
void enablePS2();
void disablePS2();
void enableMatrix();
void disableMatrix();
void ps2ToAscii(uint8_t scancode);
void scanMatrix();
uint8_t mapMatrixToAscii(uint8_t row, uint8_t col);

// ============================================================================
// Keyboard Matrix to ASCII Mapping Table
// ============================================================================
// Maps each matrix position [row][col] to its base ASCII code (no modifiers)
// Special values: 0xFF = no key, 0xFE = modifier key (handled separately)
const uint8_t matrixMap[8][8] PROGMEM = {
  // PA0/Row 0: PB0    PB1    PB2    PB3    PB4    PB5    PB6    PB7
  {             0x60,  0x31,  0x32,  0x33,  0x34,  0x35,  0x36,  0x37 },  // ` 1 2 3 4 5 6 7
  // PA1/Row 1
  {             0x38,  0x39,  0x30,  0x2D,  0x3D,  0x08,  0x1B,  0x09 },  // 8 9 0 - = BS ESC TAB
  // PA2/Row 2
  {             0x71,  0x77,  0x65,  0x72,  0x74,  0x79,  0x75,  0x69 },  // q w e r t y u i
  // PA3/Row 3
  {             0x6F,  0x70,  0x5B,  0x5D,  0x5C,  0x1A,  0xFE,  0x61 },  // o p [ ] \ INS CAPS a
  // PA4/Row 4
  {             0x73,  0x64,  0x66,  0x67,  0x68,  0x6A,  0x6B,  0x6C },  // s d f g h j k l
  // PA5/Row 5
  {             0x3B,  0x27,  0x0D,  0x7F,  0xFE,  0x7A,  0x78,  0x63 },  // ; ' ENTER DEL SHIFT z x c
  // PA6/Row 6
  {             0x76,  0x62,  0x6E,  0x6D,  0x2C,  0x2E,  0x2F,  0x1E },  // v b n m , . / UP
  // PA7/Row 7
  {             0xFE,  0x80,  0xFE,  0x20,  0xFE,  0x1C,  0x1F,  0x1D }   // CTRL MENU ALT SPACE FN LEFT DOWN RIGHT
};

void setup() {
  pinMode(VIA_CA1, OUTPUT);
  pinMode(VIA_CA2, INPUT_PULLUP);
  pinMode(VIA_CB1, OUTPUT);
  pinMode(VIA_CB2, INPUT_PULLUP);
  pinMode(PS2CLK, INPUT_PULLUP);
  pinMode(PS2DATA, INPUT_PULLUP);

  digitalWrite(VIA_CA1, HIGH);
  digitalWrite(VIA_CB1, HIGH);
  
  attachInterrupt(digitalPinToInterrupt(PS2CLK), onInterrupt, FALLING);
}

void loop() {
  // Check PS/2 enable/disable (CA2)
  int ps2EnableState = digitalRead(VIA_CA2);
  if (ps2EnableState == LOW && !ps2Enabled) {
    enablePS2();
  } else if (ps2EnableState == HIGH && ps2Enabled) {
    disablePS2();
  }
  
  // Check matrix enable/disable (CB2)
  int matrixEnableState = digitalRead(VIA_CB2);
  if (matrixEnableState == LOW && !matrixEnabled) {
    enableMatrix();
  } else if (matrixEnableState == HIGH && matrixEnabled) {
    disableMatrix();
  }
  
  // Scan matrix keyboard if enabled
  if (matrixEnabled) {
    scanMatrix();
  }
  
  // Handle PS/2 keyboard output on Port A
  if (ps2Enabled && !ps2Buffer.isEmpty()) {
    uint8_t ascii = ps2Buffer.shift();
    
    // Write ASCII value to PORT A
    digitalWrite(VIA_PA0, (ascii >> 0) & 1);
    digitalWrite(VIA_PA1, (ascii >> 1) & 1);
    digitalWrite(VIA_PA2, (ascii >> 2) & 1);
    digitalWrite(VIA_PA3, (ascii >> 3) & 1);
    digitalWrite(VIA_PA4, (ascii >> 4) & 1);
    digitalWrite(VIA_PA5, (ascii >> 5) & 1);
    digitalWrite(VIA_PA6, (ascii >> 6) & 1);
    digitalWrite(VIA_PA7, (ascii >> 7) & 1);
    
    digitalWrite(VIA_CA1, LOW); // Signal that data is ready
    delayMicroseconds(5);
    digitalWrite(VIA_CA1, HIGH);
    delayMicroseconds(100); // Wait before next character
  }
  
  // Handle matrix keyboard output on Port B
  if (matrixEnabled && !matrixBuffer.isEmpty()) {
    uint8_t ascii = matrixBuffer.shift();
    
    // Write ASCII value to PORT B
    digitalWrite(VIA_PB0, (ascii >> 0) & 1);
    digitalWrite(VIA_PB1, (ascii >> 1) & 1);
    digitalWrite(VIA_PB2, (ascii >> 2) & 1);
    digitalWrite(VIA_PB3, (ascii >> 3) & 1);
    digitalWrite(VIA_PB4, (ascii >> 4) & 1);
    digitalWrite(VIA_PB5, (ascii >> 5) & 1);
    digitalWrite(VIA_PB6, (ascii >> 6) & 1);
    digitalWrite(VIA_PB7, (ascii >> 7) & 1);
    
    digitalWrite(VIA_CB1, LOW); // Signal that data is ready
    delayMicroseconds(5);
    digitalWrite(VIA_CB1, HIGH);
    delayMicroseconds(100); // Wait before next character
  }
}

void enablePS2() {
  ps2Enabled = true;
  
  // Set Port A as output for PS/2 data
  pinMode(VIA_PA0, OUTPUT);
  pinMode(VIA_PA1, OUTPUT);
  pinMode(VIA_PA2, OUTPUT);
  pinMode(VIA_PA3, OUTPUT);
  pinMode(VIA_PA4, OUTPUT);
  pinMode(VIA_PA5, OUTPUT);
  pinMode(VIA_PA6, OUTPUT);
  pinMode(VIA_PA7, OUTPUT);
}

void disablePS2() {
  ps2Enabled = false;
  
  // Set Port A as input if matrix is also disabled
  if (!matrixEnabled) {
    pinMode(VIA_PA0, INPUT);
    pinMode(VIA_PA1, INPUT);
    pinMode(VIA_PA2, INPUT);
    pinMode(VIA_PA3, INPUT);
    pinMode(VIA_PA4, INPUT);
    pinMode(VIA_PA5, INPUT);
    pinMode(VIA_PA6, INPUT);
    pinMode(VIA_PA7, INPUT);
  }
}

void enableMatrix() {
  matrixEnabled = true;
  
  // Port A will be used for row scanning (will toggle between input/output)
  // Port B will be used for column reading and data output
  pinMode(VIA_PB0, OUTPUT);
  pinMode(VIA_PB1, OUTPUT);
  pinMode(VIA_PB2, OUTPUT);
  pinMode(VIA_PB3, OUTPUT);
  pinMode(VIA_PB4, OUTPUT);
  pinMode(VIA_PB5, OUTPUT);
  pinMode(VIA_PB6, OUTPUT);
  pinMode(VIA_PB7, OUTPUT);
  
  // Initialize Port A for scanning if PS/2 is disabled
  if (!ps2Enabled) {
    pinMode(VIA_PA0, OUTPUT);
    pinMode(VIA_PA1, OUTPUT);
    pinMode(VIA_PA2, OUTPUT);
    pinMode(VIA_PA3, OUTPUT);
    pinMode(VIA_PA4, OUTPUT);
    pinMode(VIA_PA5, OUTPUT);
    pinMode(VIA_PA6, OUTPUT);
    pinMode(VIA_PA7, OUTPUT);
  }
}

void disableMatrix() {
  matrixEnabled = false;
  
  // Set Port B as input
  pinMode(VIA_PB0, INPUT);
  pinMode(VIA_PB1, INPUT);
  pinMode(VIA_PB2, INPUT);
  pinMode(VIA_PB3, INPUT);
  pinMode(VIA_PB4, INPUT);
  pinMode(VIA_PB5, INPUT);
  pinMode(VIA_PB6, INPUT);
  pinMode(VIA_PB7, INPUT);
  
  // Set Port A as input if PS/2 is also disabled
  if (!ps2Enabled) {
    pinMode(VIA_PA0, INPUT);
    pinMode(VIA_PA1, INPUT);
    pinMode(VIA_PA2, INPUT);
    pinMode(VIA_PA3, INPUT);
    pinMode(VIA_PA4, INPUT);
    pinMode(VIA_PA5, INPUT);
    pinMode(VIA_PA6, INPUT);
    pinMode(VIA_PA7, INPUT);
  }
}

// ============================================================================
// Matrix Keyboard Scanning
// ============================================================================

// Scan the keyboard matrix for key presses
void scanMatrix() {
  unsigned long currentTime = millis();
  
  // Throttle scanning to MATRIX_SCAN_INTERVAL
  if (currentTime - matrixLastScan < MATRIX_SCAN_INTERVAL) {
    return;
  }
  matrixLastScan = currentTime;
  
  // Temporarily save current modifier states
  bool newShift = false;
  bool newCtrl = false;
  bool newAlt = false;
  bool newFn = false;
  bool capsToggled = false;
  
  // Scan each row
  for (uint8_t row = 0; row < 8; row++) {
    // Set current row LOW (active), all others HIGH
    // But only if PS/2 is not using Port A for output
    uint8_t portAPins[] = {VIA_PA0, VIA_PA1, VIA_PA2, VIA_PA3, VIA_PA4, VIA_PA5, VIA_PA6, VIA_PA7};
    
    // Configure Port A for scanning
    if (!ps2Enabled) {
      for (uint8_t i = 0; i < 8; i++) {
        pinMode(portAPins[i], OUTPUT);
        digitalWrite(portAPins[i], (i == row) ? LOW : HIGH);
      }
    } else {
      // PS/2 is using Port A, skip scanning (shouldn't happen normally)
      return;
    }
    
    // Small delay to let signals stabilize
    delayMicroseconds(10);
    
    // Read all columns
    uint8_t portBPins[] = {VIA_PB0, VIA_PB1, VIA_PB2, VIA_PB3, VIA_PB4, VIA_PB5, VIA_PB6, VIA_PB7};
    
    // Configure Port B pins as inputs with pullups for reading
    for (uint8_t col = 0; col < 8; col++) {
      pinMode(portBPins[col], INPUT_PULLUP);
    }
    
    delayMicroseconds(10);
    
    for (uint8_t col = 0; col < 8; col++) {
      // Read column (LOW = pressed, HIGH = not pressed due to pullup)
      bool pressed = (digitalRead(portBPins[col]) == LOW);
      
      // Debounce logic
      if (pressed) {
        if (matrixKeyState[row][col] < MATRIX_DEBOUNCE_COUNT) {
          matrixKeyState[row][col]++;
        }
      } else {
        if (matrixKeyState[row][col] > 0) {
          matrixKeyState[row][col]--;
        }
      }
      
      // Key is confirmed pressed if state >= MATRIX_DEBOUNCE_COUNT
      bool keyPressed = (matrixKeyState[row][col] >= MATRIX_DEBOUNCE_COUNT);
      bool keyWasPressed = matrixKeyLast[row][col];
      
      // Check for modifier keys first
      if (keyPressed) {
        // SHIFT is at VIA_PA5/VIA_PB4
        if (row == 5 && col == 4) {
          newShift = true;
        }
        // CTRL is at VIA_PA7/VIA_PB0
        else if (row == 7 && col == 0) {
          newCtrl = true;
        }
        // ALT is at VIA_PA7/VIA_PB2
        else if (row == 7 && col == 2) {
          newAlt = true;
        }
        // FN is at VIA_PA7/VIA_PB4
        else if (row == 7 && col == 4) {
          newFn = true;
        }
        // CAPS is at VIA_PA3/VIA_PB6 - toggle on key press (not repeat)
        else if (row == 3 && col == 6 && !keyWasPressed) {
          capsToggled = true;
        }
      }
      
      // Detect key press event (transition from not pressed to pressed)
      if (keyPressed && !keyWasPressed) {
        uint8_t ascii = mapMatrixToAscii(row, col);
        if (ascii != 0xFF && ascii != 0xFE) {  // 0xFF = no key, 0xFE = modifier
          matrixBuffer.push(ascii);
        }
      }
      
      matrixKeyLast[row][col] = keyPressed;
    }
    
    // Restore Port B to output mode for data transmission
    for (uint8_t col = 0; col < 8; col++) {
      pinMode(portBPins[col], OUTPUT);
    }
  }
  
  // Update modifier states
  matrixShiftPressed = newShift;
  matrixCtrlPressed = newCtrl;
  matrixAltPressed = newAlt;
  matrixFnPressed = newFn;
  
  // Toggle caps lock if CAPS was pressed
  if (capsToggled) {
    matrixCapsLockOn = !matrixCapsLockOn;
  }
}

// Map matrix position to ASCII code based on modifiers
uint8_t mapMatrixToAscii(uint8_t row, uint8_t col) {
  // Get base key code from PROGMEM
  uint8_t baseKey = pgm_read_byte(&matrixMap[row][col]);
  
  // Handle special keys
  if (baseKey == 0xFF) return 0xFF;  // No key
  if (baseKey == 0xFE) return 0xFE;  // Modifier key
  
  // Handle Fn combinations (0x81-0x9F range)
  if (matrixFnPressed) {
    // F1-F10: Fn + number keys 1-0
    if (baseKey == 0x31) return matrixAltPressed ? 0x91 : 0x81;  // Fn+1 = F1
    if (baseKey == 0x32) return matrixAltPressed ? 0x92 : 0x82;  // Fn+2 = F2
    if (baseKey == 0x33) return matrixAltPressed ? 0x93 : 0x83;  // Fn+3 = F3
    if (baseKey == 0x34) return matrixAltPressed ? 0x94 : 0x84;  // Fn+4 = F4
    if (baseKey == 0x35) return matrixAltPressed ? 0x95 : 0x85;  // Fn+5 = F5
    if (baseKey == 0x36) return matrixAltPressed ? 0x96 : 0x86;  // Fn+6 = F6
    if (baseKey == 0x37) return matrixAltPressed ? 0x97 : 0x87;  // Fn+7 = F7
    if (baseKey == 0x38) return matrixAltPressed ? 0x98 : 0x88;  // Fn+8 = F8
    if (baseKey == 0x39) return matrixAltPressed ? 0x99 : 0x89;  // Fn+9 = F9
    if (baseKey == 0x30) return matrixAltPressed ? 0x9A : 0x8A;  // Fn+0 = F10
    
    // F11-F15: Alt+Fn + K, L, M, N, O (from table)
    if (matrixAltPressed) {
      if (baseKey == 0x6B) return 0x8B;  // Alt+Fn+K = F11
      if (baseKey == 0x6C) return 0x8C;  // Alt+Fn+L = F12
      if (baseKey == 0x6D) return 0x8D;  // Alt+Fn+M = F13
      if (baseKey == 0x6E) return 0x8E;  // Alt+Fn+N = F14
      if (baseKey == 0x6F) return 0x8F;  // Alt+Fn+O = F15
    }
    
    // Special Fn combinations with arrow keys
    if (baseKey == 0x1B) return 0x9B;  // Fn+ESC = F11
    if (baseKey == 0x1C) return 0x9C;  // Fn+LEFT = F12
    if (baseKey == 0x1D) return 0x9D;  // Fn+RIGHT = F13
    if (baseKey == 0x1E) return 0x9E;  // Fn+UP = F14
    if (baseKey == 0x1F) return 0x9F;  // Fn+DOWN = F15
  }
  
  // Handle Ctrl combinations (0x00-0x1F control codes)
  if (matrixCtrlPressed) {
    if (baseKey == 0x32) return 0x00;  // Ctrl+2 = NUL
    if (baseKey >= 0x61 && baseKey <= 0x7A) {  // Ctrl+a-z
      return baseKey - 0x60;  // Convert to 0x01-0x1A
    }
    if (baseKey >= 0x41 && baseKey <= 0x5A) {  // Ctrl+A-Z (same as a-z)
      return baseKey - 0x40;  // Convert to 0x01-0x1A
    }
    if (baseKey == 0x5B) return 0x1B;  // Ctrl+[ = ESC
    if (baseKey == 0x5C) return 0x1C;  // Ctrl+\ = FS
    if (baseKey == 0x5D) return 0x1D;  // Ctrl+] = GS
    if (baseKey == 0x36) return 0x1E;  // Ctrl+6 = RS
    if (baseKey == 0x2D) return 0x1F;  // Ctrl+- = US
  }
  
  // Handle Shift combinations
  if (matrixShiftPressed) {
    // Numbers to symbols
    if (baseKey == 0x31) return 0x21;  // 1 -> !
    if (baseKey == 0x32) return 0x40;  // 2 -> @
    if (baseKey == 0x33) return 0x23;  // 3 -> #
    if (baseKey == 0x34) return 0x24;  // 4 -> $
    if (baseKey == 0x35) return 0x25;  // 5 -> %
    if (baseKey == 0x36) return 0x5E;  // 6 -> ^
    if (baseKey == 0x37) return 0x26;  // 7 -> &
    if (baseKey == 0x38) return 0x2A;  // 8 -> *
    if (baseKey == 0x39) return 0x28;  // 9 -> (
    if (baseKey == 0x30) return 0x29;  // 0 -> )
    
    // Other symbols
    if (baseKey == 0x2D) return 0x5F;  // - -> _
    if (baseKey == 0x3D) return 0x2B;  // = -> +
    if (baseKey == 0x5B) return 0x7B;  // [ -> {
    if (baseKey == 0x5D) return 0x7D;  // ] -> }
    if (baseKey == 0x5C) return 0x7C;  // \ -> |
    if (baseKey == 0x3B) return 0x3A;  // ; -> :
    if (baseKey == 0x27) return 0x22;  // ' -> "
    if (baseKey == 0x60) return 0x7E;  // ` -> ~
    if (baseKey == 0x2C) return 0x3C;  // , -> <
    if (baseKey == 0x2E) return 0x3E;  // . -> >
    if (baseKey == 0x2F) return 0x3F;  // / -> ?
    
    // Letters: lowercase to uppercase
    if (baseKey >= 0x61 && baseKey <= 0x7A) {
      if (matrixCapsLockOn) {
        return baseKey;  // Shift+Caps = lowercase
      } else {
        return baseKey - 0x20;  // Shift = uppercase
      }
    }
  }
  
  // Handle Alt combinations (0xA0-0xFF range)
  if (matrixAltPressed && !matrixFnPressed) {
    // Alt+Shift combinations (0xA1-0xDF range)
    if (matrixShiftPressed) {
      if (baseKey == 0x31) return 0xA1;  // Alt+Shift+1
      if (baseKey == 0x27) return 0xA2;  // Alt+Shift+'
      if (baseKey == 0x33) return 0xA3;  // Alt+Shift+3
      if (baseKey == 0x34) return 0xA4;  // Alt+Shift+4
      if (baseKey == 0x35) return 0xA5;  // Alt+Shift+5
      if (baseKey == 0x37) return 0xA6;  // Alt+Shift+7
      if (baseKey == 0x39) return 0xA8;  // Alt+Shift+9
      if (baseKey == 0x30) return 0xA9;  // Alt+Shift+0
      if (baseKey == 0x38) return 0xAA;  // Alt+Shift+8
      if (baseKey == 0x3D) return 0xAB;  // Alt+Shift+=
      if (baseKey == 0x3B) return 0xBA;  // Alt+Shift+;
      if (baseKey == 0x2C) return 0xBC;  // Alt+Shift+,
      if (baseKey == 0x2E) return 0xBE;  // Alt+Shift+.
      if (baseKey == 0x2F) return 0xBF;  // Alt+Shift+/
      if (baseKey == 0x32) return 0xC0;  // Alt+Shift+2
      if (baseKey == 0x36) return 0xDE;  // Alt+Shift+6
      if (baseKey == 0x2D) return 0xDF;  // Alt+Shift+-
      
      // Alt+Shift+letters (0xC1-0xDA)
      if (baseKey >= 0x61 && baseKey <= 0x7A) {
        return 0xC1 + (baseKey - 0x61);
      }
      
      // Alt+Shift+brackets (0xFB-0xFE)
      if (baseKey == 0x5B) return 0xFB;  // Alt+Shift+[
      if (baseKey == 0x5C) return 0xFC;  // Alt+Shift+backslash
      if (baseKey == 0x5D) return 0xFD;  // Alt+Shift+]
      if (baseKey == 0x60) return 0xFE;  // Alt+Shift+`
    } else {
      // Alt only (0xA0-0xFF range, no shift)
      if (baseKey == 0x20) return 0xA0;  // Alt+SPACE
      if (baseKey == 0x27) return 0xA7;  // Alt+'
      if (baseKey == 0x2C) return 0xAC;  // Alt+,
      if (baseKey == 0x2D) return 0xAD;  // Alt+-
      if (baseKey == 0x2E) return 0xAE;  // Alt+.
      if (baseKey == 0x2F) return 0xAF;  // Alt+/
      if (baseKey >= 0x30 && baseKey <= 0x39) {  // Alt+0-9
        return 0xB0 + (baseKey - 0x30);
      }
      if (baseKey == 0x3B) return 0xBB;  // Alt+;
      if (baseKey == 0x3D) return 0xBD;  // Alt+=
      if (baseKey == 0x60) return 0xE0;  // Alt+`
      
      // Alt+letters (0xE1-0xFA)
      if (baseKey >= 0x61 && baseKey <= 0x7A) {
        return 0xE1 + (baseKey - 0x61);
      }
      
      // Alt+brackets (0xDB-0xDD)
      if (baseKey == 0x5B) return 0xDB;  // Alt+[
      if (baseKey == 0x5C) return 0xDC;  // Alt+backslash
      if (baseKey == 0x5D) return 0xDD;  // Alt+]
      
      if (baseKey == 0x7F) return 0xFF;  // Alt+DEL
      if (baseKey == 0x80) return 0x90;  // Alt+MENU
    }
  }
  
  // Handle Caps Lock for letters (no other modifiers)
  if (matrixCapsLockOn && !matrixShiftPressed && !matrixCtrlPressed && !matrixAltPressed) {
    if (baseKey >= 0x61 && baseKey <= 0x7A) {
      return baseKey - 0x20;  // Convert to uppercase
    }
  }
  
  // Return base key if no modifiers matched
  return baseKey;
}

void onInterrupt() {
  static uint8_t bitcount = 0;
  static uint8_t incoming;
  static uint8_t parity;
  static uint32_t prev_ms = 0;
  uint32_t now_ms;
  uint8_t val;

  val = digitalRead(PS2DATA);
  now_ms = millis();

  if (now_ms - prev_ms > 250) { 
    bitcount = 0;
  }

  prev_ms = now_ms;
  bitcount++;

  switch (bitcount) {
    case 1:  // Start bit
      incoming = 0;
      parity = 0;
      break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:                              // Data bits
      parity += val;                     // Count number of 1 bits
      incoming >>= 1;                    // Right shift one place for next bit
      incoming |= (val) ? 0x80 : 0;      // OR in MSbit
      break;
    case 10:                             // Parity check (PS/2 uses odd parity)
      parity &= 1;                       // Get LSB: 1=odd count, 0=even count
      if (parity == val)                 // For odd parity: bit should be opposite of data parity
        parity = 0xFD;                   // Mark invalid to discard byte
      break;
    case 11: // Stop bit
      if (parity < 0xFD) {               // Good so save byte in buffer, otherwise discard
        ps2ToAscii(incoming);            // Convert to ASCII if valid scancode (if full, oldest data lost)
      }
      bitcount = 0;
      break;
    default:
      bitcount = 0;                      // Shouldn't be here so reset state machine
      break;
  }
}

// PS/2 to ASCII conversion function
void ps2ToAscii(uint8_t scancode) {
  // Handle special code prefix
  if (scancode == 0xE0) {
    extendedCodeNext = true;
    return;
  }
  if (scancode == 0xE1) {
    return;
  }

  // Handle break code prefix
  if (scancode == 0xF0) {
    breakCodeNext = true;
    return;
  }
  
  // Handle break codes (key releases)
  if (breakCodeNext) {
    breakCodeNext = false;
    
    // Handle extended keys in break codes
    if (extendedCodeNext) {
      extendedCodeNext = false;
      switch (scancode) {
        case 0x11: // Right Alt
          altPressed = false;
          break;
        case 0x14: // Right Control
          ctrlPressed = false;
          break;
      }
      return;
    }
    
    switch (scancode) {
      case 0x11: // Alt
        altPressed = false;
        break;
      case 0x12: // Left Shift
      case 0x59: // Right Shift
        shiftPressed = false;
        break;
      case 0x14: // Control
        ctrlPressed = false;
        break;
    }
    return; // Don't generate ASCII for key releases
  }
  
  // Handle make codes (key presses)
  switch (scancode) {
    case 0x05: // F1
      ps2Buffer.push(altPressed ? 0x91 : 0x81);
      return;
    case 0x06: // F2
      ps2Buffer.push(altPressed ? 0x92 : 0x82);
      return;
    case 0x04: // F3
      ps2Buffer.push(altPressed ? 0x93 : 0x83);
      return;
    case 0x0C: // F4
      ps2Buffer.push(altPressed ? 0x94 : 0x84);
      return;
    case 0x03: // F5
      ps2Buffer.push(altPressed ? 0x95 : 0x85);
      return;
    case 0x0B: // F6
      ps2Buffer.push(altPressed ? 0x96 : 0x86);
      return;
    case 0x83: // F7
      ps2Buffer.push(altPressed ? 0x97 : 0x87);
      return;
    case 0x0A: // F8
      ps2Buffer.push(altPressed ? 0x98 : 0x88);
      return;
    case 0x01: // F9
      ps2Buffer.push(altPressed ? 0x99 : 0x89);
      return;
    case 0x09: // F10
      ps2Buffer.push(altPressed ? 0x9A : 0x8A);
      return;
    case 0x78: // F11
      ps2Buffer.push(altPressed ? 0x9B : 0x8B);
      return;
    case 0x07: // F12
      ps2Buffer.push(altPressed ? 0x9C : 0x8C);
      return;
    case 0x0D: // Tab
      ps2Buffer.push(0x09);
      return;
    case 0x11: // Alt
      if (extendedCodeNext) {
        extendedCodeNext = false;
        altPressed = true; // Right Alt (same as left Alt)
      } else {
        altPressed = true;
      }
      return;
    case 0x12: // Left Shift
    case 0x59: // Right Shift
      shiftPressed = true;
      return;
    case 0x14: // Control
      if (extendedCodeNext) {
        extendedCodeNext = false;
        ctrlPressed = true; // Right Control (same as left Control)
      } else {
        ctrlPressed = true;
      }
      return;
    case 0x1F: // Windows / Menu (Left)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        ps2Buffer.push(altPressed ? 0x90 : 0x80); // Right Windows / Menu
      } else {
        ps2Buffer.push(altPressed ? 0x90 : 0x80); // Left Windows / Menu
      }
      return;
    case 0x27: // Windows / Menu (additional scancode)
    case 0x2F:
      ps2Buffer.push(altPressed ? 0x90 : 0x80);
      return;
    case 0x58: // Caps Lock
      capsLockOn = !capsLockOn;
      return;
    case 0x5A: // Enter
      if (extendedCodeNext) {
        extendedCodeNext = false;
        ps2Buffer.push(0x0D); // Keypad Enter
      } else {
        ps2Buffer.push(0x0D); // Main Enter
      }
      return;
    case 0x66: // Backspace
      ps2Buffer.push(0x08);
      return;
    case 0x69: // End (E0 69)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        // End key - not in keyboard matrix, ignore
      }
      return;
    case 0x6B: // Left Arrow (E0 6B)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        ps2Buffer.push(0x1C);
      }
      return;
    case 0x6C: // Home (E0 6C)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        // Home key - not in keyboard matrix, ignore
      }
      return;
    case 0x70: // Insert (E0 70)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        ps2Buffer.push(0x1A);
      }
      return;
    case 0x71: // Delete (E0 71)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        ps2Buffer.push(0x7F);
      }
      return;
    case 0x72: // Down Arrow (E0 72)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        ps2Buffer.push(0x1F);
      }
      return;
    case 0x74: // Right Arrow (E0 74)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        ps2Buffer.push(0x1D);
      }
      return;
    case 0x75: // Up Arrow (E0 75)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        ps2Buffer.push(0x1E);
      }
      return;
    case 0x76: // Escape
      ps2Buffer.push(0x1B);
      return;
    case 0x73: // 5
    case 0x77: // Num Lock
    case 0x79: // +
    case 0x7A: // Page Down
    case 0x7B: // "/"
    case 0x7C: // *
    case 0x7D: // Page Up
    case 0x7E: // Scroll Lock
      return;
  }

  // If we get here with an extended code, clear the flag and ignore the scancode
  if (extendedCodeNext) {
    extendedCodeNext = false;
    return;
  }

  // Handle control codes
  if (ctrlPressed) {
    switch (scancode) {
      case 0x1E: // 2 or @
        ps2Buffer.push(0x00); // NULL
        return;
      case 0x1C: // A
        ps2Buffer.push(0x01); // SOH
        return;
      case 0x32: // B
        ps2Buffer.push(0x02); // STX
        return;
      case 0x21: // C
        ps2Buffer.push(0x03); // ETX
        return;
      case 0x23: // D
        ps2Buffer.push(0x04); // EOT
        return;
      case 0x24: // E
        ps2Buffer.push(0x05); // ENQ
        return;
      case 0x2B: // F
        ps2Buffer.push(0x06); // ACK
        return;
      case 0x34: // G
        ps2Buffer.push(0x07); // BEL
        return;
      case 0x33: // H
        ps2Buffer.push(0x08); // BS
        return;
      case 0x43: // I
        ps2Buffer.push(0x09); // HT
        return;
      case 0x3B: // J
        ps2Buffer.push(0x0A); // LF
        return;
      case 0x42: // K
        ps2Buffer.push(0x0B); // VT
        return;
      case 0x4B: // L
        ps2Buffer.push(0x0C); // FF
        return;
      case 0x3A: // M
        ps2Buffer.push(0x0D); // CR
        return;
      case 0x31: // N
        ps2Buffer.push(0x0E); // SO
        return;
      case 0x44: // O
        ps2Buffer.push(0x0F); // SI
        return;
      case 0x4D: // P
        ps2Buffer.push(0x10); // DLE
        return;
      case 0x15: // Q
        ps2Buffer.push(0x11); // DC1
        return;
      case 0x2D: // R
        ps2Buffer.push(0x12); // DC2
        return;
      case 0x1B: // S
        ps2Buffer.push(0x13); // DC3
        return;
      case 0x2C: // T
        ps2Buffer.push(0x14); // DC4
        return;
      case 0x3C: // U
        ps2Buffer.push(0x15); // NAK
        return;
      case 0x2A: // V
        ps2Buffer.push(0x16); // SYN
        return;
      case 0x1D: // W
        ps2Buffer.push(0x17); // ETB
        return;
      case 0x22: // X
        ps2Buffer.push(0x18); // CAN
        return;
      case 0x35: // Y
        ps2Buffer.push(0x19); // EM
        return;
      case 0x1A: // Z
        ps2Buffer.push(0x1A); // SUB
        return;
      case 0x54: // [
        ps2Buffer.push(0x1B); // ESC
        return;
      case 0x5D: // "\"
        ps2Buffer.push(0x1C); // FS (LEFT)
        return;
      case 0x5B: // ]
        ps2Buffer.push(0x1D); // GS (RIGHT)
        return;
      case 0x36: // 6 or ^
        ps2Buffer.push(0x1E); // RS (UP)
        return;
      case 0x4E: // - or _
        ps2Buffer.push(0x1F); // US (DOWN)
        return;
    }
    return;
  }
  
  // Regular character mapping
  static const char unshifted[] = {
    0,   0,   0,   0,   0,   0,   0,   0,     // 0x00-0x07
    0,   0,   0,   0,   0,   0, '`',   0,     // 0x08-0x0F
    0,   0,   0,   0,   0, 'q', '1',   0,     // 0x10-0x17
    0,   0, 'z', 's', 'a', 'w', '2',   0,     // 0x18-0x1F
    0, 'c', 'x', 'd', 'e', '4', '3',   0,     // 0x20-0x27
    0, ' ', 'v', 'f', 't', 'r', '5',   0,     // 0x28-0x2F
    0, 'n', 'b', 'h', 'g', 'y', '6',   0,     // 0x30-0x37
    0,   0, 'm', 'j', 'u', '7', '8',   0,     // 0x38-0x3F
    0, ',', 'k', 'i', 'o', '0', '9',   0,     // 0x40-0x47
    0, '.', '/', 'l', ';', 'p', '-',   0,     // 0x48-0x4F
    0,   0,'\'',   0, '[', '=',   0,   0,     // 0x50-0x57
    0,   0,   0, ']',   0,'\\',   0,   0,     // 0x58-0x5F
  };
  
  static const char shifted[] = {
    0,   0,   0,   0,   0,   0,   0,   0,     // 0x00-0x07
    0,   0,   0,   0,   0,   0, '~',   0,     // 0x08-0x0F
    0,   0,   0,   0,   0, 'Q', '!',   0,     // 0x10-0x17
    0,   0, 'Z', 'S', 'A', 'W', '@',   0,     // 0x18-0x1F
    0, 'C', 'X', 'D', 'E', '$', '#',   0,     // 0x20-0x27
    0, ' ', 'V', 'F', 'T', 'R', '%',   0,     // 0x28-0x2F
    0, 'N', 'B', 'H', 'G', 'Y', '^',   0,     // 0x30-0x37
    0,   0, 'M', 'J', 'U', '&', '*',   0,     // 0x38-0x3F
    0, '<', 'K', 'I', 'O', ')', '(',   0,     // 0x40-0x47
    0, '>', '?', 'L', ':', 'P', '_',   0,     // 0x48-0x4F
    0,   0, '"',   0, '{', '+',   0,   0,     // 0x50-0x57
    0,   0,   0, '}',   0, '|',   0,   0,     // 0x58-0x5F
  };
  
  // Check if scancode is within our mapping range
  if (scancode >= sizeof(unshifted)) {
    return; // Unknown scancode
  }
  
  char result;
  
  // Get base character
  if (shiftPressed) {
    result = shifted[scancode];
  } else {
    result = unshifted[scancode];
  }
  
  // Apply caps lock to letters only
  if (result >= 'a' && result <= 'z' && capsLockOn && !shiftPressed) {
    result = result - 'a' + 'A';
  } else if (result >= 'A' && result <= 'Z' && capsLockOn && shiftPressed) {
    result = result - 'A' + 'a';
  }

  // Handle Alt
  if (altPressed) {
    result |= 0x80; // Extended ASCII
  }
  
  ps2Buffer.push(result);
}