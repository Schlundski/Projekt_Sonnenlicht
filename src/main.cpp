#include <Arduino.h>
#include <Wire.h>
#include <curve.h>

#include "config.h"
#include "rtc.h"
#include "ui_clock.h"
#include "settings.h"
#include "input.h"
#include "light.h"
#include "Preferences.h"
#include "audio.h"


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();

  // 🔧 DEV-RESET (solange noch keine App)
  //Preferences p;
  //p.begin("wecker", false);
  //p.clear();
  //p.end();

  settings_load();
  input_init();
  light_init();
  ui_clock_begin();

    settings_load();

  // Audio früh initialisieren -> direkt testen
  if (!audio_begin()) {
    Serial.println("DFPlayer NICHT gefunden / init fehlgeschlagen!");
  } else {
    Serial.println("DFPlayer OK");
    audio_play(3); // 0003.mp3 = "Bestätigung"
  }

  if (!rtc_begin(false)) {   // true = Uhr stellen
    Serial.println("RTC NICHT gefunden!");
    while (1) {}
  }
  Serial.println("RTC gefunden.");


}

void loop() {
  ui_clock_update();
  input_update();

  static uint32_t t = 0;
  static uint32_t now_unix = 0;

  if (input_alarm_snooze_fell()) audio_play(2); // Snooze-Sound
  if (input_alarm_stop_fell())   audio_play(1); // Alarm-Sound


  // RTC nicht dauernd pollen: 1x pro Sekunde reicht
  if (millis() - t >= 1000) {
    t = millis();
    now_unix = rtc_now().unixtime();
  }

  light_update_alarm(now_unix);

    // --- Snooze / Stop ---
  if (input_alarm_snooze_fell()) {
    if (light_alarm_active()) {
      light_alarm_snooze(now_unix);
    }
  }

  if (input_alarm_stop_fell()) {
    if (light_alarm_active() || light_alarm_snoozing()) {
      light_alarm_stop();
    }
  }


  if (!light_alarm_active() && !light_alarm_snoozing()) {
    light_update_night();
  }
}
