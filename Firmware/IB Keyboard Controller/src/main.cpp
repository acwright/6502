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

void onInterrupt();

void setup() {
  pinMode(CSB, INPUT_PULLUP);
  pinMode(DATA, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(INT, OUTPUT);

  digitalWrite(CLK, HIGH);
  digitalWrite(DATA, LOW);
  digitalWrite(INT, LOW);

  pinMode(PS2CLK, INPUT_PULLUP);
  pinMode(PS2DATA, INPUT_PULLUP);

  shiftOut(DATA, CLK, MSBFIRST, 0x00);

  attachInterrupt(digitalPinToInterrupt(PS2CLK), onInterrupt, FALLING);
}

void loop() {
  if (digitalRead(CSB) == HIGH || buffer.isEmpty()) { return; } // Not selected or buffer empty

  shiftOut(DATA, CLK, MSBFIRST, buffer.shift()); // Shift out the keycode
  digitalWrite(DATA, LOW);
  digitalWrite(CLK, LOW); // Clock out one more time to latch the data
  delayMicroseconds(5);
  digitalWrite(CLK, HIGH);
  digitalWrite(INT, HIGH); // Signal that data is ready
  delayMicroseconds(5);
  digitalWrite(INT, LOW);
  delayMicroseconds(100); // Wait a bit before triggering next interrupt
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
      //if (parity < 0xFD) {               // Good so save byte in buffer
        buffer.push(incoming);           // Add to circular buffer (if full, oldest data lost)
      //}
      break;
    default:
      bitcount = 0;                      // Shouldn't be here so reset state machine
      break;
  }
}