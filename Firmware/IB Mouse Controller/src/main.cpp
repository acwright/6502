#include <Arduino.h>
#include <PS2Mouse.h>

#define CS 5
#define DATA 1
#define CLK 0
#define INT 4
#define PS2DATA 3
#define PS2CLK  2

PS2Mouse mouse(PS2CLK, PS2DATA);

bool enabled = false;
unsigned long lastRun = millis();
uint8_t lastStatus = 0;

void enable();
void disable();

void setup() {
  pinMode(CS, INPUT_PULLUP);
  pinMode(INT, OUTPUT);
  
  digitalWrite(INT, LOW);
  
  delay(100);

  mouse.initialize();
}

void loop() {
  int csState = digitalRead(CS);

  if (csState == HIGH && !enabled) {
    enable();
  } else if (csState == LOW && enabled) {
    disable();
  }

  if (csState == LOW) { return; } // Not selected

  if (millis() - lastRun > 250) {
    lastRun = millis();
    
    MouseData data = mouse.readData();

    // Check if there's any movement or button state change
    // Button states are in bits 0-2 of status byte
    uint8_t current_buttons = data.status & 0x07;
    uint8_t last_buttons = lastStatus & 0x07;
    bool has_movement = (data.position.x != 0) || (data.position.y != 0) || (data.wheel != 0);
    bool button_changed = (current_buttons != last_buttons);
    
    // Only send data if there's movement or button state changed
    if (has_movement || button_changed) {
      lastStatus = data.status;
      
      shiftOut(DATA, CLK, MSBFIRST, data.status); // Shift out the mouse status
      digitalWrite(DATA, LOW);
      digitalWrite(CLK, HIGH); // Clock out one more time to latch the data
      delayMicroseconds(5);
      digitalWrite(CLK, LOW);
      digitalWrite(INT, HIGH); // Signal that data is ready
      delayMicroseconds(5);
      digitalWrite(INT, LOW);
      delayMicroseconds(100); // Wait a bit before triggering next interrupt

      shiftOut(DATA, CLK, MSBFIRST, data.position.x); // Shift out the X movement
      digitalWrite(DATA, LOW);
      digitalWrite(CLK, HIGH); // Clock out one more time to latch the data
      delayMicroseconds(5);
      digitalWrite(CLK, LOW);
      digitalWrite(INT, HIGH); // Signal that data is ready
      delayMicroseconds(5);
      digitalWrite(INT, LOW);
      delayMicroseconds(100); // Wait a bit before triggering next interrupt
      
      shiftOut(DATA, CLK, MSBFIRST, data.position.y); // Shift out the Y movement
      digitalWrite(DATA, LOW);
      digitalWrite(CLK, HIGH); // Clock out one more time to latch the data
      delayMicroseconds(5);
      digitalWrite(CLK, LOW);
      digitalWrite(INT, HIGH); // Signal that data is ready
      delayMicroseconds(5);
      digitalWrite(INT, LOW);
      delayMicroseconds(100); // Wait a bit before triggering next interrupt

      shiftOut(DATA, CLK, MSBFIRST, data.wheel); // Shift out the Y movement
      digitalWrite(DATA, LOW);
      digitalWrite(CLK, HIGH); // Clock out one more time to latch the data
      delayMicroseconds(5);
      digitalWrite(CLK, LOW);
      digitalWrite(INT, HIGH); // Signal that data is ready
      delayMicroseconds(5);
      digitalWrite(INT, LOW);
      delayMicroseconds(100); // Wait a bit before triggering next interrupt
    }
  }
}

void enable() {
  enabled = true;
  lastStatus = 0; // Reset status tracking

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

  pinMode(INT, INPUT);
  pinMode(DATA, INPUT);
  pinMode(CLK, INPUT);
}