#include "RTCCard.h"
#include <TimeLib.h>

RTCCard::RTCCard() {
  cpuFrequency = 1000000; // Default 1MHz, will be updated by tick()
  initializeWithCurrentTime();
}

uint8_t RTCCard::read(uint16_t address) {
  address &= 0x1F;

  switch (address) {
    case 0x00: return userSeconds;
    case 0x01: return userMinutes;
    case 0x02: return userHours;
    case 0x03: return userDayOfWeek & 0x07;
    case 0x04: return userDate;
    case 0x05: return userMonth | monthControl;
    case 0x06: return userYear;
    case 0x07: return userCentury;
    case 0x08: return alarmSeconds;
    case 0x09: return alarmMinutes;
    case 0x0A: return alarmHours;
    case 0x0B: return alarmDayDate;
    case 0x0C: return watchdog1;
    case 0x0D: return watchdog2;
    case 0x0E: {
      // Reading Control A clears the interrupt flags: IRQF, WDF, KSF, TDF
      uint8_t result = controlA;
      controlA &= 0xF0; // Clear bits 0-3 (IRQF, WDF, KSF, TDF)
      return result;
    }
    case 0x0F: return controlB;
    case 0x10: return ramAddress;
    case 0x11: return 0; // Reserved
    case 0x12: return 0; // Reserved
    case 0x13: {
      uint8_t value = ramData[ramAddress];
      if ((controlB & 0x20) != 0) {
        ramAddress = (ramAddress + 1) & 0xFF;
      }
      return value;
    }
    default: return 0;
  }
}

void RTCCard::write(uint16_t address, uint8_t value) {
  value &= 0xFF;
  address &= 0x1F;

  switch (address) {
    case 0x00:
      userSeconds = value;
      markUserTimeWrite();
      break;
    case 0x01:
      userMinutes = value;
      markUserTimeWrite();
      break;
    case 0x02:
      userHours = value;
      markUserTimeWrite();
      break;
    case 0x03:
      userDayOfWeek = value & 0x07;
      markUserTimeWrite();
      break;
    case 0x04:
      userDate = value;
      markUserTimeWrite();
      break;
    case 0x05:
      userMonth = value & 0x1F;
      monthControl = value & 0xE0;
      markUserTimeWrite();
      break;
    case 0x06:
      userYear = value;
      markUserTimeWrite();
      break;
    case 0x07:
      userCentury = value;
      markUserTimeWrite();
      break;
    case 0x08:
      alarmSeconds = value;
      break;
    case 0x09:
      alarmMinutes = value;
      break;
    case 0x0A:
      alarmHours = value;
      break;
    case 0x0B:
      alarmDayDate = value;
      break;
    case 0x0C:
      watchdog1 = value;
      reloadWatchdog();
      break;
    case 0x0D:
      watchdog2 = value;
      reloadWatchdog();
      break;
    case 0x0E:
      // Writing 1 to flag bits (0-3) clears them; control bits (4-7) are written normally
      controlA = (value & 0xF0) | ((controlA & 0x0F) & ~(value & 0x0F));
      break;
    case 0x0F:
      controlB = value;
      raiseInterruptIfEnabled(0x04, 0x04);
      if ((controlB & 0x02) != 0) {
        reloadWatchdog();
      }
      break;
    case 0x10:
      ramAddress = value;
      break;
    case 0x11:
    case 0x12:
      // Reserved, ignore writes
      break;
    case 0x13:
      ramData[ramAddress] = value;
      if ((controlB & 0x20) != 0) {
        ramAddress = (ramAddress + 1) & 0xFF;
      }
      break;
  }
}

uint8_t RTCCard::tick(uint32_t cpuFrequency) {
  // Update the CPU frequency for timing calculations
  this->cpuFrequency = cpuFrequency;
  
  bool teEnabled = (controlB & 0x80) != 0;
  if (teEnabled != lastTEEnabled) {
    lastTEEnabled = teEnabled;
    transferCycleCounter = 0;
  }

  if (teEnabled) {
    transferCycleCounter++;
  } else {
    transferCycleCounter = 0;
  }

  bool transferReady = teEnabled &&
    transferCycleCounter >= getTransferCyclesRequired();

  if (transferReady && pendingUserToInternal) {
    copyUserToInternal();
    pendingUserToInternal = false;
    userSyncNeeded = false;
  }

  if ((monthControl & 0x80) == 0) {
    // Oscillator disabled
    stepWatchdog();
    return 0x00;
  }

  cycleCounter++;

  // Advance time when we've accumulated enough cycles for 1 second
  if (cycleCounter >= cpuFrequency) {
    cycleCounter = 0;
    incrementTime();
    checkAlarm();

    if (transferReady) {
      copyInternalToUser();
      userSyncNeeded = false;
    } else {
      userSyncNeeded = true;
    }
  } else if (transferReady && userSyncNeeded) {
    copyInternalToUser();
    userSyncNeeded = false;
  }

  stepWatchdog();
  return 0x00;
}

