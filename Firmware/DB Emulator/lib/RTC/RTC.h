#ifndef _RTC_H
#define _RTC_H

#include <Arduino.h>
#include <TimeLib.h>

class RTC {
  private:
    static time_t syncTime();

  public:
    RTC();
    ~RTC() {};

    static String formattedDateTime();
};

// /*                                                      RTC Registers                                                                                   */
// /*                                                                                                                                                      */
// /*                            | BIT 7 | BIT 6 | BIT 5 | BIT 4 | BIT 3 | BIT 2 | BIT 1 | BIT 0 |                                                         */
// #define RTC_SEC    0       /* |                            SECONDS                            |  Real Time Clock:  Seconds (0-59)                       */
// #define RTC_MIN    1       /* |                            MINUTES                            |  Real Time Clock:  Minute (0-59)                        */
// #define RTC_HR     2       /* |                             HOURS                             |  Real Time Clock:  Hour (0-23)                          */
// #define RTC_DAY    3       /* |                              DAY                              |  Real Time Clock:  Day (1-31)                           */
// #define RTC_MON    4       /* |                             MONTH                             |  Real Time Clock:  Month (1-12)                         */
// #define RTC_YR     5       /* |                             YEAR                              |  Real Time Clock:  Year (0-99) (Offset from 1970)       */

// /* Notes:                                                                                                                                               */
// /* - Real Time Clock IO registers are read-only. Teensy Loader automatically initializes the RTC to your PC's time while uploading firmware.            */
// /* - If a coin cell is connected to VBAT, the RTC will continue keeping time while power is turned off. See: https://www.pjrc.com/store/teensy41.html   */

// #define RTC_START  (RTC_SEC  + 0x16)  // Defaults to $8816
// #define RTC_END    (RTC_YR   + 0x16)  // Defaults to $881B

// namespace RTC {

//   void begin();
//   void update();
//   byte read(byte reg);
//   void write(byte reg, byte data);
//   String formattedDateTime();
//   time_t syncTime();

// };

#endif