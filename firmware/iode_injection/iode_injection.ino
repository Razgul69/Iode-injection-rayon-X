// Iode injection rayon X — commande de 3 pompes péristaltiques DFR0523
// Carte : Arduino Nano ESP32 — Bibliothèque : ESP32Servo (firmware/libs)
//
// Interface : point d'accès WiFi "IodeInjection" → http://192.168.4.1 (ou http://iode.local)
// Tout le séquencement est cadencé par millis(), aucun delay() dans loop().
//
// Modules :
//   config.h      brochage, constantes, structure Config
//   Pompes        pilotage des 3 pompes avec minuterie individuelle
//   Sequenceur    délai initial → pompe → attente → pompe → attente → pompe
//   Stockage      persistance de Config en flash (NVS)
//   Reseau        WiFi AP + serveur HTTP + API JSON
//   page_web.h    interface web embarquée
//   Batterie.h    lecture Vbat via pont diviseur

#include "config.h"
#include "Pompes.h"
#include "Sequenceur.h"
#include "Stockage.h"
#include "Reseau.h"
#include "Batterie.h"

Config     cfg;
Stockage   stockage;
Pompes     pompes;
Sequenceur sequenceur(pompes, cfg);
Reseau     reseau(pompes, sequenceur, cfg, stockage);

// LED RGB du Nano ESP32 : active à l'état bas
void ledCouleur(bool r, bool g, bool b) {
  digitalWrite(LED_RED,   r ? LOW : HIGH);
  digitalWrite(LED_GREEN, g ? LOW : HIGH);
  digitalWrite(LED_BLUE,  b ? LOW : HIGH);
}

// Rouge / vert / bleu = pompe 1 / 2 / 3 en marche ; jaune clignotant = délai ou attente ; éteinte = repos
void majLed(uint32_t now) {
  static uint32_t tCligno = 0;
  static bool on = false;
  for (int i = 0; i < NB_POMPES; i++)
    if (pompes.enMarche(i)) { ledCouleur(i == 0, i == 1, i == 2); return; }
  if (sequenceur.active()) {
    if (now - tCligno >= 500) { tCligno = now; on = !on; }
    ledCouleur(on, on, 0);
    return;
  }
  ledCouleur(0, 0, 0);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  ledCouleur(0, 0, 0);
  analogReadResolution(12);

  stockage.charger(cfg);
  pompes.begin();
  reseau.begin();

  Serial.printf("Pret. Vbat = %.2f V (seuil %.1f V)\n", lireVbat(), cfg.vbatMin);
}

void loop() {
  uint32_t now = millis();
  sequenceur.update(now);
  pompes.update(now);
  reseau.update();
  majLed(now);
}
