#pragma once
#include <Arduino.h>

// ---------- Matériel ----------
const int   NB_POMPES = 3;
const int   PIN_POMPE[NB_POMPES] = {D2, D4, D6};
const int   PIN_VBAT = A0;                                   // pont 47k (haut) / 22k (bas)
const float DIVISEUR_VBAT = (47.0f + 22.0f) / 22.0f;

// Protocole DFR0523 : 500–1400 µs horaire, 1400–1600 arrêt, 1600–2500 anti-horaire
const int PPM_STOP        = 1500;
const int PPM_HORAIRE     = 1000;
const int PPM_ANTIHORAIRE = 2000;

// Garde-fou : une pompe lancée "en continu" s'arrête seule au bout de ce délai
const uint32_t MARCHE_MAX_MS = 10UL * 60UL * 1000UL;

// ---------- Réseau (point d'accès WiFi) ----------
const char WIFI_SSID[] = "IodeInjection";
const char WIFI_MDP[]  = "iode1234";      // 8 caractères minimum
const char MDNS_NOM[]  = "iode";          // http://iode.local (sinon http://192.168.4.1)

// ---------- Réglages persistants (NVS) ----------
struct Config {
  uint32_t delaiInitial_s = 120;                       // avant la première pompe de la séquence
  uint8_t  ordre[NB_POMPES]     = {0, 1, 2};           // ordre[position] = index de pompe
  bool     actif[NB_POMPES]     = {true, true, true};  // actif[position] : la position participe à la séquence
  uint32_t attente_s[NB_POMPES] = {0, 60, 60};         // attente avant la pompe de cette position (ignorée pour la première)
  uint32_t duree_s[NB_POMPES]   = {3, 3, 3};           // duree_s[pompe]
  bool     inverse[NB_POMPES]   = {false, false, false}; // sens de rotation [pompe]
  float    vbatMin = 6.4f;                             // 0 = pas de contrôle batterie
};