void RTCCard::reset() {
  // Cold start: Initialize with current time
  initializeWithCurrentTime();
  setKickstartFlag();
}

// Private helper methods

void RTCCard::initializeWithCurrentTime() {
  // Get current time from Teensy RTC
  tmElements_t tm;
  breakTime(now(), tm);
  
  internalSeconds = decimalToBCD(tm.Second);
  internalMinutes = decimalToBCD(tm.Minute);
  internalHours = decimalToBCD(tm.Hour);
  internalDayOfWeek = tm.Wday; // 1=Sunday in TimeLib
  internalDate = decimalToBCD(tm.Day);
  internalMonth = decimalToBCD(tm.Month);
  
  // TimeLib year is offset from 1970
  uint16_t fullYear = tm.Year + 1970;
  internalYear = decimalToBCD(fullYear % 100);
  internalCentury = decimalToBCD(fullYear / 100);
  
  monthControl = 0x80; // EOSC enabled by default (bit 7)
  
  copyInternalToUser();
  
  // Initialize alarm registers
  alarmSeconds = 0;
  alarmMinutes = 0;
  alarmHours = 0;
  alarmDayDate = 0;
  
  // Initialize watchdog
  watchdog1 = 0;
  watchdog2 = 0;
  watchdogCounterCentis = 0;
  watchdogCycleCounter = 0;
  
  // Initialize control registers
  controlA = 0;
  controlB = 0;
  
  // Initialize RAM
  ramAddress = 0;
  for (int i = 0; i < 256; i++) {
    ramData[i] = 0;
  }
  
  // Initialize state
  cycleCounter = 0;
  transferCycleCounter = 0;
  pendingUserToInternal = false;
  userSyncNeeded = false;
  lastTEEnabled = false;
}

uint8_t RTCCard::decimalToBCD(uint8_t decimal) {
  return ((decimal / 10) << 4) | (decimal % 10);
}

