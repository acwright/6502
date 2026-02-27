#include "GPIOKeyboardMatrixAttachment.h"
#include "utilities.h"

// Keyboard matrix mapping
// Rows are PA0-PA7, Columns are PB0-PB7
const uint8_t GPIOKeyboardMatrixAttachment::KEYBOARD_LAYOUT[8][8] = {
  // PB0    PB1    PB2    PB3    PB4    PB5    PB6    PB7
  {  '`',   '1',   '2',   '3',   '4',   '5',   '6',   '7'  }, // PA0
  {  '8',   '9',   '0',   '-',   '=',   0x08,  0x1B,  0x09 }, // PA1 (BS=Backspace, ESC, TAB)
  {  'q',   'w',   'e',   'r',   't',   'y',   'u',   'i'  }, // PA2
  {  'o',   'p',   '[',   ']',   '\\',  0x1A,  0x00,  'a'  }, // PA3 (INS, CAPS LOCK placeholder)
  {  's',   'd',   'f',   'g',   'h',   'j',   'k',   'l'  }, // PA4
  {  ';',   '\'',  0x0D,  0x7F,  0x00,  'z',   'x',   'c'  }, // PA5 (ENTER, DEL, SHIFT placeholder)
  {  'v',   'b',   'n',   'm',   ',',   '.',   '/',   0x1E }, // PA6 (UP)
  {  0x00,  0x00,  0x00,  ' ',   0x00,  0x1C,  0x1F,  0x1D }  // PA7 (CTRL, META/GUI, ALT placeholders, SPACE, FN placeholder, LEFT, DOWN, RIGHT)
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

void GPIOKeyboardMatrixAttachment::updateKey(uint8_t usbHidKeycode, bool pressed) {
  int8_t targetRow = -1;
  int8_t targetCol = -1;
  
  // Direct USB HID keycode to matrix position mapping for special/modifier keys
  // These keys don't have ASCII equivalents or need direct mapping
  switch (usbHidKeycode) {
    // Modifier keys
    case 0xE0: // Left Ctrl
    case 0xE4: // Right Ctrl
      targetRow = 7; targetCol = 0; // PA7, PB0 (CTRL)
      break;
    case 0xE1: // Left Shift
    case 0xE5: // Right Shift
      targetRow = 5; targetCol = 4; // PA5, PB4 (SHIFT)
      break;
    case 0xE2: // Left Alt
    case 0xE6: // Right Alt
      targetRow = 7; targetCol = 2; // PA7, PB2 (ALT)
      break;
    case 0xE3: // Left GUI (Windows/Command)
    case 0xE7: // Right GUI
      targetRow = 7; targetCol = 1; // PA7, PB1 (META/GUI)
      break;
    
    // Special keys that might not map well through ASCII
    case 0x39: // Caps Lock
      targetRow = 3; targetCol = 6; // PA3, PB6 (CAPS LOCK)
      break;
      
    // Function keys (F1-F10) - map to FN + number combination
    case 0x3A: // F1 = FN + 1
    case 0x3B: // F2 = FN + 2
    case 0x3C: // F3 = FN + 3
    case 0x3D: // F4 = FN + 4
    case 0x3E: // F5 = FN + 5
    case 0x3F: // F6 = FN + 6
    case 0x40: // F7 = FN + 7
    case 0x41: // F8 = FN + 8
    case 0x42: // F9 = FN + 9
    case 0x43: // F10 = FN + 0
      {
        // Always set the FN key (PA7, PB4)
        if (pressed) {
          keyboardMatrix[7] |= (1 << 4);
        } else {
          keyboardMatrix[7] &= ~(1 << 4);
        }
        
        // Map to corresponding number key
        uint8_t numberKey;
        if (usbHidKeycode >= 0x3A && usbHidKeycode <= 0x42) {
          // F1-F9 map to '1'-'9'
          numberKey = '1' + (usbHidKeycode - 0x3A);
        } else {
          // F10 maps to '0'
          numberKey = '0';
        }
        
        // Find and set the number key in the matrix
        for (int row = 0; row < 8; row++) {
          for (int col = 0; col < 8; col++) {
            if (KEYBOARD_LAYOUT[row][col] == numberKey) {
              if (pressed) {
                keyboardMatrix[row] |= (1 << col);
              } else {
                keyboardMatrix[row] &= ~(1 << col);
              }
              return;  // Done processing
            }
          }
        }
        return;  // If number key not found, still return
      }
      break;
    
    default:
      // For all other keys, try ASCII mapping
      uint8_t asciiKey = USB_HID_TO_ASCII[usbHidKeycode];
      
      // If no ASCII mapping, ignore the key
      if (asciiKey == 0x00) {
        return;
      }
      
      // Find the ASCII key in the keyboard layout
      for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
          if (KEYBOARD_LAYOUT[row][col] == asciiKey) {
            targetRow = row;
            targetCol = col;
            break;
          }
        }
        if (targetRow != -1) break;
      }
      break;
  }
  
  // Update the keyboard matrix if we found a valid position
  if (targetRow != -1 && targetCol != -1) {
    if (pressed) {
      keyboardMatrix[targetRow] |= (1 << targetCol);  // Set bit (key pressed)
    } else {
      keyboardMatrix[targetRow] &= ~(1 << targetCol);  // Clear bit (key released)
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
