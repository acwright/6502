#define CIRCULAR_BUFFER_INT_SAFE

#include <Arduino.h>
#include <CircularBuffer.hpp>

#define CSB 5
#define DATA 1
#define CLK 0
#define INT 4
#define PS2DATA 3
#define PS2CLK  2

#define BUFFER_SIZE 16

CircularBuffer<uint8_t, BUFFER_SIZE> buffer;

bool enabled = false;
bool altPressed = false;
bool shiftPressed = false;
bool ctrlPressed = false;
bool capsLockOn = false;
bool breakCodeNext = false;
bool extendedCodeNext = false;

void onInterrupt();
void enable();
void disable();
void ps2ToAscii(uint8_t scancode);

void setup() {
  pinMode(CSB, INPUT_PULLUP);
  pinMode(INT, OUTPUT);
  pinMode(PS2CLK, INPUT_PULLUP);
  pinMode(PS2DATA, INPUT_PULLUP);

  digitalWrite(INT, LOW);
  
  attachInterrupt(digitalPinToInterrupt(PS2CLK), onInterrupt, FALLING);
}

void loop() {
  int csbState = digitalRead(CSB);

  if (csbState == LOW && !enabled) {
    enable();
  } else if (csbState == HIGH && enabled) {
    disable();
  }

  if (csbState == HIGH || buffer.isEmpty()) { return; } // Not selected or buffer empty

  shiftOut(DATA, CLK, MSBFIRST, buffer.shift()); // Shift out the keycode
  digitalWrite(DATA, LOW);
  digitalWrite(CLK, HIGH); // Clock out one more time to latch the data
  delayMicroseconds(5);
  digitalWrite(CLK, LOW);
  digitalWrite(INT, HIGH); // Signal that data is ready
  delayMicroseconds(5);
  digitalWrite(INT, LOW);
  delayMicroseconds(100); // Wait a bit before triggering next interrupt
}

void enable() {
  enabled = true;

  pinMode(DATA, OUTPUT);
  pinMode(CLK, OUTPUT);

  digitalWrite(CLK, HIGH);
  digitalWrite(DATA, LOW);

  // Clear the shift register
  shiftOut(DATA, CLK, MSBFIRST, 0x00);
  digitalWrite(DATA, LOW);
  digitalWrite(CLK, HIGH);
  delayMicroseconds(5);
  digitalWrite(CLK, LOW);
}

void disable() {
  enabled = false;

  pinMode(DATA, INPUT);
  pinMode(CLK, INPUT);
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
      buffer.push(altPressed ? 0x91 : 0x81);
      return;
    case 0x06: // F2
      buffer.push(altPressed ? 0x92 : 0x82);
      return;
    case 0x04: // F3
      buffer.push(altPressed ? 0x93 : 0x83);
      return;
    case 0x0C: // F4
      buffer.push(altPressed ? 0x94 : 0x84);
      return;
    case 0x03: // F5
      buffer.push(altPressed ? 0x95 : 0x85);
      return;
    case 0x0B: // F6
      buffer.push(altPressed ? 0x96 : 0x86);
      return;
    case 0x83: // F7
      buffer.push(altPressed ? 0x97 : 0x87);
      return;
    case 0x0A: // F8
      buffer.push(altPressed ? 0x98 : 0x88);
      return;
    case 0x01: // F9
      buffer.push(altPressed ? 0x99 : 0x89);
      return;
    case 0x09: // F10
      buffer.push(altPressed ? 0x9A : 0x8A);
      return;
    case 0x78: // F11
      buffer.push(altPressed ? 0x9B : 0x8B);
      return;
    case 0x07: // F12
      buffer.push(altPressed ? 0x9C : 0x8C);
      return;
    case 0x0D: // Tab
      buffer.push(0x09);
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
        buffer.push(altPressed ? 0x90 : 0x80); // Right Windows / Menu
      } else {
        buffer.push(altPressed ? 0x90 : 0x80); // Left Windows / Menu
      }
      return;
    case 0x27: // Windows / Menu (additional scancode)
    case 0x2F:
      buffer.push(altPressed ? 0x90 : 0x80);
      return;
    case 0x58: // Caps Lock
      capsLockOn = !capsLockOn;
      return;
    case 0x5A: // Enter
      if (extendedCodeNext) {
        extendedCodeNext = false;
        buffer.push(0x0D); // Keypad Enter
      } else {
        buffer.push(0x0D); // Main Enter
      }
      return;
    case 0x66: // Backspace
      buffer.push(0x08);
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
        buffer.push(0x1C);
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
        buffer.push(0x1A);
      }
      return;
    case 0x71: // Delete (E0 71)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        buffer.push(0x7F);
      }
      return;
    case 0x72: // Down Arrow (E0 72)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        buffer.push(0x1F);
      }
      return;
    case 0x74: // Right Arrow (E0 74)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        buffer.push(0x1D);
      }
      return;
    case 0x75: // Up Arrow (E0 75)
      if (extendedCodeNext) {
        extendedCodeNext = false;
        buffer.push(0x1E);
      }
      return;
    case 0x76: // Escape
      buffer.push(0x1B);
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
        buffer.push(0x00); // NULL
        return;
      case 0x1C: // A
        buffer.push(0x01); // SOH
        return;
      case 0x32: // B
        buffer.push(0x02); // STX
        return;
      case 0x21: // C
        buffer.push(0x03); // ETX
        return;
      case 0x23: // D
        buffer.push(0x04); // EOT
        return;
      case 0x24: // E
        buffer.push(0x05); // ENQ
        return;
      case 0x2B: // F
        buffer.push(0x06); // ACK
        return;
      case 0x34: // G
        buffer.push(0x07); // BEL
        return;
      case 0x33: // H
        buffer.push(0x08); // BS
        return;
      case 0x43: // I
        buffer.push(0x09); // HT
        return;
      case 0x3B: // J
        buffer.push(0x0A); // LF
        return;
      case 0x42: // K
        buffer.push(0x0B); // VT
        return;
      case 0x4B: // L
        buffer.push(0x0C); // FF
        return;
      case 0x3A: // M
        buffer.push(0x0D); // CR
        return;
      case 0x31: // N
        buffer.push(0x0E); // SO
        return;
      case 0x44: // O
        buffer.push(0x0F); // SI
        return;
      case 0x4D: // P
        buffer.push(0x10); // DLE
        return;
      case 0x15: // Q
        buffer.push(0x11); // DC1
        return;
      case 0x2D: // R
        buffer.push(0x12); // DC2
        return;
      case 0x1B: // S
        buffer.push(0x13); // DC3
        return;
      case 0x2C: // T
        buffer.push(0x14); // DC4
        return;
      case 0x3C: // U
        buffer.push(0x15); // NAK
        return;
      case 0x2A: // V
        buffer.push(0x16); // SYN
        return;
      case 0x1D: // W
        buffer.push(0x17); // ETB
        return;
      case 0x22: // X
        buffer.push(0x18); // CAN
        return;
      case 0x35: // Y
        buffer.push(0x19); // EM
        return;
      case 0x1A: // Z
        buffer.push(0x1A); // SUB
        return;
      case 0x54: // [
        buffer.push(0x1B); // ESC
        return;
      case 0x5D: // "\"
        buffer.push(0x1C); // FS (LEFT)
        return;
      case 0x5B: // ]
        buffer.push(0x1D); // GS (RIGHT)
        return;
      case 0x36: // 6 or ^
        buffer.push(0x1E); // RS (UP)
        return;
      case 0x4E: // - or _
        buffer.push(0x1F); // US (DOWN)
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
  
  buffer.push(result);
}