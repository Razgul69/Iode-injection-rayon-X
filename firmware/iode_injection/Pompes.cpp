#include "Pompes.h"

void Pompes::begin() {
  for (int i = 0; i < NB_POMPES; i++) {
    _servo[i].setPeriodHertz(50);
    _servo[i].attach(PIN_POMPE[i], 500, 2500);
    _servo[i].writeMicroseconds(PPM_STOP);   // arrêt explicite dès la mise sous tension
  }
}

void Pompes::marche(int i, uint32_t duree_ms, bool inverse) {
  if (i < 0 || i >= NB_POMPES) return;
  if (duree_ms == 0 || duree_ms > MARCHE_MAX_MS) duree_ms = MARCHE_MAX_MS;
  _servo[i].writeMicroseconds(inverse ? PPM_ANTIHORAIRE : PPM_HORAIRE);
  _actif[i] = true;
  _fin[i] = millis() + duree_ms;
  Serial.printf("[%lu] Pompe %d ON (%lu ms)\n", millis(), i + 1, (unsigned long)duree_ms);
}

void Pompes::arret(int i) {
  if (i < 0 || i >= NB_POMPES) return;
  _servo[i].writeMicroseconds(PPM_STOP);
  if (_actif[i]) Serial.printf("[%lu] Pompe %d OFF\n", millis(), i + 1);
  _actif[i] = false;
}

void Pompes::arretTous() {
  for (int i = 0; i < NB_POMPES; i++) arret(i);
}

void Pompes::update(uint32_t now) {
  for (int i = 0; i < NB_POMPES; i++)
    if (_actif[i] && (int32_t)(now - _fin[i]) >= 0) arret(i);
}

uint32_t Pompes::restant_ms(int i, uint32_t now) const {
  if (!_actif[i]) return 0;
  int32_t r = (int32_t)(_fin[i] - now);
  return r > 0 ? (uint32_t)r : 0;
}
