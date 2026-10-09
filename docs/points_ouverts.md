# Points à régler avant montage définitif

## 1. Tension minimale du DFR0831 (7 V) vs batterie 2S

Le DFR0831 demande **≥ 7 V** en entrée. Un pack 2S Li-ion passe sous 7 V (3,5 V/élément) alors qu'il reste encore ~30–40 % de capacité. Le Nano (VIN ≥ 6 V) continuera de fonctionner mais le 5 V des pompes ne sera plus garanti.

Options :

| Option | Avantage | Inconvénient |
|---|---|---|
| **a.** Garder le DFR0831, couper le cycle par firmware sous 7,0 V | Aucun composant à changer | ~1/3 de la capacité inexploitée |
| **b.** Remplacer par un **buck-boost 5 V / ≥ 3 A** | Exploite toute la plage 6–8,4 V | Nouveau module à qualifier |
| **c.** Passer en **3S** (11,1 V nominal, 3 accus en série) avec un BMS 3S | DFR0831 et Nano VIN très confortables sur toute la décharge | BMS à changer, capacité 3,5 Ah au lieu de 7 Ah (ou 6 accus en 3S2P) |

À tester sur table : relever la tension d'entrée à partir de laquelle le 5 V décroche avec une pompe en charge.

## 2. Position du fusible

Retenu : fusible T 3,15 A sur **P+** en sortie du BMS, avant l'interrupteur. B−, BM, B+ restent connectés directement au pack pour que le BMS surveille chaque groupe.

## 3. Recharge du pack — non prévue dans la liste actuelle

Le BMS 2S-A18 protège mais **ne charge pas**. Il faut choisir :

- **Charge dans le boîtier** : chargeur **8,4 V CC/CV** (2S Li-ion) raccordé sur P+/P− si le BMS est à port commun, via une prise jack dédiée. Vérifier le courant de charge admissible par le BMS.
- **Charge externe** : retirer les accus des supports et les charger dans un chargeur 18650 indépendant. Plus simple mais nécessite d'ouvrir le boîtier.

## 4. Seuil de coupure basse tension

La coupure du BMS (typiquement 2,5–3,0 V/élément) est une protection ultime, pas un seuil d'usage. Le firmware doit refuser de lancer un cycle sous **6,4 V** (3,2 V/élément) pour préserver les accus, ou **7,0 V** si l'option 1a est retenue.

## 5. Connectique et shield Grove

- Connecteur pompe = Gravity 3 broches PH2.0 ≠ Grove 4 broches. Prévoir des câbles adaptés.
- Si le shield Grove est utilisé, ne câbler que le signal ; la broche VCC du shield ne doit pas alimenter les pompes.

## 6. Comportement au démarrage

Au reset, les broches du Nano sont en haute impédance : le driver DFR0523 ne reçoit pas de signal et doit rester arrêté. Le firmware écrit 1500 µs sur les trois sorties dès `setup()`. À vérifier sur table qu'aucune pompe ne bouge à la mise sous tension.

## 7. Sens de rotation et calibration

- 500–1400 µs : sens horaire ; 1600–2500 µs : anti-horaire. Choisir le sens selon le montage du tube (`PPM_MARCHE` dans le firmware).
- Calibrer le volume délivré en 3 s pour chaque pompe avec le liquide réel (mesure à la balance ou éprouvette), puis ajuster `DUREE_POMPE_MS` ou la vitesse si besoin.

## 8. Vérifications composants

- Interrupteur 07005 : tenue ≥ 3 A **DC**.
- BMS : courant de décharge continu ≥ 5 A.
- Accus : identiques, même lot, équilibrés avant mise en parallèle.
- Condensateur réservoir 470–1000 µF sur le 5 V des pompes.
