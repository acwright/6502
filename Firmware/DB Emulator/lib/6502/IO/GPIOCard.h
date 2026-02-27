#ifndef _GPIO_CARD_H
#define _GPIO_CARD_H

#include "IO.h"

// Forward declaration
class GPIOAttachment;

// Maximum number of attachments per port
#define MAX_ATTACHMENTS_PER_PORT 4

// 65C22 VIA Register Addresses
#define VIA_ORB     0x00  // Output Register B (or Input Register B)
#define VIA_ORA     0x01  // Output Register A (or Input Register A)
#define VIA_DDRB    0x02  // Data Direction Register B
#define VIA_DDRA    0x03  // Data Direction Register A
#define VIA_T1CL    0x04  // Timer 1 Counter Low
#define VIA_T1CH    0x05  // Timer 1 Counter High
#define VIA_T1LL    0x06  // Timer 1 Latch Low
#define VIA_T1LH    0x07  // Timer 1 Latch High
#define VIA_T2CL    0x08  // Timer 2 Counter Low
#define VIA_T2CH    0x09  // Timer 2 Counter High
#define VIA_SR      0x0A  // Shift Register
#define VIA_ACR     0x0B  // Auxiliary Control Register
#define VIA_PCR     0x0C  // Peripheral Control Register
#define VIA_IFR     0x0D  // Interrupt Flag Register
#define VIA_IER     0x0E  // Interrupt Enable Register
#define VIA_ORA_NH  0x0F  // Output Register A (no handshake)

// Interrupt bits
#define IRQ_CA2     0x01
#define IRQ_CA1     0x02
#define IRQ_SR      0x04
#define IRQ_CB2     0x08
#define IRQ_CB1     0x10
#define IRQ_T2      0x20
#define IRQ_T1      0x40
#define IRQ_IRQ     0x80

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

class GPIOCard: public IO {
  private:
    // 65C22 VIA Registers
    uint8_t regORB;      // Output Register B
    uint8_t regORA;      // Output Register A
    uint8_t regDDRB;     // Data Direction Register B
    uint8_t regDDRA;     // Data Direction Register A
    uint16_t regT1C;     // Timer 1 Counter
    uint16_t regT1L;     // Timer 1 Latch
    uint16_t regT2C;     // Timer 2 Counter
    uint8_t regT2L;      // Timer 2 Latch (low byte only)
    uint8_t regSR;       // Shift Register
    uint8_t regACR;      // Auxiliary Control Register
    uint8_t regPCR;      // Peripheral Control Register
    uint8_t regIFR;      // Interrupt Flag Register
    uint8_t regIER;      // Interrupt Enable Register
    
    // Control line states
    bool CA1;
    bool CA2;
    bool CB1;
    bool CB2;
    
    // Timer flags
    bool T1_running;
    bool T2_running;
    bool T1_IRQ_enabled;
    bool T2_IRQ_enabled;
    
    // Attachments (peripherals connected to the GPIO ports)
    GPIOAttachment* portA_attachments[MAX_ATTACHMENTS_PER_PORT];
    GPIOAttachment* portB_attachments[MAX_ATTACHMENTS_PER_PORT];
    uint8_t portA_attachmentCount;
    uint8_t portB_attachmentCount;
    
    // Timing
    uint32_t tickCounter;
    uint32_t ticksPerMicrosecond;
    
    // Internal functions
    void updateIRQ();
    void setIRQFlag(uint8_t flag);
    void clearIRQFlag(uint8_t flag);
    uint8_t readPortA();
    uint8_t readPortB();
    void writePortA(uint8_t value);
    void writePortB(uint8_t value);
    void updateCA2();
    void updateCB2();
    void notifyAttachmentsControlLines();
    void sortAttachmentsByPriority();
    
  public:
    GPIOCard();
    ~GPIOCard() {};

    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick(uint32_t cpuFrequency) override;
    void    reset() override;

    // Attachment management
    void attachToPortA(GPIOAttachment* attachment);
    void attachToPortB(GPIOAttachment* attachment);
    GPIOAttachment* getPortAAttachment(uint8_t index);
    GPIOAttachment* getPortBAttachment(uint8_t index);
    uint8_t getPortAAttachmentCount() const { return portA_attachmentCount; }
    uint8_t getPortBAttachmentCount() const { return portB_attachmentCount; }
};

#endif