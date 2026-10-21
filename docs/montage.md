# Procédure de montage et de test

Ordre de câblage avec un test à chaque étape. Multimètre : cordon rouge dans VΩ, noir dans COM, sélecteur V⎓ 20 V sauf mention contraire.

## Vue d'ensemble

```
PACK 2S2P ──► BMS ──P+──► FUSIBLE T3,15A ──► INTERRUPTEUR ──┬──► Nano VIN
                   │                                         ├──► DFR0831 IN+
                   │                                         └──► 47 kΩ ─┬─► A0
                   P−──────────────────────────────── GND ───────────────┤  22 kΩ + 100 nF
                                                       │                 │
                                                       ├──► Nano GND     GND
                                                       └──► DFR0831 IN−

DFR0831 5 V ──┬──► Pompe 1 (+)      Nano D2 ──► Pompe 1 (S)
              ├──► Pompe 2 (+)      Nano D4 ──► Pompe 2 (S)
              └──► Pompe 3 (+)      Nano D6 ──► Pompe 3 (S)
DFR0831 GND ──┴──► Pompes 1-2-3 (−)   + condo 470–1000 µF entre 5 V et GND

Commande : WiFi "IodeInjection" → http://192.168.4.1 (aucun bouton physique)
```

Règle : une seule masse commune (P−, IN−, GND DFR0831, GND Nano, − des pompes, bas du pont diviseur). Les pompes ne tirent jamais leur 5 V du Nano.

## Étape 0 — Pack batterie ✅

1. BMS : B− → négatif groupe A, BM → jonction (+A / −B), B+ → positif groupe B. Fils soudés sur P+ et P−, carte sous gaine thermo. Ne plus jamais sonder les pads directement.
2. Sans accus, ohmmètre : B+↔B−, B+↔BM, BM↔B−, P+↔BM ouverts ; P+↔B+ continuité.
3. Accus appairés (± 0,05 V), 65 mm non protégés. Insérer groupe A puis groupe B.
4. Mesures : B−→BM et BM→B+ ≈ 4,0–4,2 V (écart < 0,1 V) ; B−→B+ = somme ; P−→P+ = B−→B+ ± 0,05 V.
5. Si P−→P+ = 0 V (verrou de protection à la première mise sous tension) : retirer/remettre un accu 10 s, sinon fil bref 1 s entre P− et B−, sinon chargeur 8,4 V sur P+/P− 5–10 s.

## Étape 1 — Flasher et valider le Nano ✅

Nano seul sur USB (voir [firmware.md](firmware.md) pour la compilation). Après démarrage : WiFi `IodeInjection` visible, page http://192.168.4.1 accessible, **Seuil batterie = 0** pour les essais sans pack. Marche pompe 1/2/3 → LED rouge/vert/bleu. Les 3 sorties sont à 1500 µs (arrêt) dès la mise sous tension.

## Étape 2 — Fusible + interrupteur

Sur une plaquette d'essai : souder le MFH250, clipser le fusible TR5 3,15 A.

- P+ → borne 1 du porte-fusible.
- Borne 2 → borne centrale (commun) de l'inverseur 8013.
- Une borne latérale de l'inverseur → nœud **+BAT**. L'autre reste libre.
- P− → nœud **GND**.

Test : OFF → 0 V entre +BAT et GND ; ON → tension pack. Fusible froid.

## Étape 3 — DFR0831

+BAT → IN+, GND → IN−. Rien sur les sorties.

Test : ON → 4,9–5,1 V en sortie ; OFF → 0 V.

## Étape 4 — Nano ESP32

+BAT → VIN, GND → GND. Ne rien brancher sur 5V ni 3V3 du Nano. Débrancher l'USB.

Test : ON → LED d'alimentation allumée, LED RGB éteinte, WiFi `IodeInjection` visible, page accessible.

## Étape 5 — Mesure batterie

+BAT → 47 kΩ → point A0 → 22 kΩ → GND ; 100 nF entre A0 et GND. Point milieu → A0 du Nano.

Test : « Batterie » sur la page = multimètre sur +BAT à ± 0,1 V. Sinon corriger `DIVISEUR_VBAT` dans `config.h` par le rapport Vmultimètre / Vlue. Remettre ensuite **Seuil batterie = 6,4 V** dans la page.

## Étape 6 — Pompes, une par une

Câble FIT0896 : repérer S / + / − sur le connecteur de la pompe (pas les couleurs).

- S → D2 (pompe 1), D4 (pompe 2), D6 (pompe 3).
- + → 5 V du DFR0831 (soudé ou bornier, pas de picot Dupont pour 2 A).
- − → GND du DFR0831.
- Condensateur 470–1000 µF / 10 V au plus près des bornes des pompes, polarité respectée.

Test après chaque pompe : ON → la pompe ne tourne pas. Marche depuis la page (3 s) → elle tourne puis s'arrête. Tourne à la mise sous tension → signal mal branché. Sens inverse → cocher « Pompe n inversée » et Enregistrer.

Séquence complète avec les 3 (délai 5 s, attentes 5 s pour l'essai) : délai → pompe 1 → attente → pompe 2 → attente → pompe 3 → terminée. 5 V du DFR0831 > 4,8 V pendant qu'une pompe tourne. Noter la tension +BAT à partir de laquelle le 5 V décroche (ajuster le seuil batterie).

Calibration : mesurer le volume délivré par chaque pompe en 10 s (balance ou éprouvette), en déduire la durée à régler pour le volume voulu.

## Étape 7 — Intégration G212CMF

- Fond : 4 coupleurs (accus retirables pour recharge externe).
- Au-dessus / côté : plaquette (Nano, fusible, pont diviseur, condensateur), DFR0831, BMS sous gaine.
- Façade : inverseur 8013 + capuchon E1.
- 3 presse-étoupes PG7 pour les câbles pompes (pompes à l'extérieur). Gaine thermo sur toutes les jonctions.
- Avant fermeture : séquence complète depuis le téléphone, aucun fil ne touche les contacts des coupleurs. Vérifier la portée WiFi boîtier fermé (le polycarbonate ne gêne pas).

## Sécurité

- Interrupteur OFF et accus retirés avant toute soudure.
- Ne jamais poser une pointe de multimètre sur les pads du BMS (P+ et BM sont à 1 mm : un dérapage court-circuite le groupe B, hors protection).
- Vérifier le mode du multimètre (V⎓, cordon dans VΩ) avant de toucher le pack : en mode ampèremètre il court-circuite ce qu'il mesure.
- Seuil batterie : démarrage refusé sous la valeur réglée dans la page (6,4 V, ou 7,0 V si le DFR0831 décroche avant). Une pompe en continu s'arrête seule après 10 min.
