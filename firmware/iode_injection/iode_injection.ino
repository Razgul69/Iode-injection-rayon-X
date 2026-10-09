// Iode injection rayon X — séquence de 3 pompes péristaltiques DFR0523
// Carte : Arduino Nano ESP32 (core Arduino ESP32 >= 3.x)
// Bibliothèque : ESP32Servo (gestionnaire de bibliothèques Arduino)
//
// Protocole DFR0523 (signal PPM type servo, période 20 ms) :
//   500–1400 µs : sens horaire      1400–1600 µs : arrêt      1600–2500 µs : sens anti-horaire

#include <ESP32Servo.h>

// ---- Brochage ----
const int PIN_POMPE[3] = {D2, D4, D6};
const int PIN_BOUTON   = D7;   // bouton poussoir NO vers GND, lance un cycle
const int PIN_VBAT     = A0;   // pont diviseur 47k (haut) / 22k (bas)

// ---- Paramètres ----
const unsigned long DUREE_POMPE_MS = 3000;  // durée de marche de chaque pompe
const unsigned long PAUSE_MS       = 500;   // arrêt entre deux pompes
const int   PPM_STOP   = 1500;   // zone d'arrêt 1400–1600 µs
const int   PPM_MARCHE = 1000;   // sens horaire ; 500 = vitesse max, 2000 pour inverser le sens
const float VBAT_MIN   = 6.4;    // refus de cycle sous ce seuil (3,2 V/élément) — 7.0 si DFR0831 conservé
const float DIVISEUR   = (47.0 + 22.0) / 22.0;

Servo pompe[3];

enum Etat { REPOS, POMPAGE, PAUSE };
Etat etat = REPOS;
int indexPompe = 0;
unsigned long t0 = 0;
bool boutonPrecedent = HIGH;

// LED RGB du Nano ESP32 : active à l'état bas
void ledCouleur(bool r, bool g, bool b) {
  digitalWrite(LED_RED,   r ? LOW : HIGH);
  digitalWrite(LED_GREEN, g ? LOW : HIGH);
  digitalWrite(LED_BLUE,  b ? LOW : HIGH);
}

void afficherPompe(int i) {
  switch (i) {
    case 0:  ledCouleur(1, 0, 0); break;  // pompe 1 : rouge
    case 1:  ledCouleur(0, 1, 0); break;  // pompe 2 : vert
    case 2:  ledCouleur(0, 0, 1); break;  // pompe 3 : bleu
    default: ledCouleur(0, 0, 0);
  }
}

void toutesStop() {
  for (int i = 0; i < 3; i++) pompe[i].writeMicroseconds(PPM_STOP);
}

float lireVbat() {
  uint32_t mv = 0;
  for (int i = 0; i < 16; i++) mv += analogReadMilliVolts(PIN_VBAT);
  return (mv / 16) / 1000.0f * DIVISEUR;
}

void signalerBatterieFaible() {
  for (int i = 0; i < 6; i++) {
    ledCouleur(1, 0, 0); delay(150);
    ledCouleur(0, 0, 0); delay(150);
  }
}

void demarrerPompe(int i) {
  pompe[i].writeMicroseconds(PPM_MARCHE);
  afficherPompe(i);
  Serial.printf("Pompe %d ON\n", i + 1);
}

void arreterPompe(int i) {
  pompe[i].writeMicroseconds(PPM_STOP);
  ledCouleur(0, 0, 0);
  Serial.printf("Pompe %d OFF\n", i + 1);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  ledCouleur(0, 0, 0);

  pinMode(PIN_BOUTON, INPUT_PULLUP);
  analogReadResolution(12);

  for (int i = 0; i < 3; i++) {
    pompe[i].setPeriodHertz(50);
    pompe[i].attach(PIN_POMPE[i], 500, 2500);
  }
  toutesStop();  // arrêt explicite dès la mise sous tension

  Serial.printf("Pret. Vbat = %.2f V\n", lireVbat());
}

void loop() {
  unsigned long now = millis();

  switch (etat) {
    case REPOS: {
      bool bouton = digitalRead(PIN_BOUTON);
      if (boutonPrecedent == HIGH && bouton == LOW) {   // front descendant
        delay(30);                                        // anti-rebond
        if (digitalRead(PIN_BOUTON) == LOW) {
          float v = lireVbat();
          Serial.printf("Vbat = %.2f V\n", v);
          if (v < VBAT_MIN) {
            Serial.println("Batterie faible : cycle refuse");
            signalerBatterieFaible();
          } else {
            indexPompe = 0;
            demarrerPompe(indexPompe);
            etat = POMPAGE;
            t0 = now;
          }
        }
      }
      boutonPrecedent = bouton;
      break;
    }

    case POMPAGE:
      if (now - t0 >= DUREE_POMPE_MS) {
        arreterPompe(indexPompe);
        indexPompe++;
        if (indexPompe >= 3) {
          etat = REPOS;
          Serial.println("Cycle termine");
        } else {
          etat = PAUSE;
          t0 = now;
        }
      }
      break;

    case PAUSE:
      if (now - t0 >= PAUSE_MS) {
        demarrerPompe(indexPompe);
        etat = POMPAGE;
        t0 = now;
      }
      break;
  }
}
