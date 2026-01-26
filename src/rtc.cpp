#include "rtc.h"

static RTC_DS3231 rtc;

bool rtc_begin(bool adjust_to_compile_time) {
  if (!rtc.begin()) return false;

  if (adjust_to_compile_time) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  return true;
}

DateTime rtc_now() {
  return rtc.now();
}
