#pragma once
#include <stdint.h>

// Nachtlicht-Kanalwahl: keine Mischung, nur Warm oder Kalt oder Beide
enum class NightChannel : uint8_t { Warm = 0, Cold = 1, Both = 2 };

// Settings sind persistent (Flash/NVS) und überleben Hardware-Reset
struct Settings {
  // --- Alarm ---
  uint8_t  alarm_hour;              // 0..23
  uint8_t  alarm_min;               // 0..59
  uint16_t sunrise_duration_min;    // z.B. 1..120
  uint8_t  alarm_days_mask;         // bit0=Mo ... bit6=So
  uint8_t  snooze_minutes;          // 1..30
  uint16_t alarm_max_duration_min;  // 0 = unbegrenzt

  // --- Nachtlicht ---
  NightChannel night_channel;       // Warm / Cold / Both
  bool     night_override_enabled;  // false = Poti, true = App-Override
  uint8_t  night_override_level;    // 0..100 (nur relevant bei override=true)

  // --- Display (RGB Backlight + Dim) ---
  uint8_t  display_brightness;      // 0..100 (normal)
  uint8_t  display_r;               // 0..255
  uint8_t  display_g;               // 0..255
  uint8_t  display_b;               // 0..255
  bool     auto_dim_display_at_night;
  uint8_t  display_night_brightness;// 0..100

  // --- Audio (DFPlayer vorbereitet) ---
  uint8_t  audio_volume;            // 0..30 (DFPlayer)
  uint8_t  audio_folder;            // 1..99 (0 = nicht genutzt)
  uint16_t audio_track;             // 0 = kein Sound, sonst 1..xxx

  // --- Zeit/Sync (vorbereitet) ---
  uint8_t  time_source;             // 0=RTC, 1=App bevorzugt
  uint32_t last_time_sync_epoch;    // Info/Debug
};

extern Settings settings;

void settings_reset_defaults();
void settings_load();
void settings_save();
