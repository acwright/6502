#include "GPIOCard.h"

// Keyboard matrix mapping
// Rows are PA0-PA7, Columns are PB0-PB7
static const uint8_t KEYBOARD_MATRIX[8][8] = {
  // PB0    PB1    PB2    PB3    PB4    PB5    PB6    PB7
  {  '`',   '1',   '2',   '3',   '4',   '5',   '6',   '7'  }, // PA0
  {  '8',   '9',   '0',   '-',   '=',   0x08,  0x1B,  0x09 }, // PA1 (BS=Backspace, ESC, TAB)
  {  'q',   'w',   'e',   'r',   't',   'y',   'u',   'i'  }, // PA2
  {  'o',   'p',   '[',   ']',   '\\',  0x00,  0x00,  'a'  }, // PA3 (INS, CAPS placeholders)
  {  's',   'd',   'f',   'g',   'h',   'j',   'k',   'l'  }, // PA4
  {  ';',   '\'',  0x0D,  0x7F,  0x00,  'z',   'x',   'c'  }, // PA5 (ENTER, DEL, SHIFT placeholder)
  {  'v',   'b',   'n',   'm',   ',',   '.',   '/',   0x00 }, // PA6 (UP placeholder)
  {  0x00,  0x00,  0x00,  ' ',   0x00,  0x00,  0x00,  0x00 }  // PA7 (CTRL, MENU, ALT, SPACE, FN, LEFT, DOWN, RIGHT placeholders)
};

// Special key codes for non-ASCII keys (prefixed to avoid Arduino conflicts)
#define KBDKEY_INS     0x90
#define KBDKEY_CAPS    0x91
#define KBDKEY_SHIFT   0x92
#define KBDKEY_UP      0x93
#define KBDKEY_DOWN    0x94
#define KBDKEY_LEFT    0x95
#define KBDKEY_RIGHT   0x96
#define KBDKEY_CTRL    0x97
#define KBDKEY_MENU    0x98
#define KBDKEY_ALT     0x99
#define KBDKEY_FN      0x9A

GPIOCard::GPIOCard() {
  this->reset();
}

void GPIOCard::reset() {
  // Reset all VIA registers
  regORB = 0x00;
  regORA = 0x00;
  regDDRB = 0x00;
  regDDRA = 0x00;
  regT1C = 0xFFFF;
  regT1L = 0xFFFF;
  regT2C = 0xFFFF;
  regT2L = 0xFF;
  regSR = 0x00;
  regACR = 0x00;
  regPCR = 0x00;
  regIFR = 0x00;
  regIER = 0x00;
  
  // Reset control lines
  CA1 = false;
  CA2 = false;
  CB1 = false;
  CB2 = false;
  
  // Reset timer states
  T1_running = false;
  T2_running = false;
  T1_IRQ_enabled = false;
  T2_IRQ_enabled = false;
  
  // Reset keyboard state
  for (int i = 0; i < 8; i++) {
    keyboardMatrix[i] = 0x00;
  }
  keyboardASCII_A = 0x00;
  keyboardASCII_B = 0x00;
  keyboardData_A_ready = false;
  keyboardData_B_ready = false;
  keyboardEncoder_enabled = true;  // Enabled by default (CB2 LOW)
  ps2_enabled = true;              // Enabled by default (CA2 LOW)
  
  // Reset joystick
  joystickButtons = 0x00;
  
  // Reset timing
  tickCounter = 0;
  ticksPerMicrosecond = 1;
}

