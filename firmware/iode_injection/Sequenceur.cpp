#include "Sequenceur.h"

int Sequenceur::premierePositionActive(int depuis) const {
  for (int p = depuis; p < NB_POMPES; p++)
    if (_cfg.actif[p]) return p;
  return -1;
}

bool Sequenceur::demarrer(uint32_t now) {
  _pos = premierePositionActive(0);
  if (_pos < 0) return false;
  _pompes.arretTous();
  if (_cfg.delaiInitial_s > 0) {
    _phase = Phase::DELAI_INITIAL;
    _fin = now + _cfg.delaiInitial_s * 1000UL;
    Serial.printf("[%lu] Sequence : delai initial %lu s\n", (unsigned long)now, (unsigned long)_cfg.delaiInitial_s);
  } else {
    lancerPompe(now);
  }
  return true;
}

void Sequenceur::arreter(uint32_t now) {
  if (_phase == Phase::POMPAGE) _pompes.arret(pompeCourante());
  _phase = Phase::INACTIF;
  _pos = -1;
  Serial.printf("[%lu] Sequence arretee\n", (unsigned long)now);
}

void Sequenceur::entrerPosition(uint32_t now) {
  uint32_t att = _cfg.attente_s[_pos];
  if (att > 0) {
    _phase = Phase::ATTENTE;
    _fin = now + att * 1000UL;
    Serial.printf("[%lu] Attente %lu s avant pompe %d\n", (unsigned long)now, (unsigned long)att, pompeCourante() + 1);
  } else {
    lancerPompe(now);
  }
}

void Sequenceur::lancerPompe(uint32_t now) {
  int p = pompeCourante();
  uint32_t d = _cfg.duree_s[p] * 1000UL;
  _pompes.marche(p, d, _cfg.inverse[p]);
  _phase = Phase::POMPAGE;
  _fin = now + d;
}

void Sequenceur::update(uint32_t now) {
  if (!active()) return;
  if ((int32_t)(now - _fin) < 0) return;

  switch (_phase) {
    case Phase::DELAI_INITIAL:
    case Phase::ATTENTE:
      lancerPompe(now);
      break;

    case Phase::POMPAGE: {
      _pompes.arret(pompeCourante());
      int suivant = premierePositionActive(_pos + 1);
      if (suivant < 0) {
        _phase = Phase::TERMINE;
        _pos = -1;
        Serial.printf("[%lu] Sequence terminee\n", (unsigned long)now);
      } else {
        _pos = suivant;
        entrerPosition(now);
      }
      break;
    }

    default:
      break;
  }
}

uint32_t Sequenceur::restant_ms(uint32_t now) const {
  if (!active()) return 0;
  int32_t r = (int32_t)(_fin - now);
  return r > 0 ? (uint32_t)r : 0;
}

const char* Sequenceur::nomPhase() const {
  switch (_phase) {
    case Phase::DELAI_INITIAL: return "delai_initial";
    case Phase::ATTENTE:       return "attente";
    case Phase::POMPAGE:       return "pompage";
    case Phase::TERMINE:       return "termine";
    default:                   return "inactif";
  }
}
