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

// -----------------------------
// Alarm / Sunrise State Machine
// -----------------------------
enum class AlarmState : uint8_t { Idle, Sunrise, Hold, SnoozeWait };

static AlarmState g_alarm_state = AlarmState::Idle;

static uint32_t g_alarm_start_ms = 0;          // für millis-basierte Ramp/Maxdauer
static uint32_t g_snooze_until_unix = 0;       // RTC-basiert

static constexpr float T_START_COLD = 0.55f;   // kalt ab 55% der Sunrise-Zeit

static uint8_t anti_glow(uint8_t v) {
  return (v < 3) ? 0 : v;
}

bool light_alarm_active() {
  return (g_alarm_state == AlarmState::Sunrise || g_alarm_state == AlarmState::Hold);
}

bool light_alarm_snoozing() {
  return (g_alarm_state == AlarmState::SnoozeWait);
}

void light_alarm_start() {
  g_alarm_state    = AlarmState::Sunrise;
  g_alarm_start_ms = millis();
  g_snooze_until_unix = 0;
}

void light_alarm_stop() {
  g_alarm_state = AlarmState::Idle;
  g_snooze_until_unix = 0;
  write_channels(0, 0);
}

void light_alarm_snooze(uint32_t now_unixtime) {
  // Snooze: Alarm aus, später neu starten (inkl. neuer max-duration)
  uint32_t add = (uint32_t)settings.snooze_minutes * 60UL;
  g_snooze_until_unix = now_unixtime + add;

  g_alarm_state = AlarmState::SnoozeWait;
  write_channels(0, 0);
}

void light_update_alarm(uint32_t now_unixtime) {
  // Snooze abgelaufen?
  if (g_alarm_state == AlarmState::SnoozeWait) {
    if (g_snooze_until_unix > 0 && now_unixtime >= g_snooze_until_unix) {
      // Neustart: Sunrise + Maxdauer laufen erneut
      light_alarm_start();
    }
    return;
  }

  // Idle -> nix tun
  if (g_alarm_state == AlarmState::Idle) return;

  // Max-Dauer prüfen gilt für Sunrise + Hold zusammen
  if (settings.alarm_max_duration_min > 0) {
    uint32_t max_ms = (uint32_t)settings.alarm_max_duration_min * 60UL * 1000UL;
    if (millis() - g_alarm_start_ms >= max_ms) {
      light_alarm_stop();
      return;
    }
  }

  // Sunrise-Dauer
  uint32_t dur_ms = (uint32_t)settings.sunrise_duration_min * 60UL * 1000UL;
  if (dur_ms == 0) dur_ms = 1;

  if (g_alarm_state == AlarmState::Sunrise) {
    uint32_t elapsed = millis() - g_alarm_start_ms;

    float t = (elapsed >= dur_ms) ? 1.0f : (float)elapsed / (float)dur_ms;
    t = clamp01(t);

    // Warm 0..1 über t
    float warm_norm = curve_sunrise(t);

    // Cold ab 0.55, dann 0..1 über tc
    float cold_norm = 0.0f;
    if (t >= T_START_COLD) {
      float tc = (t - T_START_COLD) / (1.0f - T_START_COLD);
      tc = clamp01(tc);
      cold_norm = curve_sunrise(tc);
    }

    uint8_t pwm_w = anti_glow(to_pwm8(warm_norm));
    uint8_t pwm_c = anti_glow(to_pwm8(cold_norm));

    write_channels(pwm_w, pwm_c);

    // Wenn Sunrise fertig: in Hold wechseln (Licht bleibt an)
    if (elapsed >= dur_ms) {
      g_alarm_state = AlarmState::Hold;
    }
    return;
  }

  // Hold: Endzustand halten (Warm & Cold voll an)
  if (g_alarm_state == AlarmState::Hold) {
    write_channels(255, 255);
    return;
  }
}
