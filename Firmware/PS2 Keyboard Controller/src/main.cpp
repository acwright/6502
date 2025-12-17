#define CIRCULAR_BUFFER_INT_SAFE

#include <Arduino.h>
#include <CircularBuffer.hpp>

#define AX0 14
#define AX1 15
#define AX2 16

#define AY0 17
#define AY1 18
#define AY2 19

#define DATA 11
#define STROBE 12
#define RESET 13
#define PS2DATA 3
#define PS2CLK  2

#define BUFFER_SIZE 16

CircularBuffer<uint8_t, BUFFER_SIZE> buffer;

bool release = false;

void onInterrupt();
void writeMatrix(uint8_t x, uint8_t y, uint8_t data);
void ps2ToMatrix(uint8_t scancode);

void setup() {
  pinMode(AX0, OUTPUT);
  pinMode(AX1, OUTPUT);
  pinMode(AX2, OUTPUT);

  pinMode(AY0, OUTPUT);
  pinMode(AY1, OUTPUT);
  pinMode(AY2, OUTPUT);

  pinMode(DATA, OUTPUT);
  pinMode(STROBE, OUTPUT);
  pinMode(RESET, OUTPUT);
  
  pinMode(PS2CLK, INPUT_PULLUP);
  pinMode(PS2DATA, INPUT_PULLUP);

  // Reset the Mt8808
  digitalWrite(RESET, HIGH);
  delay(1);
  digitalWrite(RESET, LOW);
  delay(1);
  
  attachInterrupt(digitalPinToInterrupt(PS2CLK), onInterrupt, FALLING);
}

void loop() {
  if (buffer.isEmpty()) { return; }

  ps2ToMatrix(buffer.shift());
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
      parity += val;                     // another one received ?
      incoming >>= 1;                    // right shift one place for next bit
      incoming |= (val) ? 0x80 : 0;      // or in MSbit
      break;
    case 10:                             // Parity check
      parity &= 1;                       // Get LSB if 1 = odd number of 1's so parity should be 0
      if (parity == val)                 // Both same parity error
        parity = 0xFD;                   // To ensure at next bit count clear and discard
      break;
    case 11: // Stop bit
      if (parity < 0xFD) {               // Good so save byte in buffer, otherwise discard
        buffer.push(incoming);           // Add to circular buffer if valid scancode (if full, oldest data lost)
      }
      bitcount = 0;
      break;
    default:
      bitcount = 0;                      // Shouldn't be here so reset state machine
      break;
  }
}

// Output data to Matrix at X,Y coordinates
void writeMatrix(uint8_t x, uint8_t y, uint8_t data) {
  digitalWrite(AX0, (x & 0b001) >> 0);
  digitalWrite(AX1, (x & 0b010) >> 1);
  digitalWrite(AX2, (x & 0b100) >> 2);

  digitalWrite(AY0, (y & 0b001) >> 0);
  digitalWrite(AY1, (y & 0b010) >> 1);
  digitalWrite(AY2, (y & 0b100) >> 2);

  digitalWrite(DATA, data);

  digitalWrite(STROBE, HIGH);
  delay(1);
  digitalWrite(STROBE, LOW);
  delay(1);
}

