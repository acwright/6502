#include <Arduino.h>
#include <DOBTerminal.h>
#include <SPI.h>
#include <ILI9341_t3n.h>

void onCommand(char command);

void reset();
void bell(uint8_t action);
void backspace();
void tab();
void lineFeed();
void carriageReturn();
void deleteTo(uint8_t destination);
void scroll(uint8_t direction);
void cursor(uint8_t direction);
void del();
void data(char command);
void insertTextData(uint8_t data);
void insertGraphicsData(uint8_t data);
void drawCursor(uint8_t* renderBuffer);
void render();

void copyCharacterCell(uint8_t sourceCol, uint8_t sourceRow, uint8_t destCol, uint8_t destRow);
void clearCharacterCell(uint8_t col, uint8_t row);

void initPins();
void initBuffer();
void initTFT();

uint8_t buffer[WIDTH * HEIGHT];

uint8_t mode = MODE_TEXT;

uint8_t column = 0;
uint8_t row = 0;
uint8_t offset = 0;
bool columnNextByte = false;
bool rowNextByte = false;

uint8_t cursorChar = 0x00; // OFF
uint8_t cursorMode = CURSOR_SOLID;
bool cursorCharNextByte = false;
time_t cursorBlinkTime = 0;
bool cursorVisible = false;
uint cursorBlinkInterval = 500;

uint8_t bellDuration = 0x3C; // Duration in jiffies (1/60th of a second) (Default: 1 second)
uint8_t bellFrequency = 0x3D; // Frequency value (Default: C6) **NOT USED**
bool bellDurationNextByte = false;
bool bellFrequencyNextByte = false;
time_t bellEnd = 0;

bool dataNextByte = false;

uint8_t foregroundColor = 0xFF; // White
uint8_t backgroundColor = 0x00; // Black
bool foregroundColorNextByte = false;
bool backgroundColorNextByte = false;

time_t lastRenderTime = 0;
uint8_t targetFrameTime = 17;  // 60fps = ~16.67ms per frame so 17ms

ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RESET);

//
// MAIN
//

void setup() {
  initPins();
  initTFT();
  initBuffer();

  Serial.begin(115200);
  Serial1.begin(115200);
}

void loop() {
  if (millis() >= bellEnd) {
    bell(BELL_END);
  }

  if (Serial.available()) {
    onCommand(Serial.read());
  }
  if (Serial1.available()) {
    onCommand(Serial1.read());
  }

  // Render at approximately 60fps
  time_t now = millis();
  if (now - lastRenderTime >= targetFrameTime) {
    render();
  }
}

//
// RENDER
//

// https://github.com/KurtE/ILI9341_t3n
// https://forum.pjrc.com/index.php?threads/highly-optimized-ili9341-320x240-tft-color-display-library.26305/

void render() {
  time_t now = millis();
  time_t elapsed = now - lastRenderTime;
  
  // Update cursor blink state
  if (cursorMode == CURSOR_BLINKING) {
    cursorBlinkTime += elapsed;
    if (cursorBlinkTime >= cursorBlinkInterval) {
      cursorVisible = !cursorVisible;
      cursorBlinkTime = 0;
    }
  } else {
    cursorVisible = true;
  }
  
  // Create temporary render buffer
  uint8_t renderBuffer[WIDTH * HEIGHT];
  memcpy(renderBuffer, buffer, WIDTH * HEIGHT);
  
  // Draw cursor if visible and cursorChar is not 0x00 (OFF)
  if (cursorVisible && cursorChar != 0x00) {
    drawCursor(renderBuffer);
  }
  
  // TODO: Actual rendering to display hardware will go here
  // e.g., send renderBuffer to VGA/LCD controller
  
  lastRenderTime = now;
}

void drawCursor(uint8_t* renderBuffer) {
  const uint8_t* character = CHARACTERS[cursorChar];
  uint16_t startRow = row * 8;
  uint16_t startColumn = column * 8;
  
  // Draw cursor character with inverted colors
  for (uint8_t y = 0; y < 8; y++) {
    uint8_t rowByte = character[y];
    uint16_t bufferRowStart = (startRow + y) * WIDTH;
    
    for (uint8_t x = 0; x < 8; x++) {
      uint8_t bit = (rowByte >> (7 - x)) & 1;
      // Invert the colors: if bit is 1, use background; if 0, use foreground
      uint8_t color = bit ? backgroundColor : foregroundColor;
      renderBuffer[bufferRowStart + startColumn + x] = color;
    }
  }
}

