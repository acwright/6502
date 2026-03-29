#ifndef _GPIO_KEYBOARD_ENCODER_ATTACHMENT_H
#define _GPIO_KEYBOARD_ENCODER_ATTACHMENT_H

#include "GPIOAttachment.h"

/**
 * @brief Keyboard Encoder Attachment for GPIO Card
 * 
 * Translates USB HID key events into ASCII bytes. Only key press events
 * produce output — releases are used solely to track modifier state.
 * Letters are always uppercase (0x41-0x5A). Only Shift and Ctrl modifiers
 * are recognized; all others (Alt, Caps Lock, GUI/Menu) are ignored.
 * 
 * Modifier priority: Ctrl > Shift > Base.
 * Shift only affects number and symbol keys (not letters).
 * 
 * Port A (PS/2 Keyboard Interface):
 * - Port A: ASCII character output when enabled
 * - CA2: Enable/disable control (LOW = enabled, HIGH = disabled)
 * - CA1: Data ready interrupt (triggered when new character available)
 * 
 * Port B (Keyboard Matrix Encoder):
 * - Port B: ASCII character output when enabled
 * - CB2: Enable/disable control (LOW = enabled, HIGH = disabled)
 * - CB1: Data ready interrupt (triggered when new character available)
 * 
 * When a key is pressed, the ASCII value is latched and an interrupt
 * is generated on the appropriate port. Reading the port clears the data-ready flag.
 * There is no input buffer — only the most recent keypress is available.
 */
class GPIOKeyboardEncoderAttachment : public GPIOAttachment {
  private:
    // Port A data
    uint8_t asciiDataA;      // Buffered ASCII character for Port A
    bool dataReadyA;         // Data ready flag for Port A (triggers CA1 interrupt)
    bool interruptPendingA;  // CA1 interrupt pending
    bool enabledA;           // Port A enabled state
    
    // Port B data
    uint8_t asciiDataB;      // Buffered ASCII character for Port B
    bool dataReadyB;         // Data ready flag for Port B (triggers CB1 interrupt)
    bool interruptPendingB;  // CB1 interrupt pending
    bool enabledB;           // Port B enabled state
    
    // Modifier key states
    bool shiftPressed;       // Shift modifier active
    bool ctrlPressed;        // Ctrl modifier active
    
    /**
     * @brief Map USB HID keycode with modifiers to ASCII value
     * 
     * Priority: Ctrl > Shift > Base.
     * Letters are always uppercase. Shift only affects numbers/symbols.
     * 
     * @param usbHidKeycode USB HID keycode
     * @return uint8_t Mapped ASCII value (0x00 for unmapped or Ctrl+2)
     */
    uint8_t mapKeyWithModifiers(uint8_t usbHidKeycode);
    
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
     * @brief Read Port A - returns ASCII data when enabled and data ready
     * 
     * @param ddrA Data Direction Register A
     * @param orA Output Register A
     * @return uint8_t ASCII character or 0xFF if no data
     */
    uint8_t readPortA(uint8_t ddrA, uint8_t orA) override;
    
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
     * CA2 LOW = Port A enabled, CA2 HIGH = Port A disabled
     * CB2 LOW = Port B enabled, CB2 HIGH = Port B disabled
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
     * @return true if Port A data ready interrupt is pending
     */
    bool hasCA1Interrupt() const override;
    
    /**
     * @brief Check if CB1 interrupt is pending
     * 
     * @return true if Port B data ready interrupt is pending
     */
    bool hasCB1Interrupt() const override;
    
    /**
     * @brief Clear interrupt flags
     * 
     * Called when a port is read to clear the data-ready interrupt.
     * 
     * @param ca1 Clear CA1 interrupt
     * @param ca2 Clear CA2 interrupt
     * @param cb1 Clear CB1 interrupt
     * @param cb2 Clear CB2 interrupt
     */
    void clearInterrupts(bool ca1, bool ca2, bool cb1, bool cb2) override;
    
    /**
     * @brief Update Port A (PS/2 keyboard interface) with a key event
     * 
     * Routes a keystroke to Port A only. Use for a physical PS/2 keyboard source.
     * 
     * @param usbHidKeycode USB HID keycode
     * @param pressed True if key is pressed, false if released
     */
    void updateKeyPortA(uint8_t usbHidKeycode, bool pressed);

    /**
     * @brief Update Port B (matrix keyboard encoder) with a key event
     * 
     * Routes a keystroke to Port B only. Use for USB or browser keyboard sources.
     * 
     * @param usbHidKeycode USB HID keycode
     * @param pressed True if key is pressed, false if released
     */
    void updateKeyPortB(uint8_t usbHidKeycode, bool pressed);

    /**
     * @brief Update keyboard with new key event (both ports)
     * 
     * Converts USB HID keycode to ASCII and buffers it on both Port A and Port B.
     * Use this when the same keyboard source should feed both ports simultaneously
     * (e.g. a device acting as both PS/2 and matrix encoder).
     * For source-specific routing use updateKeyPortA() or updateKeyPortB().
     * 
     * @param usbHidKeycode USB HID keycode (raw keycode from USB keyboard)
     * @param pressed True if key is pressed, false if released
     */
    void updateKey(uint8_t usbHidKeycode, bool pressed);
    
    /**
     * @brief Check if Port A has data ready
     * 
     * @return true if ASCII data is buffered and ready to read on Port A
     */
    bool hasDataReadyA() const;
    
    /**
     * @brief Check if Port B has data ready
     * 
     * @return true if ASCII data is buffered and ready to read on Port B
     */
    bool hasDataReadyB() const;
};

#endif // _GPIO_KEYBOARD_ENCODER_ATTACHMENT_H