uint8_t GPIOCard::read(uint16_t address) {
  uint8_t reg = address & 0x0F;
  uint8_t value = 0x00;
  
  switch (reg) {
    case VIA_ORB:
      // Reading ORB clears CB1 and CB2 interrupt flags
      clearIRQFlag(IRQ_CB1 | IRQ_CB2);
      value = readPortB();
      // Clear keyboard encoder data ready flag after reading
      keyboardData_B_ready = false;
      break;
      
    case VIA_ORA:
      // Reading ORA clears CA1 and CA2 interrupt flags
      clearIRQFlag(IRQ_CA1 | IRQ_CA2);
      value = readPortA();
      // Clear PS/2 data ready flag after reading
      keyboardData_A_ready = false;
      break;
      
    case VIA_DDRB:
      value = regDDRB;
      break;
      
    case VIA_DDRA:
      value = regDDRA;
      break;
      
    case VIA_T1CL:
      // Reading T1CL clears T1 interrupt flag
      clearIRQFlag(IRQ_T1);
      value = regT1C & 0xFF;
      break;
      
    case VIA_T1CH:
      value = (regT1C >> 8) & 0xFF;
      break;
      
    case VIA_T1LL:
      value = regT1L & 0xFF;
      break;
      
    case VIA_T1LH:
      value = (regT1L >> 8) & 0xFF;
      break;
      
    case VIA_T2CL:
      // Reading T2CL clears T2 interrupt flag
      clearIRQFlag(IRQ_T2);
      value = regT2C & 0xFF;
      break;
      
    case VIA_T2CH:
      value = (regT2C >> 8) & 0xFF;
      break;
      
    case VIA_SR:
      // Reading SR clears SR interrupt flag
      clearIRQFlag(IRQ_SR);
      value = regSR;
      break;
      
    case VIA_ACR:
      value = regACR;
      break;
      
    case VIA_PCR:
      value = regPCR;
      break;
      
    case VIA_IFR:
      value = regIFR;
      // Bit 7 is set if any enabled interrupt is active
      if (regIFR & regIER & 0x7F) {
        value |= IRQ_IRQ;
      }
      break;
      
    case VIA_IER:
      value = regIER | 0x80;  // Bit 7 always reads as 1
      break;
      
    case VIA_ORA_NH:
      // Reading ORA without handshake (no interrupt flag clearing)
      value = readPortA();
      break;
  }
  
  return value;
}

void GPIOCard::write(uint16_t address, uint8_t value) {
  uint8_t reg = address & 0x0F;
  
  switch (reg) {
    case VIA_ORB:
      // Writing ORB clears CB1 and CB2 interrupt flags
      clearIRQFlag(IRQ_CB1 | IRQ_CB2);
      regORB = value;
      writePortB(value);
      break;
      
    case VIA_ORA:
      // Writing ORA clears CA1 and CA2 interrupt flags
      clearIRQFlag(IRQ_CA1 | IRQ_CA2);
      regORA = value;
      writePortA(value);
      break;
      
    case VIA_DDRB:
      regDDRB = value;
      break;
      
    case VIA_DDRA:
      regDDRA = value;
      break;
      
    case VIA_T1CL:
    case VIA_T1LL:
      // Write to T1 low latch
      regT1L = (regT1L & 0xFF00) | value;
      break;
      
    case VIA_T1CH:
      // Write to T1 high counter - loads latch into counter and starts timer
      regT1L = (regT1L & 0x00FF) | (value << 8);
      regT1C = regT1L;
      clearIRQFlag(IRQ_T1);
      T1_running = true;
      break;
      
    case VIA_T1LH:
      // Write to T1 high latch
      regT1L = (regT1L & 0x00FF) | (value << 8);
      clearIRQFlag(IRQ_T1);
      break;
      
    case VIA_T2CL:
      // Write to T2 low latch
      regT2L = value;
      break;
      
    case VIA_T2CH:
      // Write to T2 high counter - loads latch into counter and starts timer
      regT2C = (value << 8) | regT2L;
      clearIRQFlag(IRQ_T2);
      T2_running = true;
      break;
      
    case VIA_SR:
      regSR = value;
      clearIRQFlag(IRQ_SR);
      break;
      
    case VIA_ACR:
      regACR = value;
      // ACR controls timer modes, shift register, and latching
      break;
      
    case VIA_PCR:
      regPCR = value;
      // PCR controls CA1, CA2, CB1, CB2 behavior
      updateCA2();
      updateCB2();
      break;
      
    case VIA_IFR:
      // Writing to IFR clears the corresponding interrupt flags
      regIFR &= ~(value & 0x7F);
      updateIRQ();
      break;
      
    case VIA_IER:
      // Bit 7 determines set (1) or clear (0)
      if (value & 0x80) {
        regIER |= (value & 0x7F);
      } else {
        regIER &= ~(value & 0x7F);
      }
      updateIRQ();
      break;
      
    case VIA_ORA_NH:
      // Writing ORA without handshake (no interrupt flag clearing)
      regORA = value;
      writePortA(value);
      break;
  }
}

