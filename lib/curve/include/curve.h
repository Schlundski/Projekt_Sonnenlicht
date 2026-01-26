#pragma once

// clamp auf 0..1
float clamp01(float x);

// Formkurven
float curve_smoothstep(float x);          // 0..1 -> 0..1 (sanfter Start/Ende)
float curve_gamma(float x, float gamma);  // 0..1 -> 0..1 (Wahrnehmung)

// Spezielle Kurven
float curve_night(float x);               // fürs Nachtlicht (0..1 -> 0..1)
