#ifndef _GPIO_ATTACHMENT_H
#define _GPIO_ATTACHMENT_H

#include <stdint.h>

/**
 * @brief Base class for peripherals attached to the GPIOCard (65C22 VIA)
 * 
 * This abstract interface allows different peripherals (keyboards, joysticks, etc.)
 * to be attached to the GPIO ports and control lines of the 65C22 VIA emulation.
 * 
 * Communication between the GPIOCard and attachments occurs through:
 * - Port A (PA0-PA7): 8-bit bidirectional I/O port
 * - Port B (PB0-PB7): 8-bit bidirectional I/O port
 * - CA1, CA2: Control lines for Port A (interrupts and enable/disable)
 * - CB1, CB2: Control lines for Port B (interrupts and enable/disable)
 * 
 * Multiple attachments can share the same port using priority-based multiplexing.
 */
class GPIOAttachment {
  protected:
    // Control line configuration
    bool useCA1;  // Uses CA1 for interrupt input
    bool useCA2;  // Uses CA2 for enable/control
    bool useCB1;  // Uses CB1 for interrupt input
    bool useCB2;  // Uses CB2 for enable/control
    
    // Current control line states
    bool stateCA1;
    bool stateCA2;
    bool stateCB1;
    bool stateCB2;
    
    // Priority for port multiplexing (lower value = higher priority)
    uint8_t priority;
    
    // Enabled state (derived from control lines)
    bool enabled;
    
  public:
    /**
     * @brief Construct a new GPIO Attachment
     * 
     * @param priority Priority for port multiplexing (0 = highest, 255 = lowest)
     * @param useCA1 True if this attachment uses CA1 control line
     * @param useCA2 True if this attachment uses CA2 control line
     * @param useCB1 True if this attachment uses CB1 control line
     * @param useCB2 True if this attachment uses CB2 control line
     */
    GPIOAttachment(uint8_t priority = 128, 
                   bool useCA1 = false, 
                   bool useCA2 = false,
                   bool useCB1 = false, 
                   bool useCB2 = false) 
      : useCA1(useCA1), useCA2(useCA2), useCB1(useCB1), useCB2(useCB2),
        stateCA1(false), stateCA2(false), stateCB1(false), stateCB2(false),
        priority(priority), enabled(true) {}
    
    virtual ~GPIOAttachment() {}
    
    /**
     * @brief Read data from Port A
     * 
     * Called by GPIOCard when Port A is read. The attachment should return
     * the data it wants to present on Port A, or 0xFF if it has no data.
     * 
     * @param ddrA Data Direction Register A (1 = output, 0 = input)
     * @param orA Output Register A (current output values)
     * @return uint8_t Data to present on Port A inputs (0xFF = no data)
     */
    virtual uint8_t readPortA(uint8_t ddrA, uint8_t orA) { return 0xFF; }
    
    /**
     * @brief Read data from Port B
     * 
     * Called by GPIOCard when Port B is read. The attachment should return
     * the data it wants to present on Port B, or 0xFF if it has no data.
     * 
     * @param ddrB Data Direction Register B (1 = output, 0 = input)
     * @param orB Output Register B (current output values)
     * @return uint8_t Data to present on Port B inputs (0xFF = no data)
     */
    virtual uint8_t readPortB(uint8_t ddrB, uint8_t orB) { return 0xFF; }
    
    /**
     * @brief Notify attachment that Port A was written
     * 
     * Called by GPIOCard when software writes to Port A. Allows attachments
     * to respond to output changes (e.g., for bidirectional communication).
     * 
     * @param value Value written to Port A
     * @param ddrA Data Direction Register A (1 = output, 0 = input)
     */
    virtual void writePortA(uint8_t value, uint8_t ddrA) {}
    
    /**
     * @brief Notify attachment that Port B was written
     * 
     * Called by GPIOCard when software writes to Port B. Allows attachments
     * to respond to output changes (e.g., keyboard matrix column selection).
     * 
     * @param value Value written to Port B
     * @param ddrB Data Direction Register B (1 = output, 0 = input)
     */
    virtual void writePortB(uint8_t value, uint8_t ddrB) {}
    
    /**
     * @brief Update control line states
     * 
     * Called by GPIOCard when any control line changes state. The attachment
     * can update its enabled state based on the control lines it uses.
     * 
     * @param ca1 Current state of CA1 line
     * @param ca2 Current state of CA2 line
     * @param cb1 Current state of CB1 line
     * @param cb2 Current state of CB2 line
     */
    virtual void updateControlLines(bool ca1, bool ca2, bool cb1, bool cb2) {
      stateCA1 = ca1;
      stateCA2 = ca2;
      stateCB1 = cb1;
      stateCB2 = cb2;
      
      // Default behavior: disabled when any used control line is HIGH
      // Subclasses can override for different behavior
      enabled = true;
      if (useCA2 && ca2) enabled = false;
      if (useCB2 && cb2) enabled = false;
    }
    
    /**
     * @brief Tick function called each CPU cycle
     * 
     * Allows the attachment to perform periodic updates and manage timing.
     * 
     * @param cpuFrequency Current CPU frequency in Hz
     */
    virtual void tick(uint32_t cpuFrequency) {}
    
    /**
     * @brief Reset attachment to initial state
     */
    virtual void reset() {
      stateCA1 = false;
      stateCA2 = false;
      stateCB1 = false;
      stateCB2 = false;
      enabled = true;
    }
    
    /**
     * @brief Check if attachment has pending interrupt on CA1
     * 
     * @return true if CA1 interrupt should be triggered
     */
    virtual bool hasCA1Interrupt() const { return false; }
    
    /**
     * @brief Check if attachment has pending interrupt on CA2
     * 
     * @return true if CA2 interrupt should be triggered
     */
    virtual bool hasCA2Interrupt() const { return false; }
    
    /**
     * @brief Check if attachment has pending interrupt on CB1
     * 
     * @return true if CB1 interrupt should be triggered
     */
    virtual bool hasCB1Interrupt() const { return false; }
    
    /**
     * @brief Check if attachment has pending interrupt on CB2
     * 
     * @return true if CB2 interrupt should be triggered
     */
    virtual bool hasCB2Interrupt() const { return false; }
    
    /**
     * @brief Clear interrupt flags
     * 
     * Called by GPIOCard when interrupt flags are cleared (e.g., by reading port).
     * 
     * @param ca1 Clear CA1 interrupt
     * @param ca2 Clear CA2 interrupt
     * @param cb1 Clear CB1 interrupt
     * @param cb2 Clear CB2 interrupt
     */
    virtual void clearInterrupts(bool ca1, bool ca2, bool cb1, bool cb2) {}
    
    /**
     * @brief Get attachment priority
     * 
     * @return uint8_t Priority value (0 = highest, 255 = lowest)
     */
    uint8_t getPriority() const { return priority; }
    
    /**
     * @brief Check if attachment is currently enabled
     * 
     * @return true if enabled
     */
    bool isEnabled() const { return enabled; }
};

#endif // _GPIO_ATTACHMENT_H
