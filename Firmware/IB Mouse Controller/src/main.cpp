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
unsigned long last_run = millis();

void enable();
void disable();

void setup() {
  delay(100);

  mouse.initialize();

  pinMode(CS, INPUT_PULLUP);
  pinMode(INT, OUTPUT);

  digitalWrite(INT, LOW);
}

void loop() {
  int csState = digitalRead(CS);

  if (csState == HIGH && !enabled) {
    enable();
  } else if (csState == LOW && enabled) {
    disable();
  }

  if (csState == LOW) { return; } // Not selected

  if (millis() - last_run > 250) {
    last_run = millis();
    
    MouseData data = mouse.readData();

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