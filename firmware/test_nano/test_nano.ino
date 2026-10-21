// Test matériel Nano ESP32 : LED RGB en séquence, message série, lecture A0.

const int PIN_VBAT = A0;
const float DIVISEUR = (47.0 + 22.0) / 22.0;

// LED RGB active à l'état bas
void ledCouleur(bool r, bool g, bool b) {
  digitalWrite(LED_RED,   r ? LOW : HIGH);
  digitalWrite(LED_GREEN, g ? LOW : HIGH);
  digitalWrite(LED_BLUE,  b ? LOW : HIGH);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  ledCouleur(0, 0, 0);
  analogReadResolution(12);
  delay(1500);
  Serial.println("=== Test Nano ESP32 OK ===");
}

void loop() {
  static int n = 0;
  n++;

  ledCouleur(1, 0, 0); Serial.println("LED rouge  (pompe 1)"); delay(700);
  ledCouleur(0, 1, 0); Serial.println("LED verte  (pompe 2)"); delay(700);
  ledCouleur(0, 0, 1); Serial.println("LED bleue  (pompe 3)"); delay(700);
  ledCouleur(0, 0, 0);

  digitalWrite(LED_BUILTIN, HIGH); delay(150);
  digitalWrite(LED_BUILTIN, LOW);

  uint32_t mv = 0;
  for (int i = 0; i < 16; i++) mv += analogReadMilliVolts(PIN_VBAT);
  float vA0 = (mv / 16) / 1000.0f;
  Serial.printf("Cycle %d  A0 = %.3f V  -> Vbat estimee = %.2f V  (0 si rien de branche)\n",
                n, vA0, vA0 * DIVISEUR);
  delay(1000);
}
