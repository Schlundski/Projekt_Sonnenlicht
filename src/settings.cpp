#include "settings.h"
#include <Preferences.h>

Settings settings;
static Preferences prefs;

static const char* NS = "wecker";

// prüfen, ob wir schon initial gespeichert haben
static bool has_init_flag() {
  prefs.begin(NS, true);
  bool ok = prefs.getBool("init", false);
  prefs.end();
  return ok;
}

void settings_reset_defaults() {
  // --- Alarm ---
  settings.alarm_hour = 6;
  settings.alarm_min  = 30;
  settings.sunrise_duration_min = 30;

  // Mo-Fr aktiv (bit0=Mo ... bit6=So) => 0b00011111
  settings.alarm_days_mask = 0b00011111;

  settings.snooze_minutes = 10;
  settings.alarm_max_duration_min = 20; // 0 = unbegrenzt

  // --- Nachtlicht ---
  settings.night_channel = NightChannel::Both;
  settings.night_override_enabled = false; // Standard: Poti
  settings.night_override_level = 15;      // fallback, falls Override an

  // --- Display ---
  settings.display_brightness = 100;        // tagsüber ruhig voll
  settings.display_r = 255;
  settings.display_g = 140;
  settings.display_b = 40;
  settings.auto_dim_display_at_night = true;
  settings.display_night_brightness = 10;

  // --- Audio (später) ---
  settings.audio_volume = 20; // 0..30
  settings.audio_folder = 1;
  settings.audio_track  = 1;

  // --- Zeit/Sync ---
  settings.time_source = 0;              // RTC
  settings.last_time_sync_epoch = 0;
}

void settings_load() {
  if (!has_init_flag()) {
    settings_reset_defaults();
    settings_save();
    return;
  }

  prefs.begin(NS, true);

  settings.alarm_hour = prefs.getUChar("ah", 6);
  settings.alarm_min  = prefs.getUChar("am", 30);
  settings.sunrise_duration_min = prefs.getUShort("sd", 30);
  settings.alarm_days_mask = prefs.getUChar("dm", 0b00011111);
  settings.snooze_minutes = prefs.getUChar("sz", 10);
  settings.alarm_max_duration_min = prefs.getUShort("mx", 20);

  settings.night_channel = (NightChannel)prefs.getUChar("nc", (uint8_t)NightChannel::Both);
  settings.night_override_enabled = prefs.getBool("noe", false);
  settings.night_override_level = prefs.getUChar("nol", 15);

  settings.display_brightness = prefs.getUChar("db", 100);
  settings.display_r = prefs.getUChar("dr", 255);
  settings.display_g = prefs.getUChar("dg", 140);
  settings.display_b = prefs.getUChar("dB", 40);
  settings.auto_dim_display_at_night = prefs.getBool("adn", true);
  settings.display_night_brightness = prefs.getUChar("dnb", 10);

  settings.audio_volume = prefs.getUChar("av", 20);
  settings.audio_folder = prefs.getUChar("af", 1);
  settings.audio_track  = prefs.getUShort("at", 1);

  settings.time_source = prefs.getUChar("ts", 0);
  settings.last_time_sync_epoch = prefs.getUInt("lse", 0);

  prefs.end();
}

void settings_save() {
  prefs.begin(NS, false);

  prefs.putBool("init", true);

  prefs.putUChar("ah", settings.alarm_hour);
  prefs.putUChar("am", settings.alarm_min);
  prefs.putUShort("sd", settings.sunrise_duration_min);
  prefs.putUChar("dm", settings.alarm_days_mask);
  prefs.putUChar("sz", settings.snooze_minutes);
  prefs.putUShort("mx", settings.alarm_max_duration_min);

  prefs.putUChar("nc", (uint8_t)settings.night_channel);
  prefs.putBool("noe", settings.night_override_enabled);
  prefs.putUChar("nol", settings.night_override_level);

  prefs.putUChar("db", settings.display_brightness);
  prefs.putUChar("dr", settings.display_r);
  prefs.putUChar("dg", settings.display_g);
  prefs.putUChar("dB", settings.display_b);
  prefs.putBool("adn", settings.auto_dim_display_at_night);
  prefs.putUChar("dnb", settings.display_night_brightness);

  prefs.putUChar("av", settings.audio_volume);
  prefs.putUChar("af", settings.audio_folder);
  prefs.putUShort("at", settings.audio_track);

  prefs.putUChar("ts", settings.time_source);
  prefs.putUInt("lse", settings.last_time_sync_epoch);

  prefs.end();
}
