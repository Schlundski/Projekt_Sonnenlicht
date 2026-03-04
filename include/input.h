#pragma once
#include <Arduino.h>

void input_init();
void input_update();

// Taster-Flanken
bool input_alarm_snooze_fell();
bool input_alarm_stop_fell();

// Poti
float input_get_poti_norm();   // 0.0 - 1.0
