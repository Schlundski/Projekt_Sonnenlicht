#include "curve.h"
#include <math.h>   // powf

float clamp01(float x) {
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;
  return x;
}

float curve_smoothstep(float x) {
  x = clamp01(x);
  return x * x * (3.0f - 2.0f * x);
}

float curve_gamma(float x, float gamma) {
  x = clamp01(x);
  if (gamma <= 0.0f) gamma = 1.0f; // Fallback
  return powf(x, gamma);
}

// Nachtlicht: unten fein, oben schneller
float curve_night(float x) {
  return curve_gamma(x, 2.6f);
}

// Sunrise: weich starten/enden, insgesamt "natürlich"
float curve_sunrise(float x) {
  // weiche Anfahr- und Abflach-Phase
  // Optional zusätzlich leichtes Gamma für gefühlte Helligkeit
  return curve_gamma(curve_smoothstep(x), 2.2f);
  // heller/früher: 1.8f
  // "dunkel" bleiben: 2.6f
}
