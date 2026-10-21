# Bibliothèques tierces

Ce dossier reçoit les bibliothèques externes nécessaires au firmware, pour compiler sans dépendre du dossier `Documents/Arduino/libraries` (utile si un antivirus bloque l'écriture).

| Bibliothèque | Version testée | Source | Licence |
|---|---|---|---|
| ESP32Servo | 3.2.1 | https://github.com/madhephaestus/ESP32Servo | LGPL-2.1 |

Installation :

```powershell
# Depuis la racine du dépôt
arduino-cli lib download ESP32Servo@3.2.1
Expand-Archive "$env:LOCALAPPDATA\Arduino15\staging\libraries\ESP32Servo-3.2.1.zip" firmware\libs\
```

ou télécharger le zip depuis GitHub et l'extraire ici de sorte à obtenir `firmware/libs/ESP32Servo/src/ESP32Servo.h`.

Le contenu de ce dossier (hors ce README) est ignoré par git.
