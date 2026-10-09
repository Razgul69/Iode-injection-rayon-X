# Nomenclature (commande Gotronic)

| Qté | Code Gotronic | Désignation | Réf. fabricant | Caractéristiques vérifiées (fiche Gotronic / DFRobot) | Rôle | Verdict |
|---|---|---|---|---|---|---|
| 1 | 39164 | Convertisseur de tension 5 Vcc / 4 A | DFRobot DFR0831 | Entrée 7–24 V, sortie 5 V / 4 A max (20 W), 6 sorties | Alimentation 5 V des pompes | OK — voir point 1 (7 V mini) |
| 3 | 35792 | Pompe péristaltique | DFRobot DFR0523 | 5–6 V, signal PPM type servo (500–1400 µs CW / 1400–1600 µs stop / 1600–2500 µs CCW), connecteur Gravity 3 broches PH2.0 | Injection séquentielle | OK |
| 1 | 39402 | Module de protection BMS 2S Li-ion | Futura 1606-2S4A18650 | **Courant max 4 A**, surintensité 9,8 A, coupure décharge 3,0 V/élément, surcharge 4,29 V, 34,5 × 16,2 mm | Protection / équilibrage du pack | OK — 4 A suffisants pour ≈ 2,2 A pointe |
| 8 | 09588 | Coupleur d'accu BH18650-1 | — | 1 accu, sortie sur fils 150 mm, **section 0,20 mm²**, **accus 65 mm uniquement** (non protégés) | Pack 2S2P (4 utilisés) | OK — voir remarques accus |
| 1 | 07005 | Inverseur unipolaire 8013 | — | SPDT ON-ON, **5 A / 28 Vcc**, 2 A / 250 Vac, cosses à souder, perçage 6,2 mm | Marche/arrêt général | OK — utiliser une seule voie ; capuchon étanche E1 (07024) pour garder l'IP65 |
| 1 | 37719 | 10 câbles 30 cm Gravity → 3 contacts mâles | DFRobot FIT0896 | PH2.0 3 broches d'un côté, 3 picots mâles Dupont indépendants de l'autre | Raccordement pompes | OK — signal uniquement ; voir remarque alimentation |
| 1 | 08913 | Pochette gaines thermo couleur GT100M | — | — | Isolation | OK |
| 1 | 09730 | Porte-fusible MFH250 | — | Pour **microfuses TR5**, 6,3 A max, montage CI | Fusible sur P+ | OK — nécessite une plaquette d'essai |
| 1 | 09720 | Fusible temporisé 3,15 A | — | **Microfuse TR5** Ø 8,5 mm, pas 5,08 mm | Protection alimentation | OK — compatible MFH250 |
| 2 m | 08946 | Fil FC50R | — | 0,50 mm² souple rouge, 16 × 0,20 mm | Câblage + | OK |
| 2 m | 08940 | Fil FC50N | — | 0,50 mm² souple noir | Câblage − | OK |
| 1 m | 08944 | Fil FC50B | — | 0,50 mm² souple | Point milieu BM | OK |
| 3 m | 08945 | Fil FC50V | — | 0,50 mm² souple vert | Signaux | OK |
| 1 | 11203 | Boîtier ABS étanche G212CMF | — | IP65, couvercle transparent, **extérieur 115 × 91,4 × 55 mm, intérieur 109 × 84 × 49,5 mm** | Intégration | **Très juste** — voir point 5 |
| 1 | 38251 | Dongle USB LoRaWAN LA66 | Dragino LA66 | — | Hors projet (autre usage) | — |

## À prévoir en plus (non dans la commande)

| Qté | Désignation | Rôle |
|---|---|---|
| 4 | Accus 18650 **non protégés, 65 mm**, flat-top, même lot (ex. 3 500 mAh) | Pack 2S2P |
| 1 | Arduino Nano ESP32 | Commande |
| 1 | Plaquette d'essai pastillée (~50 × 70 mm) + 2 barrettes femelles 15 pts | Support du Nano, porte-fusible, pont diviseur, répartition 5 V |
| 1 | Résistance 47 kΩ 1 % | Pont diviseur Vbat |
| 1 | Résistance 22 kΩ 1 % | Pont diviseur Vbat |
| 1 | Condensateur 100 nF | Filtrage A0 |
| 1 | Condensateur 470–1000 µF / 10 V | Réserve sur rail 5 V pompes |
| 1 | Bouton poussoir NO (étanche si monté sur le boîtier) | Déclenchement du cycle |
| 1 | Capuchon étanche E1 (Gotronic 07024) | Étanchéité de l'inverseur 8013 |
| 4–5 | Presse-étoupes PG7 | Passage des câbles pompes / bouton |
| 1 | Chargeur 18650 externe (2 ou 4 baies) | Recharge hors boîtier |

## Remarques de câblage issues des fiches

- **Coupleurs BH18650-1** : fils de 0,20 mm². En 2S2P chaque coupleur ne voit que la moitié du courant d'un groupe (≈ 0,8 A continu, 1,1 A pointe) : acceptable. Les liaisons communes (B−, BM, B+, P+, P−) se font en FC50 0,5 mm².
- **Accus** : les coupleurs n'acceptent que des cellules de **65 mm** → accus **non protégés** (les protégés font 67–70 mm). La protection est assurée par le BMS.
- **FIT0896** : les picots Dupont conviennent pour le signal. Pour le `+` et le `−` des pompes (1,8 A continu, 2,5 A pointe), préférer souder les fils rouge/noir du câble Gravity directement sur la plaquette de distribution 5 V plutôt que de passer par un picot Dupont.
- **Inverseur 8013** : inverseur ON-ON ; câbler le commun et une seule position. Le second contact reste libre.
- **MFH250 + fusible TR5** : composants pour circuit imprimé → à souder sur la plaquette d'essai.