uint8_t GPIOCard::tick(uint32_t cpuFrequency) {
  tickCounter++;
  
  // Update Timer 1
  if (T1_running && regT1C > 0) {
    regT1C--;
    if (regT1C == 0) {
      setIRQFlag(IRQ_T1);
      
      // Check if timer is in free-run mode (ACR bit 6)
      if (regACR & 0x40) {
        regT1C = regT1L;  // Reload from latch
      } else {
        T1_running = false;
      }
      
      // Toggle PB7 if enabled (ACR bit 7)
      if (regACR & 0x80) {
        regORB ^= 0x80;
      }
    }
  }
  
  // Update Timer 2
  if (T2_running && regT2C > 0) {
    regT2C--;
    if (regT2C == 0) {
      setIRQFlag(IRQ_T2);
      T2_running = false;
    }
  }
  
  // Return IRQ status (bit 7 of IFR)
  return (regIFR & regIER & 0x7F) ? IRQ_IRQ : 0x00;
}

void GPIOCard::updateIRQ() {
  // Update bit 7 of IFR based on enabled interrupts
  if (regIFR & regIER & 0x7F) {
    regIFR |= IRQ_IRQ;
  } else {
    regIFR &= ~IRQ_IRQ;
  }
}

void GPIOCard::setIRQFlag(uint8_t flag) {
  regIFR |= flag;
  updateIRQ();
}

void GPIOCard::clearIRQFlag(uint8_t flag) {
  regIFR &= ~flag;
  updateIRQ();
}

uint8_t GPIOCard::readPortA() {
  uint8_t value = 0xFF;
  
  // Determine input sources
  uint8_t externalInput = 0xFF;
  
  // Port A serves multiple functions:
  // 1. PS/2 keyboard ASCII input (when encoder enabled via CA2)
  // 2. Joystick input
  // 3. Keyboard matrix row input (when manually scanning)
  
  if (ps2_enabled && keyboardData_A_ready) {
    // PS/2 keyboard data takes priority
    externalInput = keyboardASCII_A;
  } else {
    // Start with joystick data (active-low, so invert)
    externalInput = ~joystickButtons;
    
    // If keyboard encoder is disabled, overlay manual matrix scanning
    if (!keyboardEncoder_enabled) {
      // Check keyboard matrix based on Port B column selection
      for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
          // If column is selected (low) on Port B and key is pressed
          if (!(regORB & (1 << col)) && (keyboardMatrix[row] & (1 << col))) {
            externalInput &= ~(1 << row);  // Pull row low
          }
        }
      }
    }
  }
  
  // Apply DDR settings: output bits come from ORA, input bits from external
  for (int bit = 0; bit < 8; bit++) {
    if (regDDRA & (1 << bit)) {
      // Output mode - read from register
      if (regORA & (1 << bit)) {
        value |= (1 << bit);
      } else {
        value &= ~(1 << bit);
      }
    } else {
      // Input mode - read from external source
      if (externalInput & (1 << bit)) {
        value |= (1 << bit);
      } else {
        value &= ~(1 << bit);
      }
    }
  }
  
  return value;
}

uint8_t GPIOCard::readPortB() {
  uint8_t value = 0xFF;
  
  // Determine input sources
  uint8_t externalInput = 0xFF;
  
  // Port B serves multiple functions:
  // 1. Keyboard encoder ASCII input (when encoder enabled via CB2)
  // 2. Keyboard matrix column input (when manually scanning)
  // 3. Joystick input
  
  if (keyboardEncoder_enabled && keyboardData_B_ready) {
    // Keyboard encoder ASCII data takes priority
    externalInput = keyboardASCII_B;
  } else if (!keyboardEncoder_enabled) {
    // Manual keyboard matrix scanning mode
    // In this mode, software writes to Port B to select columns
    // and reads Port A for rows. Port B typically reads as 0xFF
    externalInput = 0xFF;
  } else {
    // Read joystick state (active-low, so invert)
    externalInput = ~joystickButtons;
  }
  
  // Apply DDR settings: output bits come from ORB, input bits from external
  for (int bit = 0; bit < 8; bit++) {
    if (regDDRB & (1 << bit)) {
      // Output mode - read from register
      if (regORB & (1 << bit)) {
        value |= (1 << bit);
      } else {
        value &= ~(1 << bit);
      }
    } else {
      // Input mode - read from external source
      if (externalInput & (1 << bit)) {
        value |= (1 << bit);
      } else {
        value &= ~(1 << bit);
      }
    }
  }
  
  return value;
}

