#ifndef _GPIO_JOYSTICK_ATTACHMENT_H
#define _GPIO_JOYSTICK_ATTACHMENT_H

#include "GPIOAttachment.h"

// Joystick button bits
// | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
// | R | L | D | U | Y | X | B | A |
#define JOY_A       0x01
#define JOY_B       0x02
#define JOY_X       0x04
#define JOY_Y       0x08
#define JOY_UP      0x10
#define JOY_DOWN    0x20
#define JOY_LEFT    0x40
#define JOY_RIGHT   0x80

/**
 * @brief Joystick Attachment for GPIO Card
 * 
 * Emulates an 8-button joystick connected to a GPIO port (Keyboard Encoder Helper or Input Board - Port B).
 * The joystick provides active-low button states (0 = pressed, 1 = not pressed).
 * 
 * Buttons are mapped as:
 * - Bit 0: A button
 * - Bit 1: B button
 * - Bit 2: X button
 * - Bit 3: Y button
 * - Bit 4: Up
 * - Bit 5: Down
 * - Bit 6: Left
 * - Bit 7: Right
 * 
 * This attachment does not use control lines and is always active.
 * It has low priority to allow keyboard/PS2 to take precedence.
 */
class GPIOJoystickAttachment : public GPIOAttachment {
  private:
    uint8_t buttonState;    // Current button state (1 = pressed, 0 = not pressed)
    bool attachedToPortA;   // True if attached to Port A, false for Port B
    
  public:
    /**
     * @brief Construct a new Joystick Attachment
     * 
     * @param attachToPortA True to attach to Port A, false for Port B (default: true)
     * @param priority Priority for port multiplexing (default: 100 = low priority)
     */
    GPIOJoystickAttachment(bool attachToPortA = true, uint8_t priority = 100);
    
    /**
     * @brief Reset joystick to initial state (no buttons pressed)
     */
    void reset() override;
    
    /**
     * @brief Read Port A - returns joystick state if attached to Port A
     * 
     * Returns active-low button states (inverted).
     * 
     * @param ddrA Data Direction Register A
     * @param orA Output Register A
     * @return uint8_t Button states (active-low) or 0xFF if not attached to Port A
     */
    uint8_t readPortA(uint8_t ddrA, uint8_t orA) override;
    
    /**
     * @brief Read Port B - returns joystick state if attached to Port B
     * 
     * Returns active-low button states (inverted).
     * 
     * @param ddrB Data Direction Register B
     * @param orB Output Register B
     * @return uint8_t Button states (active-low) or 0xFF if not attached to Port B
     */
    uint8_t readPortB(uint8_t ddrB, uint8_t orB) override;
    
    /**
     * @brief Update joystick button state
     * 
     * @param buttons Button state (1 = pressed, 0 = not pressed)
     */
    void updateJoystick(uint8_t buttons);
    
    /**
     * @brief Get current button state
     * 
     * @return uint8_t Button state (1 = pressed, 0 = not pressed)
     */
    uint8_t getButtonState() const;
    
    /**
     * @brief Check if specific button is pressed
     * 
     * @param button Button mask (e.g., JOY_A, JOY_UP)
     * @return true if button is pressed
     */
    bool isButtonPressed(uint8_t button) const;
};

#endif // _GPIO_JOYSTICK_ATTACHMENT_H