#pragma once
#include "config.h"

inline float lireVbat() {
  uint32_t mv = 0;
  for (int i = 0; i < 16; i++) mv += analogReadMilliVolts(PIN_VBAT);
  return (mv / 16) / 1000.0f * DIVISEUR_VBAT;
}
