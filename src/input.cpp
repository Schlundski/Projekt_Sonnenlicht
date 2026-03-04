#include "input.h"
#include "config.h"

// -----------------------------
// Konfiguration
// -----------------------------
static constexpr uint32_t DEBOUNCE_MS = 30;

// -----------------------------
// Taster-Zustände
// -----------------------------
static bool raw_snooze = true;
static bool raw_stop   = true;

static bool stable_snooze = true;
static bool stable_stop   = true;

static uint32_t t_snooze = 0;
static uint32_t t_stop   = 0;

static bool ev_snooze_fell = false;
static bool ev_stop_fell   = false;

// -----------------------------
// Poti
// -----------------------------
static float poti_norm = 0.0f;

// -----------------------------
void input_init() {
  pinMode(PIN__ALARM_SNOOZE, INPUT_PULLUP);
  pinMode(PIN_ALARM_STOP, INPUT_PULLUP);

  raw_snooze = digitalRead(PIN__ALARM_SNOOZE);
  raw_stop   = digitalRead(PIN_ALARM_STOP);

  stable_snooze = raw_snooze;
  stable_stop   = raw_stop;

  t_snooze = millis();
  t_stop   = millis();

  analogReadResolution(12); // 0..4095
}

// -----------------------------
void input_update() {
  ev_snooze_fell = false;
  ev_stop_fell   = false;

  uint32_t now = millis();

  // ===== Snooze-Taster =====
  bool rS = digitalRead(PIN__ALARM_SNOOZE);
  if (rS != raw_snooze) {
    raw_snooze = rS;
    t_snooze = now;
  }
  if ((now - t_snooze) >= DEBOUNCE_MS && stable_snooze != raw_snooze) {
    bool prev = stable_snooze;
    stable_snooze = raw_snooze;
    if (prev == true && stable_snooze == false) {
      ev_snooze_fell = true;
    }
  }

  // ===== Stop-Taster =====
  bool rP = digitalRead(PIN_ALARM_STOP);
  if (rP != raw_stop) {
    raw_stop = rP;
    t_stop = now;
  }
  if ((now - t_stop) >= DEBOUNCE_MS && stable_stop != raw_stop) {
    bool prev = stable_stop;
    stable_stop = raw_stop;
    if (prev == true && stable_stop == false) {
      ev_stop_fell = true;
    }
  }

  // ===== Poti =====
  int adc = analogRead(PIN_POTI);   // 0..4095
  poti_norm = (float)adc / 4095.0f;

  // kleine Deadzone unten (optional, aber hilfreich)
  if (poti_norm < 0.01f) poti_norm = 0.0f;
}

// -----------------------------
bool input_alarm_snooze_fell() {
  bool v = ev_snooze_fell;
  ev_snooze_fell = false;
  return v;
}

bool input_alarm_stop_fell() {
  bool v = ev_stop_fell;
  ev_stop_fell = false;
  return v;
}

float input_get_poti_norm() {
  return poti_norm;
}