//
// EVENTS
//

void onCommand(char command) {
  if (cursorCharNextByte) {
    cursorChar = command;
    cursorCharNextByte = false;
    return;
  }

  if (columnNextByte) {
    column = command;
    columnNextByte = false;
    return;
  }
  if (rowNextByte) {
    row = command;
    rowNextByte = false;
    return;
  }

  if (bellDurationNextByte) {
    bellDuration = command;
    bellDurationNextByte = false;
    return;
  }
  if (bellFrequencyNextByte) {
    bellFrequency = command;
    bellFrequencyNextByte = false;
    return;
  }

  if (foregroundColorNextByte) {
    foregroundColor = command;
    foregroundColorNextByte = false;
    return;
  }
  if (backgroundColorNextByte) {
    backgroundColor = command;
    backgroundColorNextByte = false;
    return;
  }

  if (dataNextByte) {
    data(command);
    dataNextByte = false;
    return;
  }

  switch (command) {
    case 0x00: // NULL
      break;
    case 0x01: // CURSOR HOME
      column = 0;
      row = 0;
      offset = 0;
      break;
    case 0x02: // CURSOR CHARACTER
      cursorCharNextByte = true;
      break;
    case 0x03: // CURSOR BLINKING
      cursorMode == CURSOR_SOLID ? cursorMode = CURSOR_BLINKING : cursorMode = CURSOR_SOLID;
      break;
    case 0x04: /// RESET
      reset();
      break;
    case 0x05: // BELL DURATION
      bellDurationNextByte = true;
      break;
    case 0x06: // BELL FREQUENCY
      bellFrequencyNextByte = true;
      break;
    case 0x07: // BELL
      bell(BELL_START);
      break;
    case 0x08: // BACKSPACE
      backspace();
      break;
    case 0x09: // TAB
      tab();
      break;
    case 0x0A: // LINE FEED
      lineFeed();
      break;
    case 0x0B: // SCREEN MODE
      mode == MODE_TEXT ? mode = MODE_GRAPHICS : mode = MODE_TEXT;
      break;
    case 0x0C: // CLEAR SCREEN
      for (uint16_t i = 0; i < WIDTH * HEIGHT; i++) {
        buffer[i] = backgroundColor; 
      }
      break;
    case 0x0D: // CARRIAGE RETURN
      carriageReturn();
      break;
    case 0x0E: // SET COLUMN
      columnNextByte = true;
      break;
    case 0x0F: // SET ROW
      rowNextByte = true;
      break;
    case 0x10: // DELETE TO START OF LINE
      deleteTo(START_OF_LINE);
      break;
    case 0x11: // DELETE TO END OF LINE
      deleteTo(END_OF_LINE);
      break;
    case 0x12: // DELETE TO START OF SCREEN
      deleteTo(START_OF_SCREEN);
      break;
    case 0x13: // DELETE TO END OF SCREEN
      deleteTo(END_OF_SCREEN);
      break;
    case 0x14: // SCROLL LEFT
      scroll(LEFT);
      break;
    case 0x15: // SCROLL RIGHT
      scroll(RIGHT);
      break;
    case 0x16: // SCROLL UP
      scroll(UP);
      break;
    case 0x17: // SCROLL DOWN
      scroll(DOWN);
      break;
    case 0x18: // FOREGROUND COLOR
      foregroundColorNextByte = true;
      break;
    case 0x19: // BACKGROUND COLOR
      backgroundColorNextByte = true;
      break;
    case 0x1A: // NEXT BYTE AS DATA
      dataNextByte = true;
      break;
    case 0x1B: // ESCAPE
      // Reserved for future ANSI escape code handling
      break;
    case 0x1C: // CURSOR LEFT
      cursor(LEFT);
      break;
    case 0x1D: // CURSOR RIGHT
      cursor(RIGHT);
      break;
    case 0x1E: // CURSOR UP
      cursor(UP);
      break;
    case 0x1F: // CURSOR DOWN
      cursor(DOWN);
      break;
    case 0x7F: // DELETE
      del();
    default:
      if (command >= 0x20 && command <= 0x7E) { // ASCII CHARACTERS
        data(command);
      }
      if (command >= 0x80 && command <= 0xFF) { /// EXTENDED ASCII CHARACTERS
        data(command);
      }
      break;
  }
}

