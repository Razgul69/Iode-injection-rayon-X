# Nomenclature

| Qté | Désignation | Référence | Réf. fournisseur | Rôle |
|---|---|---|---|---|
| 1 | Convertisseur DC-DC buck 7–24 V → 5 V / 4 A | DFRobot DFR0831 | 39164 | Alimentation 5 V des pompes |
| 3 | Pompe péristaltique numérique Gravity | DFRobot DFR0523 | 35792 | Injection séquentielle |
| 1 | BMS 2S avec équilibrage | HXYP 2S-A18 | 39402 | Protection / équilibrage du pack |
| 8 | Support 18650 | — | 09588 | Pack 2S2P (4 utilisés, 4 en réserve ou second pack) |
| 4 | Accu Li-ion 18650 (identiques, même lot, ~3 500 mAh) | — | — | Batterie 7,4 V nominal, ~7 Ah |
| 1 | Interrupteur | — | 07005 | Marche/arrêt général |
| 1 | Porte-fusible 5×20 | MFH250 | 09730 | Fusible sur P+ |
| 1 | Fusible temporisé 3,15 A 5×20 | — | 09720 | Protection alimentation |
| 1 | Boîtier | G212CMF | 11203 | Intégration |
| 1 | Arduino Nano ESP32 | ABX00083 | — | Commande |
| 1 | Grove Shield pour Arduino Nano | — | — | Connectique (signal uniquement) |
| 1 | Résistance 47 kΩ 1 % | — | — | Pont diviseur Vbat |
| 1 | Résistance 22 kΩ 1 % | — | — | Pont diviseur Vbat |
| 1 | Condensateur 100 nF | — | — | Filtrage A0 |
| 1 | Condensateur 470–1000 µF / 10 V | — | — | Réserve sur rail 5 V pompes (recommandé) |
| 1 | Bouton poussoir NO | — | — | Déclenchement du cycle (optionnel) |
| — | Câbles, gaines thermo, passe-fils, cosses | — | — | Câblage |

## Points de vérification sur les composants

- **Interrupteur 07005** : vérifier le courant admissible en **continu (DC)**, pas seulement en alternatif ; il doit tenir ≥ 3 A DC.
- **BMS 2S-A18** : vérifier le courant continu de décharge (doit être ≥ 5 A), le seuil de coupure basse tension et si le port est **commun** (charge par P+/P−) ou **séparé** (bornes de charge distinctes).
- **Accus 18650** : prendre des cellules identiques, du même lot, et les mettre au même niveau de tension avant de les mettre en parallèle.
- **Connectique pompes** : les DFR0523 utilisent un connecteur **Gravity 3 broches (PH2.0)**, différent du **Grove 4 broches**. Prévoir un câble adapté ou souder.
