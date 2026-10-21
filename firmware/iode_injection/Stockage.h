#pragma once
#include <Preferences.h>
#include "config.h"

// Persistance de Config en flash (NVS), avec validation au chargement.
class Stockage {
public:
  void charger(Config& c) {
    Preferences p;
    p.begin("iode", true);
    if (p.getUInt("ver", 0) == VERSION && p.getBytesLength("cfg") == sizeof(Config))
      p.getBytes("cfg", &c, sizeof(Config));
    p.end();
    valider(c);
  }

  void sauver(const Config& c) {
    Preferences p;
    p.begin("iode", false);
    p.putUInt("ver", VERSION);
    p.putBytes("cfg", &c, sizeof(Config));
    p.end();
  }

  // Ordre = permutation de 0..NB_POMPES-1, durées non nulles et bornées.
  static void valider(Config& c) {
    bool vu[NB_POMPES] = {};
    bool ok = true;
    for (int i = 0; i < NB_POMPES; i++) {
      if (c.ordre[i] >= NB_POMPES || vu[c.ordre[i]]) ok = false;
      else vu[c.ordre[i]] = true;
    }
    if (!ok) for (int i = 0; i < NB_POMPES; i++) c.ordre[i] = i;

    const uint32_t maxS = MARCHE_MAX_MS / 1000;
    for (int i = 0; i < NB_POMPES; i++) {
      if (c.duree_s[i] == 0) c.duree_s[i] = 1;
      if (c.duree_s[i] > maxS) c.duree_s[i] = maxS;
    }
    if (c.vbatMin < 0) c.vbatMin = 0;
  }

private:
  static const uint32_t VERSION = 1;
};
