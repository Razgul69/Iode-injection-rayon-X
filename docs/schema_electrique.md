# Schéma électrique

Les accus restent hors circuit tant que le câblage et les polarités ne sont pas vérifiés.

## A. Batterie 2S2P et BMS

```
       GROUPE A : 2 accus en parallèle
       3,7 V nominal — capacité doublée

       Accu 1                 Accu 2
    (+) rouge ───────────────── rouge (+)
    (-) noir  ───────────────── noir  (-)
        │                         │
        └── négatifs communs ─────┴────── B− BMS
        └── positifs communs ──────────── BM BMS
                       │
                       └──────────────┐
                                      │ POINT MILIEU
                                      │
       GROUPE B : 2 accus en parallèle
       3,7 V nominal — capacité doublée

       Accu 3                 Accu 4
    (+) rouge ───────────────── rouge (+)
    (-) noir  ───────────────── noir  (-)
        │                         │
        └── négatifs communs ─────┴────── POINT MILIEU
        └── positifs communs ──────────── B+ BMS
```

Connexions batterie → BMS :

- **B−** : négatif commun du groupe A.
- **BM** : positif du groupe A relié au négatif du groupe B (point milieu).
- **B+** : positif commun du groupe B.

Le BMS fournit la sortie protégée **P+ / P−**. Ordre de câblage conseillé : B− d'abord, puis BM, puis B+ (le BMS s'initialise correctement).

## B. Alimentation générale

```
             BATTERIE 2S2P
                  │
                  ▼
             BMS HXYP 2S-A18
                  │
             P+   │   P−
              │   │    └─────────────────────────┐
              ▼   │                              │
        Fusible T 3,15 A                         │
              │                                  │
              ▼                                  │
        Interrupteur                             │
              │                                  │
              ├───────────────┐                  │
              │               │                  │
              ▼               ▼                  │
         Nano ESP32       DFR0831                │
            VIN           IN+                    │
            GND ───────┐   IN− ──────────────────┘
                       │
                       │              DFR0831
                       │           sortie 5 V / GND
                       │                  │
                       │       ┌──────────┼──────────┐
                       │       ▼          ▼          ▼
                       │    Pompe 1    Pompe 2    Pompe 3
                       │     + / −      + / −      + / −
                       │
                       └── masse commune avec GND DFR0831

         Nano D2 ── signal PPM pompe 1
         Nano D4 ── signal PPM pompe 2
         Nano D6 ── signal PPM pompe 3
```

- Les trois pompes sont alimentées **en parallèle** sur la sortie 5 V du DFR0831 (6 sorties disponibles sur le module).
- Le Nano ne fournit que les signaux de commande ; il ne doit pas alimenter les moteurs.
- Le fusible est placé sur **P+**, en sortie du BMS, avant l'interrupteur. B−, BM et B+ restent reliés directement au pack.
- Ajouter un condensateur **470–1000 µF / 10 V** sur le rail 5 V au plus près des pompes pour encaisser les pointes de courant (2,5 A au démarrage).

### Bilan de courant

| Point | Valeur |
|---|---|
| Pompe active (une seule à la fois) | 1,8 A continu, 2,5 A pointe sous 5 V |
| Entrée DFR0831 à 7 V (rendement ~90 %) | ≈ 1,4 A continu, ≈ 2 A pointe |
| Nano ESP32 (Wi-Fi coupé) | ≈ 0,1–0,2 A |
| Total côté batterie | ≈ 1,6 A continu, ≈ 2,2 A pointe → fusible T 3,15 A adapté |

## C. Connexions de chaque pompe

| Pompe | Signal Nano | Alimentation |
|---|---|---|
| Pompe 1 | D2 | 5 V DFR0831 / GND |
| Pompe 2 | D4 | 5 V DFR0831 / GND |
| Pompe 3 | D6 | 5 V DFR0831 / GND |

Pour chaque câble Gravity 3 contacts :

- `D` → broche de commande du Nano.
- `+` → sortie 5 V du DFR0831.
- `−` → GND du DFR0831.

Toutes les masses communes : P− du BMS, IN− et GND du DFR0831, GND des pompes, GND du Nano. Vérifier le repérage des contacts sur les connecteurs ; ne pas se fier uniquement aux couleurs des fils.

Si le Grove Shield est utilisé pour la commodité des connecteurs, **n'utiliser que la broche signal** : la broche VCC du shield (3,3 V ou 5 V du Nano) ne doit pas être reliée au `+` des pompes.

## D. Mesure de la tension batterie

```
Après interrupteur (+)
          │
        47 kΩ
          │
          ├────────────── A0 du Nano
          │
       22 kΩ
          │
         GND

100 nF entre A0 et GND
```

- Rapport : 22 / (47 + 22) = 0,319 → **2,68 V sur A0 à 8,4 V**, 1,91 V à 6,0 V.
- Consommation du pont : ≈ 120 µA, uniquement interrupteur fermé.
- Ne jamais relier la batterie directement à A0 (entrées 3,3 V).
- L'ADC de l'ESP32 n'est pas parfaitement linéaire : utiliser `analogReadMilliVolts()` (calibration usine) et corriger avec une mesure au multimètre.

## E. Bouton de déclenchement (firmware fourni)

```
Nano D7 ────┤ bouton poussoir NO ├──── GND
(INPUT_PULLUP interne)
```
