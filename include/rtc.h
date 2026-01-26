#pragma once
#include <RTClib.h>

// adjust_to_compile_time = true nur EINMAL zum Stellen der Uhr!
bool rtc_begin(bool adjust_to_compile_time);
DateTime rtc_now();
