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
bool shiftPressed = false;
bool ctrlPressed = false;
bool breakCodeNext = false;
bool extendedCodeNext = false;

// Matrix keyboard state
bool matrixEnabled = false;
bool matrixShiftPressed = false;
bool matrixCtrlPressed = false;

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
  {             0x51,  0x57,  0x45,  0x52,  0x54,  0x59,  0x55,  0x49 },  // Q W E R T Y U I
  // PA3/Row 3
  {             0x4F,  0x50,  0x5B,  0x5D,  0x5C,  0x1A,  0xFF,  0x41 },  // O P [ ] \ INS (ign) A
  // PA4/Row 4
  {             0x53,  0x44,  0x46,  0x47,  0x48,  0x4A,  0x4B,  0x4C },  // S D F G H J K L
  // PA5/Row 5
  {             0x3B,  0x27,  0x0D,  0x7F,  0xFE,  0x5A,  0x58,  0x43 },  // ; ' ENTER DEL SHIFT Z X C
  // PA6/Row 6
  {             0x56,  0x42,  0x4E,  0x4D,  0x2C,  0x2E,  0x2F,  0x1E },  // V B N M , . / UP
  // PA7/Row 7
  {             0xFE,  0xFF,  0xFF,  0x20,  0xFF,  0x1C,  0x1F,  0x1D }   // CTRL (ign) (ign) SPACE (ign) LEFT DOWN RIGHT
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
    
    // Ensure Port A is in output mode (matrix scanning leaves it as INPUT)
    pinMode(VIA_PA0, OUTPUT);
    pinMode(VIA_PA1, OUTPUT);
    pinMode(VIA_PA2, OUTPUT);
    pinMode(VIA_PA3, OUTPUT);
    pinMode(VIA_PA4, OUTPUT);
    pinMode(VIA_PA5, OUTPUT);
    pinMode(VIA_PA6, OUTPUT);
    pinMode(VIA_PA7, OUTPUT);

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
    // When two keys on the same row are held simultaneously, their switches
    // bridge Port B columns through the shared row wire.  This corrupts the
    // output byte because the ATmega's output drivers fight each other through
    // that path.  Defer output until no row has multiple pressed keys.
    bool hasRowConflict = false;
    for (uint8_t row = 0; row < 8; row++) {
      uint8_t count = 0;
      for (uint8_t col = 0; col < 8; col++) {
        if (matrixKeyState[row][col] >= MATRIX_DEBOUNCE_COUNT) {
          if (++count >= 2) {
            hasRowConflict = true;
            break;
          }
        }
      }
      if (hasRowConflict) break;
    }

    if (!hasRowConflict) {
      // Set Port A (row lines) to high-impedance to prevent bus contention
      // through a single pressed key switch when driving data on Port B
      pinMode(VIA_PA0, INPUT);
      pinMode(VIA_PA1, INPUT);
      pinMode(VIA_PA2, INPUT);
      pinMode(VIA_PA3, INPUT);
      pinMode(VIA_PA4, INPUT);
      pinMode(VIA_PA5, INPUT);
      pinMode(VIA_PA6, INPUT);
      pinMode(VIA_PA7, INPUT);

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
  
  uint8_t portAPins[] = {VIA_PA0, VIA_PA1, VIA_PA2, VIA_PA3, VIA_PA4, VIA_PA5, VIA_PA6, VIA_PA7};
  uint8_t portBPins[] = {VIA_PB0, VIA_PB1, VIA_PB2, VIA_PB3, VIA_PB4, VIA_PB5, VIA_PB6, VIA_PB7};
  
  // Phase 1: Scan all rows, update debounce state only
  for (uint8_t row = 0; row < 8; row++) {
    // Drive only the active row LOW; all others high-Z to prevent
    // current leaking through pressed keys on inactive rows (ghosting)
    for (uint8_t i = 0; i < 8; i++) {
      if (i == row) {
        pinMode(portAPins[i], OUTPUT);
        digitalWrite(portAPins[i], LOW);
      } else {
        pinMode(portAPins[i], INPUT);
      }
    }
    
    // Configure Port B pins as inputs with pullups for reading
    for (uint8_t col = 0; col < 8; col++) {
      pinMode(portBPins[col], INPUT_PULLUP);
    }
    
    // Small delay to let signals stabilize
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
    }
    
    // Restore Port B to output mode for data transmission
    for (uint8_t col = 0; col < 8; col++) {
      pinMode(portBPins[col], OUTPUT);
    }
  }
  
  // Release all row lines after scanning
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(portAPins[i], INPUT);
  }
  
  // Phase 2: Update modifier states from current debounce results
  matrixShiftPressed = (matrixKeyState[5][4] >= MATRIX_DEBOUNCE_COUNT);
  matrixCtrlPressed  = (matrixKeyState[7][0] >= MATRIX_DEBOUNCE_COUNT);
  
  // Phase 3: Fire key events with up-to-date modifier state
  for (uint8_t row = 0; row < 8; row++) {
    for (uint8_t col = 0; col < 8; col++) {
      bool keyPressed = (matrixKeyState[row][col] >= MATRIX_DEBOUNCE_COUNT);
      bool keyWasPressed = matrixKeyLast[row][col];
      
      // Detect key press event (transition from not pressed to pressed)
      if (keyPressed && !keyWasPressed) {
        uint8_t ascii = mapMatrixToAscii(row, col);
        if (ascii != 0xFF && ascii != 0xFE) {  // 0xFF = no key, 0xFE = modifier
          matrixBuffer.push(ascii);
        }
      }
      
      matrixKeyLast[row][col] = keyPressed;
    }
  }
}

