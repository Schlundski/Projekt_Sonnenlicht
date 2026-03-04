#pragma once
#include <stdint.h>

bool audio_begin();                 // init + Karte prüfen
void audio_set_volume(uint8_t v);   // lautstärke
void audio_play(uint16_t track);    // spielt mp3s über nummer
bool audio_ok();                    // init erfolgreich erfolgreich abfrage