void reset() {
  column = 0;
  row = 0;
  offset = 0;
  mode = MODE_TEXT;
  cursorChar = 0x00;
  cursorMode = CURSOR_SOLID;
  backgroundColor = 0x00;
  foregroundColor = 0xFF;
  bellDuration = 0x3C;
  bellFrequency = 0x3D;
  bellEnd = 0;
  columnNextByte = false;
  rowNextByte = false;
  cursorCharNextByte = false;
  foregroundColorNextByte = false;
  backgroundColorNextByte = false;
  bellDurationNextByte = false;
  bellFrequencyNextByte = false;

  for (uint16_t i = 0; i < WIDTH * HEIGHT; i++) {
    buffer[i] = backgroundColor; 
  }
}

void bell(uint8_t action) {
  if (action == BELL_START) {
    bellEnd = millis() + (bellDuration * (1000 / 60));
    digitalWriteFast(BELL, HIGH);
  }
  if (action == BELL_END) {
    digitalWriteFast(BELL, LOW);
  }
}

void backspace() {
  if (column > 0) {
    column--;
  }
  offset = 0;
}

void tab() {
  column = min((column / 4 + 1) * 4, COLUMNS - 1);
  offset = 0;
}

void lineFeed() {
  uint8_t nextRow = row + 1;

  if (nextRow > ROWS) {
    nextRow = ROWS - 1;
    scroll(UP);
  }

  row = nextRow;
  offset = 0;
}

void carriageReturn() {
  column = 0;
  offset = 0;
}

void deleteTo(uint8_t destination) {
  switch (destination) {
    case START_OF_LINE:
      // Clear from start of current line to current cursor position
      for (uint8_t col = 0; col <= column; col++) {
        clearCharacterCell(col, row);
      }
      break;
    case END_OF_LINE:
      // Clear from current cursor position to end of current line
      for (uint8_t col = column; col < COLUMNS; col++) {
        clearCharacterCell(col, row);
      }
      break;
    case START_OF_SCREEN:
      // Clear from start of screen to current cursor position
      for (uint8_t r = 0; r < row; r++) {
        for (uint8_t col = 0; col < COLUMNS; col++) {
          clearCharacterCell(col, r);
        }
      }
      // Clear partial line up to and including cursor
      for (uint8_t col = 0; col <= column; col++) {
        clearCharacterCell(col, row);
      }
      break;
    case END_OF_SCREEN:
      // Clear from current cursor position to end of screen
      for (uint8_t col = column; col < COLUMNS; col++) {
        clearCharacterCell(col, row);
      }
      // Clear remaining lines
      for (uint8_t r = row + 1; r < ROWS; r++) {
        for (uint8_t col = 0; col < COLUMNS; col++) {
          clearCharacterCell(col, r);
        }
      }
      break;
  }
}

void scroll(uint8_t direction) {
  switch (direction) {
    case LEFT:
      // Shift all character cells one position to the left
      for (uint8_t r = 0; r < ROWS; r++) {
        for (uint8_t col = 0; col < COLUMNS - 1; col++) {
          copyCharacterCell(col + 1, r, col, r);
        }
        // Fill rightmost column with background color
        clearCharacterCell(COLUMNS - 1, r);
      }
      break;
    case RIGHT:
      // Shift all character cells one position to the right
      for (uint8_t r = 0; r < ROWS; r++) {
        for (uint8_t col = COLUMNS - 1; col > 0; col--) {
          copyCharacterCell(col - 1, r, col, r);
        }
        // Fill leftmost column with background color
        clearCharacterCell(0, r);
      }
      break;
    case UP:
      // Shift all character cells one position up
      for (uint8_t r = 0; r < ROWS - 1; r++) {
        for (uint8_t col = 0; col < COLUMNS; col++) {
          copyCharacterCell(col, r + 1, col, r);
        }
      }
      // Fill bottom row with background color
      for (uint8_t col = 0; col < COLUMNS; col++) {
        clearCharacterCell(col, ROWS - 1);
      }
      break;
    case DOWN:
      // Shift all character cells one position down
      for (uint8_t r = ROWS - 1; r > 0; r--) {
        for (uint8_t col = 0; col < COLUMNS; col++) {
          copyCharacterCell(col, r - 1, col, r);
        }
      }
      // Fill top row with background color
      for (uint8_t col = 0; col < COLUMNS; col++) {
        clearCharacterCell(col, 0);
      }
      break;
  }
}

