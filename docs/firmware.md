# Firmware — architecture, interface web et API

## Choix : WiFi en point d'accès + page web embarquée

Le Nano ESP32 crée son propre réseau WiFi. Aucune box, aucune application à installer : un téléphone ou un PC se connecte au réseau et ouvre une page web. Plus simple et plus robuste sur le terrain que le Bluetooth (qui imposerait une application dédiée).

| Paramètre | Valeur | Où changer |
|---|---|---|
| SSID | `IodeInjection` | `config.h` → `WIFI_SSID` |
| Mot de passe | `iode1234` | `config.h` → `WIFI_MDP` |
| Adresse | http://192.168.4.1 ou http://iode.local | `config.h` → `MDNS_NOM` |

## Fonctions

- **Commande manuelle** : marche/arrêt de chaque pompe indépendamment, avec sa durée (0 = continu, coupé automatiquement après 10 min).
- **Séquence automatique** : délai initial (ex. 2 min) → pompe en position 1 → attente → pompe en position 2 → attente → pompe en position 3. Ordre des pompes, activation de chaque position, attentes et durées réglables.
- **Sens de rotation** par pompe.
- **Seuil batterie** : démarrage refusé sous ce seuil (0 = désactivé, pour les essais sur table).
- **Arrêt général** : coupe séquence et pompes immédiatement.
- Tous les réglages sont **persistants** (flash NVS) et survivent à la coupure.
- Tout le cadencement repose sur `millis()` : aucun `delay()` dans `loop()`, la page web reste réactive pendant une séquence.

## LED RGB du Nano

| État | LED |
|---|---|
| Repos | éteinte |
| Pompe 1 / 2 / 3 en marche | rouge / vert / bleu |
| Délai initial ou attente entre pompes | jaune clignotant (0,5 s) |

## Structure des sources

```
firmware/
├── iode_injection/
│   ├── iode_injection.ino   setup/loop, LED
│   ├── config.h             brochage, constantes, struct Config
│   ├── Pompes.h/.cpp        3 pompes, minuterie individuelle
│   ├── Sequenceur.h/.cpp    machine à états de la séquence
│   ├── Stockage.h           persistance NVS + validation
│   ├── Reseau.h/.cpp        WiFi AP, serveur HTTP, API JSON
│   ├── page_web.h           interface web (HTML/JS embarqué)
│   └── Batterie.h           lecture Vbat
├── test_nano/
│   └── test_nano.ino        test LED / série / A0
└── libs/                    bibliothèques tierces (voir README)
```

## Compilation et téléversement

Core **arduino:esp32** (version Arduino, pas espressif), carte `arduino:esp32:nano_nora`.

```powershell
$cli = "D:\Mes Programmes\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe"
& $cli compile --fqbn arduino:esp32:nano_nora --library firmware\libs\ESP32Servo firmware\iode_injection
& $cli upload  --fqbn arduino:esp32:nano_nora -p COM11 firmware\iode_injection
```

Si le téléversement échoue avec `exit status 74` / `LIBUSB_ERROR_PIPE` : double-clic rapide sur **RESET** (LED verte pulse = bootloader), puis relancer.

Moniteur série : `arduino-cli monitor -p COM11 --config baudrate=115200`. Le port USB disparaît à chaque reset du Nano ESP32 : ouvrir le moniteur **après** le démarrage.

## API HTTP

| Méthode | Route | Paramètres | Effet |
|---|---|---|---|
| GET | `/` | — | page de commande |
| GET | `/api/etat` | — | état complet (JSON) |
| POST | `/api/pompe` | `n=1..3`, `action=marche\|arret`, `duree=s` (0 = continu) | pompe individuelle |
| POST | `/api/sequence` | `action=start\|stop` | séquence |
| POST | `/api/config` | `delai`, `vbatMin`, `o1..o3`, `a1..a3`, `t1..t3`, `d1..d3`, `i1..i3` | réglages (sauvés en flash) |
| POST | `/api/stop` | — | arrêt général |

`o` = pompe (1..3) à la position, `a` = position active (0/1), `t` = attente avant la position (s), `d` = durée de la pompe (s), `i` = sens inversé (0/1).

Exemple :

```
curl -X POST "http://192.168.4.1/api/pompe?n=2&action=marche&duree=5"
curl -X POST "http://192.168.4.1/api/config?delai=120&t2=300&t3=300&d1=3&d2=3&d3=3"
curl -X POST "http://192.168.4.1/api/sequence?action=start"
```

Réponses d'erreur en texte clair (400 paramètre invalide, 409 conflit : séquence en cours, batterie faible).

## Test validé (21/10/2026)

Nano flashé, AP `IodeInjection` visible, page accessible, pompe 2 déclenchée depuis la page → LED verte 3 s.