// PS/2 to Matrix conversion function
void ps2ToMatrix(uint8_t scancode) {
  // Handle special code prefix
  if (scancode == 0xE0) {
    return;
  }
  if (scancode == 0xE1) {
    return;
  }

  // Handle break code prefix
  if (scancode == 0xF0) {
    release = true;
    return;
  }

  // Handle scancode

  switch (scancode) {
    // Row X0
    case 0x0E:      // `
      writeMatrix(0x0, 0x0, release ? 0x0 : 0x1);
      break;
    case 0x16:      // 1
      writeMatrix(0x0, 0x1, release ? 0x0 : 0x1);
      break;
    case 0x1E:      // 2
      writeMatrix(0x0, 0x2, release ? 0x0 : 0x1);
      break;
    case 0x26:      // 3
      writeMatrix(0x0, 0x3, release ? 0x0 : 0x1);
      break;
    case 0x25:      // 4
      writeMatrix(0x0, 0x4, release ? 0x0 : 0x1);
      break;
    case 0x2E:      // 5
      writeMatrix(0x0, 0x5, release ? 0x0 : 0x1);
      break;
    case 0x36:      // 6
      writeMatrix(0x0, 0x6, release ? 0x0 : 0x1);
      break;
    case 0x3D:      // 7
      writeMatrix(0x0, 0x7, release ? 0x0 : 0x1);
      break;
    // Row X1
    case 0x3E:      // 8
      writeMatrix(0x1, 0x0, release ? 0x0 : 0x1);
      break;
    case 0x46:      // 9
      writeMatrix(0x1, 0x1, release ? 0x0 : 0x1);
      break;
    case 0x45:      // 0
      writeMatrix(0x1, 0x2, release ? 0x0 : 0x1);
      break;
    case 0x4E:      // -
      writeMatrix(0x1, 0x3, release ? 0x0 : 0x1);
      break;
    case 0x55:      // =
      writeMatrix(0x1, 0x4, release ? 0x0 : 0x1);
      break;
    case 0x66:      // BACKSPACE
      writeMatrix(0x1, 0x5, release ? 0x0 : 0x1);
      break;
    case 0x76:      // ESC
      writeMatrix(0x1, 0x6, release ? 0x0 : 0x1);
      break;
    case 0x0D:      // TAB
      writeMatrix(0x1, 0x7, release ? 0x0 : 0x1);
      break;
    // Row X2
    case 0x15:      // Q
      writeMatrix(0x2, 0x0, release ? 0x0 : 0x1);
      break;
    case 0x1D:      // W
      writeMatrix(0x2, 0x1, release ? 0x0 : 0x1);
      break;
    case 0x24:      // E
      writeMatrix(0x2, 0x2, release ? 0x0 : 0x1);
      break;
    case 0x2D:      // R
      writeMatrix(0x2, 0x3, release ? 0x0 : 0x1);
      break;
    case 0x2C:      // T
      writeMatrix(0x2, 0x4, release ? 0x0 : 0x1);
      break;
    case 0x35:      // Y
      writeMatrix(0x2, 0x5, release ? 0x0 : 0x1);
      break;
    case 0x3C:      // U
      writeMatrix(0x2, 0x6, release ? 0x0 : 0x1);
      break;
    case 0x43:      // I
      writeMatrix(0x2, 0x7, release ? 0x0 : 0x1);
      break;
    // Row X3
    case 0x44:      // O
      writeMatrix(0x3, 0x0, release ? 0x0 : 0x1);
      break;
    case 0x4D:      // P
      writeMatrix(0x3, 0x1, release ? 0x0 : 0x1);
      break;
    case 0x54:      // [
      writeMatrix(0x3, 0x2, release ? 0x0 : 0x1);
      break;
    case 0x5B:      // ]
      writeMatrix(0x3, 0x3, release ? 0x0 : 0x1);
      break;
    case 0x5D:      // "\"
      writeMatrix(0x3, 0x4, release ? 0x0 : 0x1);
      break;
    case 0x70:      // INSERT
      writeMatrix(0x3, 0x5, release ? 0x0 : 0x1);
      break;
    case 0x58:      // CAPS LOCK
      writeMatrix(0x3, 0x6, release ? 0x0 : 0x1);
      break;
    case 0x1C:      // A
      writeMatrix(0x3, 0x7, release ? 0x0 : 0x1);
      break;
    // Row X4
    case 0x1B:      // S
      writeMatrix(0x4, 0x0, release ? 0x0 : 0x1);
      break;
    case 0x23:      // D
      writeMatrix(0x4, 0x1, release ? 0x0 : 0x1);
      break;
    case 0x2B:      // F
      writeMatrix(0x4, 0x2, release ? 0x0 : 0x1);
      break;
    case 0x34:      // G
      writeMatrix(0x4, 0x3, release ? 0x0 : 0x1);
      break;
    case 0x33:      // H
      writeMatrix(0x4, 0x4, release ? 0x0 : 0x1);
      break;
    case 0x3B:      // J
      writeMatrix(0x4, 0x5, release ? 0x0 : 0x1);
      break;
    case 0x42:      // K
      writeMatrix(0x4, 0x6, release ? 0x0 : 0x1);
      break;
    case 0x4B:      // L
      writeMatrix(0x4, 0x7, release ? 0x0 : 0x1);
      break;
    // Row X5
    case 0x4C:      // ;
      writeMatrix(0x5, 0x0, release ? 0x0 : 0x1);
      break;
    case 0x52:      // '
      writeMatrix(0x5, 0x1, release ? 0x0 : 0x1);
      break;
    case 0x5A:      // ENTER
      writeMatrix(0x5, 0x2, release ? 0x0 : 0x1);
      break;
    case 0x71:      // DELETE
      writeMatrix(0x5, 0x3, release ? 0x0 : 0x1);
      break;
    case 0x12:      // SHIFT
    case 0x59:
      writeMatrix(0x5, 0x4, release ? 0x0 : 0x1);
      break;
    case 0x1A:      // Z
      writeMatrix(0x5, 0x5, release ? 0x0 : 0x1);
      break;
    case 0x22:      // X
      writeMatrix(0x5, 0x6, release ? 0x0 : 0x1);
      break;
    case 0x21:      // C
      writeMatrix(0x5, 0x7, release ? 0x0 : 0x1);
      break;
    // Row X6
    case 0x2A:      // V
      writeMatrix(0x6, 0x0, release ? 0x0 : 0x1);
      break;
    case 0x32:      // B
      writeMatrix(0x6, 0x1, release ? 0x0 : 0x1);
      break;
    case 0x31:      // N
      writeMatrix(0x6, 0x2, release ? 0x0 : 0x1);
      break;
    case 0x3A:      // M
      writeMatrix(0x6, 0x3, release ? 0x0 : 0x1);
      break;
    case 0x41:      // ,
      writeMatrix(0x6, 0x4, release ? 0x0 : 0x1);
      break;
    case 0x49:      // .
      writeMatrix(0x6, 0x5, release ? 0x0 : 0x1);
      break;
    case 0x4A:      // /
      writeMatrix(0x6, 0x6, release ? 0x0 : 0x1);
      break;
    case 0x75:      // UP
      writeMatrix(0x6, 0x7, release ? 0x0 : 0x1);
      break;
    // Row X7
    case 0x14:      // CTRL
      writeMatrix(0x7, 0x0, release ? 0x0 : 0x1);
      break;
    case 0x2F:      // MENU
    case 0x1F:
    case 0x27:
      writeMatrix(0x7, 0x1, release ? 0x0 : 0x1);
      break;
    case 0x11:      // ALT
      writeMatrix(0x7, 0x2, release ? 0x0 : 0x1);
      break;
    case 0x29:      // SPACE
      writeMatrix(0x7, 0x3, release ? 0x0 : 0x1);
      break;
    // N/A...       // FN
    case 0x6B:      // LEFT
      writeMatrix(0x7, 0x5, release ? 0x0 : 0x1);
      break;
    case 0x72:      // DOWN
      writeMatrix(0x7, 0x6, release ? 0x0 : 0x1);
      break;
    case 0x74:      // RIGHT
      writeMatrix(0x7, 0x7, release ? 0x0 : 0x1);
      break;
    // Function Keys
    case 0x05:      // F1
      writeMatrix(0x7, 0x4, release ? 0x0 : 0x1);
      writeMatrix(0x0, 0x1, release ? 0x0 : 0x1);
      break;
    case 0x06:      // F2
      writeMatrix(0x7, 0x4, release ? 0x0 : 0x1);
      writeMatrix(0x0, 0x2, release ? 0x0 : 0x1);
      break;
    case 0x04:      // F3
      writeMatrix(0x7, 0x4, release ? 0x0 : 0x1);
      writeMatrix(0x0, 0x3, release ? 0x0 : 0x1);
      break;
    case 0x0C:      // F4
      writeMatrix(0x7, 0x4, release ? 0x0 : 0x1);
      writeMatrix(0x0, 0x4, release ? 0x0 : 0x1);
      break;
    case 0x03:      // F5
      writeMatrix(0x7, 0x4, release ? 0x0 : 0x1);
      writeMatrix(0x0, 0x5, release ? 0x0 : 0x1);
      break;
    case 0x0B:      // F6
      writeMatrix(0x7, 0x4, release ? 0x0 : 0x1);
      writeMatrix(0x0, 0x6, release ? 0x0 : 0x1);
      break;
    case 0x83:      // F7
      writeMatrix(0x7, 0x4, release ? 0x0 : 0x1);
      writeMatrix(0x0, 0x7, release ? 0x0 : 0x1);
      break;
    case 0x0A:      // F8
      writeMatrix(0x7, 0x4, release ? 0x0 : 0x1);
      writeMatrix(0x1, 0x0, release ? 0x0 : 0x1);
      break;
    case 0x01:      // F9
      writeMatrix(0x7, 0x4, release ? 0x0 : 0x1);
      writeMatrix(0x1, 0x1, release ? 0x0 : 0x1);
      break;
    case 0x09:      // F10
      writeMatrix(0x7, 0x4, release ? 0x0 : 0x1);
      writeMatrix(0x1, 0x2, release ? 0x0 : 0x1);
      break;
    default:
      break;
  }

  if (release) {
    release = false;
  }
}