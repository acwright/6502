#ifndef _RTC_CARD_H
#define _RTC_CARD_H

#include "IO.h"

/**
 * DS1511Y Real-Time Clock IC Emulation
 * 
 * Register Map (0x00-0x1F):
 * 0x00: Seconds (BCD, 00-59)
 * 0x01: Minutes (BCD, 00-59)
 * 0x02: Hours (BCD, 00-23)
 * 0x03: Day of Week (1-7, 1=Sunday)
 * 0x04: Date (BCD, 01-31)
 * 0x05: Month (BCD, 01-12) + Control bits (EOSC, E32K)
 * 0x06: Year (BCD, 00-99)
 * 0x07: Century (BCD, 00-39)
 * 0x08: Alarm Seconds (BCD, 00-59) + AM1 bit
 * 0x09: Alarm Minutes (BCD, 00-59) + AM2 bit
 * 0x0A: Alarm Hours (BCD, 00-23) + AM3 bit
 * 0x0B: Alarm Day/Date + AM4, DY/DT bits
 * 0x0C: Watchdog (0.1 Second and 0.01 Second)
 * 0x0D: Watchdog (0.1 Second and Second)
 * 0x0E: Control A (BLF1, BLF2, PBS, PAB, TDF, KSF, WDF, IRQF)
 * 0x0F: Control B (TE, CS, BME, TPE, TIE, KIE, WDE, WDS)
 * 0x10: RAM Address (Extended RAM Address pointer)
 * 0x11: Reserved
 * 0x12: Reserved
 * 0x13: RAM Data (Extended RAM Data at address pointed to by 0x10)
 */
class RTCCard: public IO {
  public:
    RTCCard();
    ~RTCCard() {};

    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick(uint32_t cpuFrequency) override;
    void    reset() override;

  private:
    // RTC Registers (user-visible)
    uint8_t userSeconds;
    uint8_t userMinutes;
    uint8_t userHours;
    uint8_t userDayOfWeek;
    uint8_t userDate;
    uint8_t userMonth;
    uint8_t monthControl;
    uint8_t userYear;
    uint8_t userCentury;

    // Internal timekeeping registers
    uint8_t internalSeconds;
    uint8_t internalMinutes;
    uint8_t internalHours;
    uint8_t internalDayOfWeek;
    uint8_t internalDate;
    uint8_t internalMonth;
    uint8_t internalYear;
    uint8_t internalCentury;

    // Alarm registers
    uint8_t alarmSeconds;
    uint8_t alarmMinutes;
    uint8_t alarmHours;
    uint8_t alarmDayDate;

    // Watchdog
    uint8_t watchdog1;
    uint8_t watchdog2;
    uint16_t watchdogCounterCentis;
    uint32_t watchdogCycleCounter;

    // Control registers
    uint8_t controlA;
    uint8_t controlB;

    // Extended RAM
    uint8_t ramAddress;
    uint8_t ramData[256];

    // Time tracking
    uint32_t cycleCounter;
    uint32_t cpuFrequency;
    uint32_t transferCycleCounter;
    bool pendingUserToInternal;
    bool userSyncNeeded;
    bool lastTEEnabled;

    // Helper methods
    void initializeWithCurrentTime();
    uint8_t decimalToBCD(uint8_t decimal);
    uint8_t bcdToDecimal(uint8_t bcd);
    uint8_t getDaysInMonth(uint8_t month, uint8_t year, uint8_t century);
    uint8_t getNextDayOfWeek(uint8_t currentDay);
    void copyInternalToUser();
    void copyUserToInternal();
    uint32_t getTransferCyclesRequired();
    void markUserTimeWrite();
    void incrementTime();
    void checkAlarm();
    void raiseInterruptIfEnabled(uint8_t flagMask, uint8_t enableMask);
    void setKickstartFlag();
    uint16_t decodeWatchdogCentis();
    void reloadWatchdog();
    void stepWatchdog();
};

#endif