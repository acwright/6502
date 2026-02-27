#include "GPIOKeyboardMatrixAttachment.h"

// Keyboard matrix mapping (same as original GPIOCard)
// Rows are PA0-PA7, Columns are PB0-PB7
const uint8_t GPIOKeyboardMatrixAttachment::KEYBOARD_LAYOUT[8][8] = {
  // PB0    PB1    PB2    PB3    PB4    PB5    PB6    PB7
  {  '`',   '1',   '2',   '3',   '4',   '5',   '6',   '7'  }, // PA0
  {  '8',   '9',   '0',   '-',   '=',   0x08,  0x1B,  0x09 }, // PA1 (BS=Backspace, ESC, TAB)
  {  'q',   'w',   'e',   'r',   't',   'y',   'u',   'i'  }, // PA2
  {  'o',   'p',   '[',   ']',   '\\',  0x1A,  0x00,  'a'  }, // PA3 (INS, CAPS LOCK placeholder)
  {  's',   'd',   'f',   'g',   'h',   'j',   'k',   'l'  }, // PA4
  {  ';',   '\'',  0x0D,  0x7F,  0x00,  'z',   'x',   'c'  }, // PA5 (ENTER, DEL, SHIFT placeholder)
  {  'v',   'b',   'n',   'm',   ',',   '.',   '/',   0x0E }, // PA6 (UP placeholder)
  {  0x00,  0x00,  0x00,  ' ',   0x00,  0x1C,  0x1F,  0x1D }  // PA7 (CTRL, MENU, ALT placeholders, SPACE, FN placeholder, LEFT, DOWN, RIGHT)
};

GPIOKeyboardMatrixAttachment::GPIOKeyboardMatrixAttachment(uint8_t priority)
  : GPIOAttachment(priority, false, false, false, false),
    selectedColumns(0xFF) {
  reset();
}

void GPIOKeyboardMatrixAttachment::reset() {
  GPIOAttachment::reset();
  for (int i = 0; i < 8; i++) {
    keyboardMatrix[i] = 0x00;
  }
  selectedColumns = 0xFF;
}

uint8_t GPIOKeyboardMatrixAttachment::readPortA(uint8_t ddrA, uint8_t orA) {
  uint8_t rowStates = 0xFF;  // All rows high (not pressed) by default
  
  // Check keyboard matrix based on selected columns
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      // If column is selected (low) and key is pressed in matrix
      if (!(selectedColumns & (1 << col)) && (keyboardMatrix[row] & (1 << col))) {
        rowStates &= ~(1 << row);  // Pull row low (active-low)
      }
    }
  }
  
  return rowStates;
}

uint8_t GPIOKeyboardMatrixAttachment::readPortB(uint8_t ddrB, uint8_t orB) {
  return 0xFF;  // Don't interfere with Port B reads
}

void GPIOKeyboardMatrixAttachment::writePortB(uint8_t value, uint8_t ddrB) {
  // Store the column selection for matrix scanning
  selectedColumns = value;
}

void GPIOKeyboardMatrixAttachment::updateKey(uint8_t key, bool pressed) {
  // Find the key in the keyboard layout
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if (KEYBOARD_LAYOUT[row][col] == key) {
        if (pressed) {
          keyboardMatrix[row] |= (1 << col);  // Set bit (key pressed)
        } else {
          keyboardMatrix[row] &= ~(1 << col);  // Clear bit (key released)
        }
        return;
      }
    }
  }
}

void GPIOKeyboardMatrixAttachment::updateMatrixPosition(uint8_t row, uint8_t col, bool pressed) {
  if (row < 8 && col < 8) {
    if (pressed) {
      keyboardMatrix[row] |= (1 << col);
    } else {
      keyboardMatrix[row] &= ~(1 << col);
    }
  }
}
