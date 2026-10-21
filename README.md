# Iode injection rayon X

Boîtier portable sur batterie pilotant **3 pompes péristaltiques DFRobot DFR0523** depuis une **interface web WiFi** (le Nano ESP32 crée son propre réseau). Commande manuelle de chaque pompe, séquence automatique à retard (délai initial → pompe → attente → pompe → attente → pompe), réglages persistants, tout cadencé par `millis()`.

| Document | Contenu |
|---|---|
| [docs/firmware.md](docs/firmware.md) | Architecture du firmware, interface web, API HTTP, compilation |
| [docs/montage.md](docs/montage.md) | Procédure de montage et de test étape par étape |
| [docs/nomenclature.md](docs/nomenclature.md) | Liste du matériel avec codes Gotronic vérifiés |
| [docs/schema_electrique.md](docs/schema_electrique.md) | Schéma électrique (batterie, BMS, alimentation, pompes, Vbat) |
| [docs/points_ouverts.md](docs/points_ouverts.md) | Points à régler / vérifier avant montage définitif |
| [firmware/iode_injection/](firmware/iode_injection/) | Firmware Arduino Nano ESP32 (modulaire) |
| [firmware/test_nano/](firmware/test_nano/) | Sketch de test LED / série / A0 |

## 1. Principe

```mermaid
flowchart LR
    BAT["Batterie 2S2P<br/>4 × 18650 — 7,4 V nom."] --> BMS["BMS HXYP 2S-A18"]
    BMS -->|P+| F["Fusible T 3,15 A"] --> SW["Interrupteur"]
    SW --> NANO["Arduino Nano ESP32<br/>VIN"]
    SW --> DCDC["DFR0831<br/>7–24 V → 5 V / 4 A"]
    DCDC -->|5 V| P1["Pompe 1"]
    DCDC -->|5 V| P2["Pompe 2"]
    DCDC -->|5 V| P3["Pompe 3"]
    NANO -. D2 PPM .-> P1
    NANO -. D4 PPM .-> P2
    NANO -. D6 PPM .-> P3
    SW -->|47k / 22k| A0["A0 — mesure Vbat"]
    TEL["Téléphone / PC"] -. WiFi AP<br/>http://192.168.4.1 .-> NANO
```

## 2. Utilisation

1. Interrupteur ON. Le Nano crée le WiFi **`IodeInjection`** (mot de passe `iode1234`).
2. Se connecter à ce réseau, ouvrir **http://192.168.4.1** (ou http://iode.local).
3. **Commande manuelle** : Marche/Arrêt de chaque pompe avec sa durée.
4. **Séquence automatique** : délai initial (ex. 120 s), ordre des pompes, attente avant chacune, durées ; Enregistrer puis Démarrer. La page affiche la phase, le temps restant et la pompe active.
5. **ARRÊT GÉNÉRAL** coupe tout immédiatement.

LED du Nano : rouge / vert / bleu = pompe 1 / 2 / 3 en marche, jaune clignotant = délai ou attente, éteinte = repos.

Les débits réels doivent être **calibrés avec le liquide utilisé** (volume par seconde de chaque pompe).

## 3. Caractéristiques clés (fiches constructeur)

| Élément | Donnée |
|---|---|
| DFR0523 | Alimentation 5–6 V, signal PPM type servo : 500–1400 µs horaire, 1400–1600 µs arrêt, 1600–2500 µs anti-horaire. Logique 3,3 V OK. |
| DFR0831 | Entrée 7–24 V, sortie 5 V / 4 A max. |
| BMS 2S (Gotronic 39402) | 4 A continu, surintensité 9,8 A, coupure 3,0 V/élément. |
| Nano ESP32 | VIN 6–21 V, E/S 3,3 V, LED RGB intégrée (active à l'état bas), WiFi. |

## 4. État d'avancement

- [x] Pack 2S2P + BMS câblé, testé (8,18 V sur P+/P− après réveil du BMS)
- [x] Firmware flashé, point d'accès WiFi et page web validés, LED OK
- [ ] Fusible + interrupteur + DFR0831 + Nano sur plaquette
- [ ] Pont diviseur Vbat, calibration `DIVISEUR_VBAT`
- [ ] Raccordement des 3 pompes, calibration des volumes
- [ ] Intégration dans le G212CMF

## 5. Sécurité

- Accus hors circuit tant que câblage et polarités ne sont pas vérifiés.
- Masses communes (P−, IN−/GND DFR0831, GND pompes, GND Nano). Les pompes ne sont **jamais** alimentées par le Nano.
- Ne jamais poser une pointe de multimètre sur les pads du BMS ; mesurer sur les fils.
- Séparer l'hydraulique de l'électronique dans le boîtier.