void GPIOCard::writePortA(uint8_t value) {
  // Port A outputs
  // In manual keyboard scanning mode, Port A may be used to select rows
  // Most commonly Port A is inputs for PS/2 and joystick
}

void GPIOCard::writePortB(uint8_t value) {
  // Port B outputs
  // In manual keyboard scanning mode, Port B is used to select columns
  // The value written selects which column(s) to scan
}

void GPIOCard::updateCA2() {
  // CA2 control based on PCR bits 1-3
  uint8_t ca2_control = (regPCR >> 1) & 0x07;
  
  switch (ca2_control) {
    case 0x00:  // Input mode - negative edge
    case 0x01:  // Independent interrupt input - negative edge
    case 0x02:  // Input mode - positive edge
    case 0x03:  // Independent interrupt input - positive edge
      // Input modes
      ps2_enabled = true;
      break;
      
    case 0x04:  // Handshake output
    case 0x05:  // Pulse output
      // Output modes
      ps2_enabled = false;
      break;
      
    case 0x06:  // Manual output LOW
      CA2 = false;
      ps2_enabled = true;  // LOW enables PS/2
      break;
      
    case 0x07:  // Manual output HIGH
      CA2 = true;
      ps2_enabled = false;  // HIGH disables PS/2
      break;
  }
}

void GPIOCard::updateCB2() {
  // CB2 control based on PCR bits 5-7
  uint8_t cb2_control = (regPCR >> 5) & 0x07;
  
  switch (cb2_control) {
    case 0x00:  // Input mode - negative edge
    case 0x01:  // Independent interrupt input - negative edge
    case 0x02:  // Input mode - positive edge
    case 0x03:  // Independent interrupt input - positive edge
      // Input modes
      keyboardEncoder_enabled = true;
      break;
      
    case 0x04:  // Handshake output
    case 0x05:  // Pulse output
      // Output modes
      keyboardEncoder_enabled = false;
      break;
      
    case 0x06:  // Manual output LOW
      CB2 = false;
      keyboardEncoder_enabled = true;  // LOW enables keyboard encoder
      break;
      
    case 0x07:  // Manual output HIGH
      CB2 = true;
      keyboardEncoder_enabled = false;  // HIGH disables keyboard encoder
      break;
  }
}

void GPIOCard::updateKeyboard(uint8_t key) {
  // This handles keyboard matrix updates
  // Convert ASCII key to matrix position
  
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if (KEYBOARD_MATRIX[row][col] == key) {
        // Set the bit in the keyboard matrix
        keyboardMatrix[row] |= (1 << col);
        
        // If keyboard encoder is enabled, send ASCII to Port B
        if (keyboardEncoder_enabled) {
          keyboardASCII_B = key;
          keyboardData_B_ready = true;
          
          // Generate CB1 interrupt to signal data ready
          setIRQFlag(IRQ_CB1);
        }
        
        return;
      }
    }
  }
}

void GPIOCard::updatePS2Keyboard(uint8_t ascii) {
  // Update PS/2 keyboard input on Port A
  if (ps2_enabled) {
    keyboardASCII_A = ascii;
    keyboardData_A_ready = true;
    
    // Generate CA1 interrupt to signal data ready
    setIRQFlag(IRQ_CA1);
  }
}

void GPIOCard::releaseKey(uint8_t key) {
  // Clear the key from the keyboard matrix
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if (KEYBOARD_MATRIX[row][col] == key) {
        keyboardMatrix[row] &= ~(1 << col);
        return;
      }
    }
  }
}

void GPIOCard::updateJoystick(uint8_t buttons) {
  // Update joystick button state
  joystickButtons = buttons;
}