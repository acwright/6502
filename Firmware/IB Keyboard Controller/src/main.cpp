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

void onInterrupt();
void enable();
void disable();

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
        buffer.push(incoming);           // Add to circular buffer (if full, oldest data lost)
      }
      bitcount = 0;
      break;
    default:
      bitcount = 0;                      // Shouldn't be here so reset state machine
      break;
  }
}