void cursor(uint8_t direction) {
  switch (direction) {
    case LEFT:
      if (column > 0) {
        column--;
      }
      break;
    case RIGHT:
      if (column < COLUMNS - 1) {
        column++;
      }
      break;
    case UP:
      if (row > 0) {
        row--;
      }
      break;
    case DOWN:
      if (row < ROWS - 1) {
        row++;
      }
      break;
  }
  offset = 0;
}

void del() {
  clearCharacterCell(column, row);
}

void data(char command) {
  switch (mode) {
    case MODE_TEXT:
      insertTextData(command);
      break;
    case MODE_GRAPHICS:
      insertGraphicsData(command);
      break;
  }
}

//
// INSERT
//

void insertTextData(uint8_t data) {
  const uint8_t* character = CHARACTERS[data];
  uint16_t startRow = row * 8;
  uint16_t startColumn = column * 8;
  
  // Render 8x8 character bitmap
  for (uint8_t y = 0; y < 8; y++) {
    uint8_t rowByte = character[y];
    uint16_t bufferRowStart = (startRow + y) * WIDTH;
    
    for (uint8_t x = 0; x < 8; x++) {
      uint8_t bit = (rowByte >> (7 - x)) & 1;
      uint8_t color = bit ? foregroundColor : backgroundColor;
      buffer[bufferRowStart + startColumn + x] = color;
    }
  }
  
  // Move to next character position
  column++;
  if (column >= COLUMNS) {
    column = 0;
    row++;
    if (row >= ROWS) {
      row = ROWS - 1;
    }
  }
  offset = 0;
}

void insertGraphicsData(uint8_t data) {
  // Calculate starting pixel position (offset is the row within the 8x8 block)
  uint16_t pixelRow = row * 8 + offset;
  uint16_t startColumn = column * 8;
  uint16_t bufferRowStart = pixelRow * WIDTH;
  
  // Write 8 pixels horizontally - each bit in data represents one pixel
  // Bit 7 (MSB) is leftmost pixel, bit 0 (LSB) is rightmost pixel
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t bit = (data >> (7 - i)) & 1;
    uint8_t color = bit ? foregroundColor : backgroundColor;
    buffer[bufferRowStart + startColumn + i] = color;
  }
  
  // Move to next location in pixel array
  offset++;
  if (offset >= 8) {
    offset = 0;
    column++;
    if (column >= COLUMNS) {
      column = 0;
      row++;
      if (row >= ROWS) {
        row = 0;
      }
    }
  }
}

//
// HELPERS
//

void copyCharacterCell(uint8_t sourceCol, uint8_t sourceRow, uint8_t destCol, uint8_t destRow) {
  uint16_t sourceStartRow = sourceRow * 8;
  uint16_t sourceStartColumn = sourceCol * 8;
  uint16_t destStartRow = destRow * 8;
  uint16_t destStartColumn = destCol * 8;
  
  // Copy 8x8 pixel block
  for (uint8_t y = 0; y < 8; y++) {
    uint16_t sourceBufferRowStart = (sourceStartRow + y) * WIDTH;
    uint16_t destBufferRowStart = (destStartRow + y) * WIDTH;
    for (uint8_t x = 0; x < 8; x++) {
      buffer[destBufferRowStart + destStartColumn + x] = 
        buffer[sourceBufferRowStart + sourceStartColumn + x];
    }
  }
}

void clearCharacterCell(uint8_t col, uint8_t row) {
  uint16_t startRow = row * 8;
  uint16_t startColumn = col * 8;
  
  // Clear 8x8 pixel block with background color
  for (uint8_t y = 0; y < 8; y++) {
    uint16_t bufferRowStart = (startRow + y) * WIDTH;
    for (uint8_t x = 0; x < 8; x++) {
      buffer[bufferRowStart + startColumn + x] = backgroundColor;
    }
  }
}

//
// INITIALIZATION
//

void initPins() {
  pinMode(BELL, OUTPUT);
  pinMode(TFT_BL, OUTPUT);
  pinMode(TFT_RESET, OUTPUT);

  digitalWriteFast(BELL, LOW);
  digitalWriteFast(TFT_BL, HIGH);
  digitalWriteFast(TFT_RESET, HIGH);
}

void initBuffer() {
  for (uint16_t i = 0; i < WIDTH * HEIGHT; i++) {
    buffer[i] = backgroundColor; 
  }
}

void initTFT() {
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
}