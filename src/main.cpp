#include <Arduino.h>
#include <Wire.h>
#include <curve.h>

#include "config.h"
#include "rtc.h"
#include "ui_clock.h"
#include "settings.h"
#include "input.h"
#include "light.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();

  settings_load();
  input_init();
  light_init();
  ui_clock_begin();

  if (!rtc_begin(true)) {   // true = Uhr stellen (NUR EINMAL!)
    Serial.println("RTC NICHT gefunden!");
    while (1) {}
  }
  Serial.println("RTC gefunden.");
}

void loop() {
  // put your main code here, to run repeatedly:
    ui_clock_update();
    light_update_night(); 
}