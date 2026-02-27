#ifndef _GPIO_KEYBOARD_MATRIX_ATTACHMENT_H
#define _GPIO_KEYBOARD_MATRIX_ATTACHMENT_H

#include "GPIOAttachment.h"

/**
 * @brief Keyboard Matrix Attachment for GPIO Card
 * 
 * Emulates an 8x8 keyboard matrix connected to the GPIO ports (Keyboard Encoder Helper or PS2 Helper):
 * - Rows (PA0-PA7): Connected to Port A for reading keypresses
 * - Columns (PB0-PB7): Connected to Port B for column selection
 * 
 * The software scans the keyboard by:
 * 1. Writing to Port B to select column(s) (active-low)
 * 2. Reading Port A to check which row(s) are pressed (active-low)
 * 
 * This attachment does not use control lines and is always active when
 * Port B is in output mode and Port A is in input mode.
 */
class GPIOKeyboardMatrixAttachment : public GPIOAttachment {
  private:
    // Keyboard matrix state: 8 rows, each byte represents which columns are pressed
    // Bit 0 = column 0, bit 1 = column 1, etc.
    uint8_t keyboardMatrix[8];
    
    // Static keyboard matrix mapping (same as original GPIOCard)
    // Rows are PA0-PA7, Columns are PB0-PB7
    static const uint8_t KEYBOARD_LAYOUT[8][8];
    
    // Current column selection from Port B
    uint8_t selectedColumns;
    
  public:
    /**
     * @brief Construct a new Keyboard Matrix Attachment
     * 
     * @param priority Priority for port multiplexing (default: 10 = high priority)
     */
    GPIOKeyboardMatrixAttachment(uint8_t priority = 10);
    
    /**
     * @brief Reset keyboard matrix to unpressed state
     */
    void reset() override;
    
    /**
     * @brief Read Port A - returns keyboard row states
     * 
     * Returns the row state based on currently selected columns.
     * Each bit in the return value represents a row (0 = pressed, 1 = not pressed).
     * 
     * @param ddrA Data Direction Register A
     * @param orA Output Register A
     * @return uint8_t Row states (active-low)
     */
    uint8_t readPortA(uint8_t ddrA, uint8_t orA) override;
    
    /**
     * @brief Read Port B - returns high impedance (0xFF) in input mode
     * 
     * When Port B is in input mode, this attachment doesn't drive the port.
     * 
     * @param ddrB Data Direction Register B
     * @param orB Output Register B
     * @return uint8_t 0xFF (no data)
     */
    uint8_t readPortB(uint8_t ddrB, uint8_t orB) override;
    
    /**
     * @brief Handle Port B write - captures column selection
     * 
     * @param value Value written to Port B (column selection, active-low)
     * @param ddrB Data Direction Register B
     */
    void writePortB(uint8_t value, uint8_t ddrB) override;
    
    /**
     * @brief Update keyboard matrix when a key is pressed or released
     * 
     * @param key ASCII character corresponding to the key
     * @param pressed True if key is pressed, false if released
     */
    void updateKey(uint8_t key, bool pressed);
    
    /**
     * @brief Update keyboard matrix for a specific row/column position
     * 
     * @param row Row index (0-7)
     * @param col Column index (0-7)
     * @param pressed True if key is pressed, false if released
     */
    void updateMatrixPosition(uint8_t row, uint8_t col, bool pressed);
};

#endif // _GPIO_KEYBOARD_MATRIX_ATTACHMENT_H
