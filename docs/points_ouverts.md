# Points à régler avant montage définitif

## 1. Tension minimale du DFR0831 (7 V) vs batterie 2S

Le DFR0831 demande **≥ 7 V** en entrée. Un pack 2S Li-ion passe sous 7 V (3,5 V/élément) alors qu'il reste encore ~30–40 % de capacité. Le Nano (VIN ≥ 6 V) continuera de fonctionner mais le 5 V des pompes ne sera plus garanti.

Options :

| Option | Avantage | Inconvénient |
|---|---|---|
| **a.** Garder le DFR0831, couper le cycle par firmware sous 7,0 V | Aucun composant à changer | ~1/3 de la capacité inexploitée |
| **b.** Remplacer par un **buck-boost 5 V / ≥ 3 A** | Exploite toute la plage 6–8,4 V | Nouveau module à qualifier |
| **c.** Passer en **3S** (11,1 V nominal) avec un BMS 3S | DFR0831 et Nano VIN très confortables | BMS à changer, 3 accus (3,5 Ah) ou 6 accus (3S2P) |

À tester sur table : relever la tension d'entrée à partir de laquelle le 5 V décroche avec une pompe en charge.

## 2. Position du fusible

Retenu : fusible T 3,15 A (TR5 dans MFH250) sur **P+** en sortie du BMS, avant l'interrupteur. B−, BM, B+ restent connectés directement au pack.

## 3. Recharge — décision : pas de recharge dans le boîtier

Les accus sont retirés des coupleurs et chargés dans un **chargeur 18650 externe**. Conséquences :

- Le couvercle doit rester facilement démontable (vis du G212CMF) et les coupleurs accessibles.
- Remettre les 4 accus **au même niveau de charge** avant de les réinsérer (les deux accus d'un groupe sont en parallèle direct).
- Le BMS reste utile pour la coupure basse tension, la surintensité et le court-circuit.

## 4. Seuil de coupure basse tension

La coupure du BMS est à **3,0 V/élément** (6,0 V pack) : protection ultime, pas un seuil d'usage. Le firmware refuse de lancer un cycle sous **6,4 V** (3,2 V/élément), ou **7,0 V** si l'option 1a est retenue.

## 5. Volume du boîtier G212CMF

Intérieur **109 × 84 × 49,5 mm**. Les 4 coupleurs BH18650-1 (76 × 21 × 21 mm) occupent déjà **84 × 76 mm au sol sur 21 mm de haut**. Il reste ≈ 28 mm de hauteur pour une plaquette portant Nano ESP32, DFR0831, BMS, porte-fusible et distribution 5 V.

- Les **3 pompes DFR0523 sont forcément à l'extérieur** du boîtier (passage par presse-étoupes).
- Le **Grove Shield pour Nano n'a pas sa place** dans ce volume ; le remplacer par une plaquette d'essai avec barrettes femelles pour le Nano, sur laquelle arrivent les picots Dupont des câbles FIT0896.
- Si l'intégration est trop serrée, passer au modèle supérieur de la gamme (G218CMF ou plus).

## 6. Étanchéité

Le G212CMF est IP65, mais chaque perçage la dégrade : prévoir **capuchon E1 (07024)** sur l'inverseur, **presse-étoupes PG7** pour les câbles pompes et le bouton, et un bouton poussoir étanche si monté en façade.

## 7. Connectique et shield

- Connecteur pompe = Gravity 3 broches PH2.0. Les câbles FIT0896 (37719) sont adaptés côté pompe.
- Côté Nano : picot Dupont **signal** sur la barrette ; fils **+ / −** des pompes soudés sur la distribution 5 V du DFR0831 (courant trop élevé pour un picot Dupont).

## 8. Comportement au démarrage

Au reset, les broches du Nano sont en haute impédance : le driver DFR0523 ne reçoit pas de signal et doit rester arrêté. Le firmware écrit 1500 µs sur les trois sorties dès `setup()`. À vérifier sur table.

## 9. Sens de rotation et calibration

- 500–1400 µs : sens horaire ; 1600–2500 µs : anti-horaire. Choisir le sens selon le montage du tube (`PPM_MARCHE` dans le firmware).
- Calibrer le volume délivré en 3 s pour chaque pompe avec le liquide réel, puis ajuster `DUREE_POMPE_MS` ou la vitesse.

## 10. Composants vérifiés (fiches Gotronic)

| Composant | Vérification | Résultat |
|---|---|---|
| Inverseur 8013 (07005) | Tenue en continu | **5 A / 28 Vcc** → OK pour ≈ 2,2 A pointe |
| BMS 2S (39402) | Courant max | **4 A**, surintensité 9,8 A → OK |
| MFH250 (09730) + fusible 09720 | Compatibilité | Tous deux au format **TR5** → OK |
| Coupleur BH18650-1 (09588) | Longueur accus | **65 mm uniquement** → accus non protégés |
| Boîtier G212CMF (11203) | Volume | 109 × 84 × 49,5 mm → très juste, pompes à l'extérieur |
