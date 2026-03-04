#include <Arduino.h>
#include "config.h"
#include "settings.h"

#include <DFRobotDFPlayerMini.h>

static HardwareSerial mp3Serial(1);
static DFRobotDFPlayerMini df;
static bool g_ok = false;

bool audio_begin() {
  // DFPlayer Standard: 9600 baud
  mp3Serial.begin(9600, SERIAL_8N1, PIN_DF_RX, PIN_DF_TX);

  // DFPlayer init
  if (!df.begin(mp3Serial, true, true)) {
    g_ok = false;
    return false;
  }

  g_ok = true;

  // Einstellungen übernehmen falls geladen
  audio_set_volume(settings.audio_volume); // 0..30


  return true;
}

bool audio_ok() { return g_ok; }

void audio_set_volume(uint8_t v) {
  if (!g_ok) return;
  if (v > 30) v = 30;
  df.volume(v);
}

void audio_play(uint16_t track) {
  if (!g_ok) return;
  if (track == 0) return;
  // Im /mp3 Ordner wird per Nummer abgespielt: 1 => 0001.mp3
  df.playMp3Folder(track);
}

void audio_stop() {
  if (!g_ok) return;
  df.stop();
}
