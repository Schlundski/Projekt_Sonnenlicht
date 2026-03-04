#pragma once

void light_init();
void light_update_night();
void light_alarm_start();        // startet Sunrise+Alarm jetzt
void light_alarm_stop();         // stoppt komplett (aus)
void light_alarm_snooze(uint32_t now_unixtime); // setzt snooze_until und geht aus

bool light_alarm_active();
bool light_alarm_snoozing();

// wird regelmäßig im loop aufgerufen
void light_update_alarm(uint32_t now_unixtime);