// Map matrix position to ASCII code based on modifiers
uint8_t mapMatrixToAscii(uint8_t row, uint8_t col) {
  // Get base key code from PROGMEM
  uint8_t baseKey = pgm_read_byte(&matrixMap[row][col]);
  
  // Handle special keys
  if (baseKey == 0xFF) return 0xFF;  // No key / ignored
  if (baseKey == 0xFE) return 0xFE;  // Modifier key
  
  // Ctrl takes priority (Shift ignored when Ctrl held)
  if (matrixCtrlPressed) {
    if (baseKey == 0x32) return 0x00;  // Ctrl+2 = NUL
    if (baseKey >= 0x41 && baseKey <= 0x5A) {  // Ctrl+A-Z
      return baseKey - 0x40;  // Convert to 0x01-0x1A
    }
    if (baseKey == 0x5B) return 0x1B;  // Ctrl+[ = ESC
    if (baseKey == 0x5C) return 0x1C;  // Ctrl+\ = FS
    if (baseKey == 0x5D) return 0x1D;  // Ctrl+] = GS
    if (baseKey == 0x36) return 0x1E;  // Ctrl+6 = RS
    if (baseKey == 0x2D) return 0x1F;  // Ctrl+- = US
    return 0xFF;  // Ctrl + unmapped key = no output
  }
  
  // Shift only affects numbers and symbols (not letters)
  if (matrixShiftPressed) {
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
  }
  
  // Return base key (letters are already uppercase in the map)
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
        case 0x14: // Right Control
          ctrlPressed = false;
          break;
      }
      return;
    }
    
    switch (scancode) {
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
    case 0x0D: // Tab
      ps2Buffer.push(0x09);
      return;
    case 0x12: // Left Shift
    case 0x59: // Right Shift
      shiftPressed = true;
      return;
    case 0x14: // Control
      if (extendedCodeNext) {
        extendedCodeNext = false;
      }
      ctrlPressed = true;
      return;
    case 0x5A: // Enter
      if (extendedCodeNext) {
        extendedCodeNext = false;
      }
      ps2Buffer.push(0x0D);
      return;
    case 0x66: // Backspace
      ps2Buffer.push(0x08);
      return;
    case 0x6B: // Left Arrow (E0 6B)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        ps2Buffer.push(0x1C);
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
    // Ignored keys: F-keys, Alt, GUI, CapsLock, navigation, keypad
    case 0x01: case 0x03: case 0x04: case 0x05: case 0x06:
    case 0x07: case 0x09: case 0x0A: case 0x0B: case 0x0C:
    case 0x11: case 0x1F: case 0x27: case 0x2F:
    case 0x58: case 0x69: case 0x6C:
    case 0x73: case 0x77: case 0x78: case 0x79:
    case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E:
    case 0x83:
      extendedCodeNext = false;
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
    0,   0,   0,   0,   0, 'Q', '1',   0,     // 0x10-0x17
    0,   0, 'Z', 'S', 'A', 'W', '2',   0,     // 0x18-0x1F
    0, 'C', 'X', 'D', 'E', '4', '3',   0,     // 0x20-0x27
    0, ' ', 'V', 'F', 'T', 'R', '5',   0,     // 0x28-0x2F
    0, 'N', 'B', 'H', 'G', 'Y', '6',   0,     // 0x30-0x37
    0,   0, 'M', 'J', 'U', '7', '8',   0,     // 0x38-0x3F
    0, ',', 'K', 'I', 'O', '0', '9',   0,     // 0x40-0x47
    0, '.', '/', 'L', ';', 'P', '-',   0,     // 0x48-0x4F
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
  
  // Get character (letters are always uppercase in both tables)
  if (shiftPressed) {
    result = shifted[scancode];
  } else {
    result = unshifted[scancode];
  }
  
  if (result != 0) {
    ps2Buffer.push(result);
  }
}