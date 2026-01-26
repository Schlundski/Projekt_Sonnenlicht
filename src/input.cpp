#include "input.h"
#include "config.h"
#include <Arduino.h>


void input_init() {
    pinMode(PIN_POTI, INPUT);
}

float input_get_poti_norm() {
  int raw = analogRead(PIN_POTI);     // Auflösung ADC 0..4095
  float x = raw / 4095.0f;

  // clamp
  if (x < 0.0f) x = 0.0f;
  if (x > 1.0f) x = 1.0f;

  return x;
}
