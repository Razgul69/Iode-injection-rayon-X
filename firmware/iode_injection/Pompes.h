#pragma once
#include <ESP32Servo.h>
#include "config.h"

// Pilotage des 3 pompes avec minuterie individuelle (millis).
class Pompes {
public:
  void begin();
  void marche(int i, uint32_t duree_ms, bool inverse);   // duree_ms = 0 : continu, borné par MARCHE_MAX_MS
  void arret(int i);
  void arretTous();
  void update(uint32_t now);
  bool enMarche(int i) const { return _actif[i]; }
  uint32_t restant_ms(int i, uint32_t now) const;

private:
  Servo    _servo[NB_POMPES];
  bool     _actif[NB_POMPES] = {};
  uint32_t _fin[NB_POMPES]   = {};
};