uint8_t RTCCard::bcdToDecimal(uint8_t bcd) {
  return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

uint8_t RTCCard::getDaysInMonth(uint8_t month, uint8_t year, uint8_t century) {
  uint16_t fullYear = (century * 100) + year;
  
  if (month == 1 || month == 3 || month == 5 || month == 7 || 
      month == 8 || month == 10 || month == 12) {
    return 31;
  }
  if (month == 4 || month == 6 || month == 9 || month == 11) {
    return 30;
  }
  
  // February - check for leap year
  if ((fullYear % 4 == 0 && fullYear % 100 != 0) || fullYear % 400 == 0) {
    return 29;
  }
  return 28;
}

uint8_t RTCCard::getNextDayOfWeek(uint8_t currentDay) {
  return currentDay == 7 ? 1 : currentDay + 1;
}

void RTCCard::copyInternalToUser() {
  userSeconds = internalSeconds;
  userMinutes = internalMinutes;
  userHours = internalHours;
  userDayOfWeek = internalDayOfWeek;
  userDate = internalDate;
  userMonth = internalMonth;
  userYear = internalYear;
  userCentury = internalCentury;
}

void RTCCard::copyUserToInternal() {
  internalSeconds = userSeconds;
  internalMinutes = userMinutes;
  internalHours = userHours;
  internalDayOfWeek = userDayOfWeek;
  internalDate = userDate;
  internalMonth = userMonth;
  internalYear = userYear;
  internalCentury = userCentury;
}

uint32_t RTCCard::getTransferCyclesRequired() {
  return max(1UL, (cpuFrequency * 366UL) / 1000000UL);
}

void RTCCard::markUserTimeWrite() {
  pendingUserToInternal = true;
  if ((controlB & 0x80) != 0 &&
      transferCycleCounter >= getTransferCyclesRequired()) {
    copyUserToInternal();
    pendingUserToInternal = false;
    userSyncNeeded = false;
  }
}

void RTCCard::incrementTime() {
  uint8_t sec = bcdToDecimal(internalSeconds);
  uint8_t min = bcdToDecimal(internalMinutes);
  uint8_t hour = bcdToDecimal(internalHours);
  uint8_t date = bcdToDecimal(internalDate);
  uint8_t month = bcdToDecimal(internalMonth);
  uint8_t year = bcdToDecimal(internalYear);
  uint8_t century = bcdToDecimal(internalCentury);

  sec++;
  if (sec >= 60) {
    sec = 0;
    min++;
    if (min >= 60) {
      min = 0;
      hour++;
      if (hour >= 24) {
        hour = 0;
        internalDayOfWeek = getNextDayOfWeek(internalDayOfWeek);
        date++;
        
        uint8_t daysInMonth = getDaysInMonth(month, year, century);
        if (date > daysInMonth) {
          date = 1;
          month++;
          if (month > 12) {
            month = 1;
            year++;
            if (year > 99) {
              year = 0;
              century++;
              if (century > 39) {
                century = 0;
              }
            }
          }
        }
      }
    }
  }

  internalSeconds = decimalToBCD(sec);
  internalMinutes = decimalToBCD(min);
  internalHours = decimalToBCD(hour);
  internalDate = decimalToBCD(date);
  internalMonth = decimalToBCD(month);
  internalYear = decimalToBCD(year);
  internalCentury = decimalToBCD(century);
}

void RTCCard::checkAlarm() {
  bool am1 = (alarmSeconds & 0x80) != 0;
  bool am2 = (alarmMinutes & 0x80) != 0;
  bool am3 = (alarmHours & 0x80) != 0;
  bool am4 = (alarmDayDate & 0x80) != 0;

  // If all AM bits are set, alarm is disabled
  if (am1 && am2 && am3 && am4) return;

  // Check matching based on AM bits
  bool secondsMatch = am1 || (internalSeconds == (alarmSeconds & 0x7F));
  bool minutesMatch = am2 || (internalMinutes == (alarmMinutes & 0x7F));
  bool hoursMatch = am3 || (internalHours == (alarmHours & 0x7F));
  
  bool dayDateMatch = true;
  if (!am4) {
    bool dyDt = (alarmDayDate & 0x40) != 0;
    uint8_t alarmValue = alarmDayDate & 0x3F;
    
    if (dyDt) {
      // Day of week match
      dayDateMatch = internalDayOfWeek == alarmValue;
    } else {
      // Date match
      dayDateMatch = internalDate == alarmValue;
    }
  }

  if (secondsMatch && minutesMatch && hoursMatch && dayDateMatch) {
    controlA |= 0x08; // Set TDF flag (bit 3)
    raiseInterruptIfEnabled(0x08, 0x08);
  }
}

void RTCCard::raiseInterruptIfEnabled(uint8_t flagMask, uint8_t enableMask) {
  if ((controlA & flagMask) == 0) return;
  if ((controlB & enableMask) == 0) return;
  controlA |= 0x01; // Set IRQF flag (bit 0)
  // Note: IRQ raising would need to be implemented via callback or external interface
}

void RTCCard::setKickstartFlag() {
  controlA |= 0x04; // Set KSF flag (bit 2)
  raiseInterruptIfEnabled(0x04, 0x04);
}

uint16_t RTCCard::decodeWatchdogCentis() {
  uint8_t hundredths = watchdog1 & 0x0F;
  uint8_t tenths = (watchdog1 >> 4) & 0x0F;
  uint8_t seconds = watchdog2 & 0x0F;
  uint8_t tensSeconds = (watchdog2 >> 4) & 0x0F;

  uint16_t totalSeconds = (tensSeconds * 10) + seconds;
  uint16_t totalCentis = (tenths * 10) + hundredths;
  return (totalSeconds * 100) + totalCentis;
}

void RTCCard::reloadWatchdog() {
  watchdogCounterCentis = decodeWatchdogCentis();
  watchdogCycleCounter = 0;
}

void RTCCard::stepWatchdog() {
  if ((controlB & 0x02) == 0) return; // WDE disabled
  if (watchdogCounterCentis == 0) return;

  uint32_t cyclesPerCentisecond = max(1UL, cpuFrequency / 100UL);
  watchdogCycleCounter++;
  if (watchdogCycleCounter < cyclesPerCentisecond) return;

  watchdogCycleCounter = 0;
  watchdogCounterCentis--;
  if (watchdogCounterCentis > 0) return;

  controlA |= 0x02; // Set WDF flag (bit 1)

  if ((controlB & 0x01) == 0) {
    raiseInterruptIfEnabled(0x02, 0x02);
  } else {
    // WDS=1 steers watchdog to reset; emulate by clearing WDE
    controlB &= ~0x02;
    // Note: NMI raising would need to be implemented via callback or external interface
  }
}