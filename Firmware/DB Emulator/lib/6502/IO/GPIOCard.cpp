#include "GPIOCard.h"
#include "GPIOAttachments/GPIOAttachment.h"

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
  
  // Initialize attachment arrays
  portA_attachmentCount = 0;
  portB_attachmentCount = 0;
  for (int i = 0; i < MAX_ATTACHMENTS_PER_PORT; i++) {
    portA_attachments[i] = nullptr;
    portB_attachments[i] = nullptr;
  }
  
  // Reset all attachments
  for (uint8_t i = 0; i < portA_attachmentCount; i++) {
    if (portA_attachments[i] != nullptr) {
      portA_attachments[i]->reset();
    }
  }
  for (uint8_t i = 0; i < portB_attachmentCount; i++) {
    if (portB_attachments[i] != nullptr) {
      portB_attachments[i]->reset();
    }
  }
  
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
      // Notify attachments that interrupts were cleared
      for (uint8_t i = 0; i < portB_attachmentCount; i++) {
        if (portB_attachments[i] != nullptr) {
          portB_attachments[i]->clearInterrupts(false, false, true, true);
        }
      }
      break;
      
    case VIA_ORA:
      // Reading ORA clears CA1 and CA2 interrupt flags
      clearIRQFlag(IRQ_CA1 | IRQ_CA2);
      value = readPortA();
      // Notify attachments that interrupts were cleared
      for (uint8_t i = 0; i < portA_attachmentCount; i++) {
        if (portA_attachments[i] != nullptr) {
          portA_attachments[i]->clearInterrupts(true, true, false, false);
        }
      }
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
  
  // Tick all attachments
  for (uint8_t i = 0; i < portA_attachmentCount; i++) {
    if (portA_attachments[i] != nullptr) {
      portA_attachments[i]->tick(cpuFrequency);
    }
  }
  for (uint8_t i = 0; i < portB_attachmentCount; i++) {
    if (portB_attachments[i] != nullptr) {
      portB_attachments[i]->tick(cpuFrequency);
    }
  }
  
  // Check for attachment interrupts
  for (uint8_t i = 0; i < portA_attachmentCount; i++) {
    if (portA_attachments[i] != nullptr) {
      if (portA_attachments[i]->hasCA1Interrupt()) {
        setIRQFlag(IRQ_CA1);
      }
      if (portA_attachments[i]->hasCA2Interrupt()) {
        setIRQFlag(IRQ_CA2);
      }
    }
  }
  for (uint8_t i = 0; i < portB_attachmentCount; i++) {
    if (portB_attachments[i] != nullptr) {
      if (portB_attachments[i]->hasCB1Interrupt()) {
        setIRQFlag(IRQ_CB1);
      }
      if (portB_attachments[i]->hasCB2Interrupt()) {
        setIRQFlag(IRQ_CB2);
      }
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
  
  // Determine input sources from attachments (priority-based multiplexing)
  uint8_t externalInput = 0xFF;
  
  // Query all Port A attachments in priority order
  for (uint8_t i = 0; i < portA_attachmentCount; i++) {
    if (portA_attachments[i] != nullptr && portA_attachments[i]->isEnabled()) {
      uint8_t attachmentData = portA_attachments[i]->readPortA(regDDRA, regORA);
      // First enabled attachment with data (not 0xFF) wins, or AND all values together
      externalInput &= attachmentData;
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
  
  // Determine input sources from attachments (priority-based multiplexing)
  uint8_t externalInput = 0xFF;
  
  // Query all Port B attachments in priority order
  for (uint8_t i = 0; i < portB_attachmentCount; i++) {
    if (portB_attachments[i] != nullptr && portB_attachments[i]->isEnabled()) {
      uint8_t attachmentData = portB_attachments[i]->readPortB(regDDRB, regORB);
      // First enabled attachment with data (not 0xFF) wins, or AND all values together
      externalInput &= attachmentData;
    }
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
  // Notify all Port A attachments of the write
  for (uint8_t i = 0; i < portA_attachmentCount; i++) {
    if (portA_attachments[i] != nullptr) {
      portA_attachments[i]->writePortA(value, regDDRA);
    }
  }
}

void GPIOCard::writePortB(uint8_t value) {
  // Notify all Port B attachments of the write
  for (uint8_t i = 0; i < portB_attachmentCount; i++) {
    if (portB_attachments[i] != nullptr) {
      portB_attachments[i]->writePortB(value, regDDRB);
    }
  }
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
      break;
      
    case 0x04:  // Handshake output
    case 0x05:  // Pulse output
      // Output modes
      break;
      
    case 0x06:  // Manual output LOW
      CA2 = false;
      break;
      
    case 0x07:  // Manual output HIGH
      CA2 = true;
      break;
  }
  
  // Notify all attachments of control line changes
  notifyAttachmentsControlLines();
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
      break;
      
    case 0x04:  // Handshake output
    case 0x05:  // Pulse output
      // Output modes
      break;
      
    case 0x06:  // Manual output LOW
      CB2 = false;
      break;
      
    case 0x07:  // Manual output HIGH
      CB2 = true;
      break;
  }
  
  // Notify all attachments of control line changes
  notifyAttachmentsControlLines();
}

void GPIOCard::notifyAttachmentsControlLines() {
  // Notify all attachments of control line state changes
  for (uint8_t i = 0; i < portA_attachmentCount; i++) {
    if (portA_attachments[i] != nullptr) {
      portA_attachments[i]->updateControlLines(CA1, CA2, CB1, CB2);
    }
  }
  for (uint8_t i = 0; i < portB_attachmentCount; i++) {
    if (portB_attachments[i] != nullptr) {
      portB_attachments[i]->updateControlLines(CA1, CA2, CB1, CB2);
    }
  }
}

void GPIOCard::sortAttachmentsByPriority() {
  // Simple bubble sort for Port A attachments by priority (lower = higher priority)
  for (uint8_t i = 0; i < portA_attachmentCount - 1; i++) {
    for (uint8_t j = 0; j < portA_attachmentCount - i - 1; j++) {
      if (portA_attachments[j] != nullptr && portA_attachments[j + 1] != nullptr) {
        if (portA_attachments[j]->getPriority() > portA_attachments[j + 1]->getPriority()) {
          // Swap
          GPIOAttachment* temp = portA_attachments[j];
          portA_attachments[j] = portA_attachments[j + 1];
          portA_attachments[j + 1] = temp;
        }
      }
    }
  }
  
  // Simple bubble sort for Port B attachments by priority
  for (uint8_t i = 0; i < portB_attachmentCount - 1; i++) {
    for (uint8_t j = 0; j < portB_attachmentCount - i - 1; j++) {
      if (portB_attachments[j] != nullptr && portB_attachments[j + 1] != nullptr) {
        if (portB_attachments[j]->getPriority() > portB_attachments[j + 1]->getPriority()) {
          // Swap
          GPIOAttachment* temp = portB_attachments[j];
          portB_attachments[j] = portB_attachments[j + 1];
          portB_attachments[j + 1] = temp;
        }
      }
    }
  }
}

void GPIOCard::attachToPortA(GPIOAttachment* attachment) {
  if (attachment != nullptr && portA_attachmentCount < MAX_ATTACHMENTS_PER_PORT) {
    portA_attachments[portA_attachmentCount++] = attachment;
    sortAttachmentsByPriority();
    // Notify the attachment of current control line states
    attachment->updateControlLines(CA1, CA2, CB1, CB2);
  }
}

void GPIOCard::attachToPortB(GPIOAttachment* attachment) {
  if (attachment != nullptr && portB_attachmentCount < MAX_ATTACHMENTS_PER_PORT) {
    portB_attachments[portB_attachmentCount++] = attachment;
    sortAttachmentsByPriority();
    // Notify the attachment of current control line states
    attachment->updateControlLines(CA1, CA2, CB1, CB2);
  }
}

GPIOAttachment* GPIOCard::getPortAAttachment(uint8_t index) {
  if (index < portA_attachmentCount) {
    return portA_attachments[index];
  }
  return nullptr;
}

GPIOAttachment* GPIOCard::getPortBAttachment(uint8_t index) {
  if (index < portB_attachmentCount) {
    return portB_attachments[index];
  }
  return nullptr;
}