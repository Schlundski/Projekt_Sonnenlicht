#include "light.h"

#include "config.h"
#include "settings.h"
#include "input.h"

#include <Arduino.h>
#include <curve.h>

// -----------------------------
// PWM-Setup (ESP32 LEDC)
// -----------------------------
static constexpr uint8_t  PWM_RES_BITS = 8;      // 8 Bit -> 0..255
static constexpr uint32_t PWM_FREQ_HZ  = 2000;   // 2 kHz flackerfrei, leise genug

// LEDC Channels (frei wählbar, aber eindeutig)
static constexpr uint8_t CH_WARM = 0;
static constexpr uint8_t CH_COLD = 1;

unsigned long timer = 0;

// Helper: 0..1 -> 0..255
static uint8_t to_pwm8(float x) {
  x = clamp01(x);
  // runden statt abschneiden
  int v = (int)lroundf(x * 255.0f);
  if (v < 0) v = 0;
  if (v > 255) v = 255;
  return (uint8_t)v;
}

static float get_night_level_norm() {
  if (settings.night_override_enabled) {
    float x = (float)settings.night_override_level / 100.0f;
    return clamp01(x);
  }
  return input_get_poti_norm(); // 0..1
}

static void write_channels(uint8_t pwm_warm, uint8_t pwm_cold) {
  ledcWrite(CH_WARM, pwm_warm);
  ledcWrite(CH_COLD, pwm_cold);
    if(millis() - timer >= 1000) {
    Serial.println("Warm PWM: " + String(pwm_warm));
    Serial.println("Cold PWM: " + String(pwm_cold));
  }

}

void light_init() {
  // Pins als PWM-Ausgänge initialisieren
  pinMode(PIN_MOSFET_WARM, OUTPUT);
  pinMode(PIN_MOSFET_COLD, OUTPUT);

  // LEDC Setup
  ledcSetup(CH_WARM, PWM_FREQ_HZ, PWM_RES_BITS);
  ledcSetup(CH_COLD, PWM_FREQ_HZ, PWM_RES_BITS);

  ledcAttachPin(PIN_MOSFET_WARM, CH_WARM);
  ledcAttachPin(PIN_MOSFET_COLD, CH_COLD);

  // Start: aus
  write_channels(0, 0);
}

void light_update_night() {
  // 1) Quelle: Override oder Poti
  float level = get_night_level_norm();

  // Optional: Deadzone, damit bei "fast 0" wirklich aus ist
  if (level < 0.02f) level = 0.0f;

  // 2) Kurve (gefühlte Helligkeit)
  float shaped = curve_night(level);

  // 3) PWM
  uint8_t pwm = to_pwm8(shaped);

  // 4) Kanalwahl
  switch (settings.night_channel) {
    case NightChannel::Warm:
      write_channels(pwm, 0);
      break;
    case NightChannel::Cold:
      write_channels(0, pwm);
      break;
    case NightChannel::Both:
      write_channels(pwm, pwm);
      break;
    default:
      write_channels(pwm, 0);
      break;
  }
}
