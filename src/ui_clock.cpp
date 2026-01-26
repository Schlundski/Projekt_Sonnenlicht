#include "ui_clock.h"

#include <Wire.h>
#include <elapsedMillis.h>
#include "Waveshare_LCD1602_RGB.h"

#include "config.h"
#include "rtc.h"

static elapsedMillis timer_lcd;
static Waveshare_LCD1602_RGB lcd_rgb(LCD_COLS, LCD_ROWS);

void ui_clock_begin() {
  lcd_rgb.init();
  lcd_rgb.setRGB(LCD_R, LCD_G, LCD_B);
  timer_lcd = 1000; // damit direkt beim Start einmal angezeigt wird
}

void ui_clock_update() {
  if (timer_lcd < 1000) return;
  timer_lcd = 0;

  DateTime now = rtc_now();

  char dayStr[18];
  char timeStr[19];

  // Hinweis: dein Original " %s der %02d. %s" kann >16 Zeichen werden und wird abgeschnitten.
  snprintf(dayStr, sizeof(dayStr), " %s %02d. %s",
           WOCHENTAGE[now.dayOfTheWeek()],
           now.day(),
           MONATE[now.month() - 1]);

  snprintf(timeStr, sizeof(timeStr), " %02d:%02d:%02d",
           now.hour(),
           now.minute(),
           now.second());

  lcd_rgb.setCursor(0, 0);
  lcd_rgb.send_string(dayStr);

  lcd_rgb.setCursor(0, 1);
  lcd_rgb.send_string(timeStr);
}
