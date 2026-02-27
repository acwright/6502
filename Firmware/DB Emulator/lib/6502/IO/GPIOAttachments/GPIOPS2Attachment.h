#ifndef _GPIO_PS2_ATTACHMENT_H
#define _GPIO_PS2_ATTACHMENT_H

#include "GPIOAttachment.h"

/**
 * @brief PS/2 Keyboard Attachment for GPIO Card
 * 
 * Emulates a PS/2 keyboard interface that presents ASCII values on Port A (Keyboard Encoder Helper or Input Board).
 * The PS/2 port is controlled by CA2 (enable) and generates interrupts on CA1.
 * 
 * - Port A: ASCII character output when enabled
 * - CA2: Enable/disable control (LOW = enabled, HIGH = disabled)
 * - CA1: Data ready interrupt (triggered when new character available)
 * 
 * When a key is pressed, the ASCII value is buffered and a CA1 interrupt
 * is generated. Reading Port A clears the data-ready flag.
 */
class GPIOPS2Attachment : public GPIOAttachment {
  private:
    uint8_t asciiData;      // Buffered ASCII character
    bool dataReady;         // Data ready flag (triggers CA1 interrupt)
    bool interruptPending;  // CA1 interrupt pending
    
  public:
    /**
     * @brief Construct a new PS/2 Attachment
     * 
     * @param priority Priority for port multiplexing (default: 30)
     */
    GPIOPS2Attachment(uint8_t priority = 30);
    
    /**
     * @brief Reset PS/2 interface to initial state
     */
    void reset() override;
    
    /**
     * @brief Read Port A - returns ASCII data when enabled and data ready
     * 
     * @param ddrA Data Direction Register A
     * @param orA Output Register A
     * @return uint8_t ASCII character or 0xFF if no data
     */
    uint8_t readPortA(uint8_t ddrA, uint8_t orA) override;
    
    /**
     * @brief Update control lines - determines enabled state
     * 
     * CA2 LOW = PS/2 enabled, CA2 HIGH = PS/2 disabled
     * 
     * @param ca1 Current state of CA1 line
     * @param ca2 Current state of CA2 line
     * @param cb1 Current state of CB1 line
     * @param cb2 Current state of CB2 line
     */
    void updateControlLines(bool ca1, bool ca2, bool cb1, bool cb2) override;
    
    /**
     * @brief Check if CA1 interrupt is pending
     * 
     * @return true if data ready interrupt is pending
     */
    bool hasCA1Interrupt() const override;
    
    /**
     * @brief Clear interrupt flags
     * 
     * Called when Port A is read to clear the data-ready interrupt.
     * 
     * @param ca1 Clear CA1 interrupt
     * @param ca2 Clear CA2 interrupt
     * @param cb1 Clear CB1 interrupt
     * @param cb2 Clear CB2 interrupt
     */
    void clearInterrupts(bool ca1, bool ca2, bool cb1, bool cb2) override;
    
    /**
     * @brief Update PS/2 keyboard with new ASCII character
     * 
     * Buffers the ASCII value and triggers CA1 interrupt if enabled.
     * 
     * @param ascii ASCII character code
     */
    void updatePS2Keyboard(uint8_t ascii);
    
    /**
     * @brief Check if PS/2 has data ready
     * 
     * @return true if ASCII data is buffered and ready to read
     */
    bool hasDataReady() const;
};

#endif // _GPIO_PS2_ATTACHMENT_H