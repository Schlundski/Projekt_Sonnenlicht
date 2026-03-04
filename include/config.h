// Pins, I2C, PWM-Limits, Standardwerte, Debug-Flags, Kurvenparameter

#pragma once
#include <Arduino.h>

// ------------------ Pins ------------------
constexpr int PIN_MOSFET_WARM = A0;
constexpr int PIN_MOSFET_COLD = A1;
constexpr int PIN_POTI = A3;
constexpr int PIN__ALARM_SNOOZE = D2;
constexpr int PIN_ALARM_STOP = D3;

// ------------------ LCD ------------------
constexpr uint8_t LCD_COLS = 16;
constexpr uint8_t LCD_ROWS = 2;

constexpr uint8_t LCD_R = 255;
constexpr uint8_t LCD_G = 0;
constexpr uint8_t LCD_B = 0;

// ------------------ Audio ------------------
constexpr int PIN_DF_RX = D9;   // empfangen DFPlayer TX
constexpr int PIN_DF_TX = D10;  // senden DFPlayer RX

// ------------------ Texte ------------------
static const char* const MONATE[] = {
  "Jan", "Feb", "Mär", "Apr", "Mai", "Jun",
  "Jul", "Aug", "Sep", "Okt", "Nov", "Dez"
};

static const char* const WOCHENTAGE[] = {
  "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
