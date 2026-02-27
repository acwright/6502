#ifndef _GPIO_KEYBOARD_ENCODER_ATTACHMENT_H
#define _GPIO_KEYBOARD_ENCODER_ATTACHMENT_H

#include "GPIOAttachment.h"

/**
 * @brief Keyboard Encoder Attachment for GPIO Card
 * 
 * Emulates a keyboard encoder that presents ASCII values on Port B (Keyboard Encoder Helper).
 * The encoder is controlled by CB2 (enable) and generates interrupts on CB1.
 * 
 * - Port B: ASCII character output when enabled
 * - CB2: Enable/disable control (LOW = enabled, HIGH = disabled)
 * - CB1: Data ready interrupt (triggered when new character available)
 * 
 * When a key is pressed, the ASCII value is buffered and a CB1 interrupt
 * is generated. Reading Port B clears the data-ready flag.
 */
class GPIOKeyboardEncoderAttachment : public GPIOAttachment {
  private:
    uint8_t asciiData;      // Buffered ASCII character
    bool dataReady;         // Data ready flag (triggers CB1 interrupt)
    bool interruptPending;  // CB1 interrupt pending
    
  public:
    /**
     * @brief Construct a new Keyboard Encoder Attachment
     * 
     * @param priority Priority for port multiplexing (default: 20)
     */
    GPIOKeyboardEncoderAttachment(uint8_t priority = 20);
    
    /**
     * @brief Reset encoder to initial state
     */
    void reset() override;
    
    /**
     * @brief Read Port B - returns ASCII data when enabled and data ready
     * 
     * @param ddrB Data Direction Register B
     * @param orB Output Register B
     * @return uint8_t ASCII character or 0xFF if no data
     */
    uint8_t readPortB(uint8_t ddrB, uint8_t orB) override;
    
    /**
     * @brief Update control lines - determines enabled state
     * 
     * CB2 LOW = encoder enabled, CB2 HIGH = encoder disabled
     * 
     * @param ca1 Current state of CA1 line
     * @param ca2 Current state of CA2 line
     * @param cb1 Current state of CB1 line
     * @param cb2 Current state of CB2 line
     */
    void updateControlLines(bool ca1, bool ca2, bool cb1, bool cb2) override;
    
    /**
     * @brief Check if CB1 interrupt is pending
     * 
     * @return true if data ready interrupt is pending
     */
    bool hasCB1Interrupt() const override;
    
    /**
     * @brief Clear interrupt flags
     * 
     * Called when Port B is read to clear the data-ready interrupt.
     * 
     * @param ca1 Clear CA1 interrupt
     * @param ca2 Clear CA2 interrupt
     * @param cb1 Clear CB1 interrupt
     * @param cb2 Clear CB2 interrupt
     */
    void clearInterrupts(bool ca1, bool ca2, bool cb1, bool cb2) override;
    
    /**
     * @brief Update keyboard with new ASCII character
     * 
     * Buffers the ASCII value and triggers CB1 interrupt if enabled.
     * 
     * @param ascii ASCII character code
     */
    void updateKeyboard(uint8_t ascii);
    
    /**
     * @brief Check if encoder has data ready
     * 
     * @return true if ASCII data is buffered and ready to read
     */
    bool hasDataReady() const;
};

#endif // _GPIO_KEYBOARD_ENCODER_ATTACHMENT_H
