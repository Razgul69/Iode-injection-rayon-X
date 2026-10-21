#pragma once
#include "config.h"
#include "Pompes.h"

enum class Phase : uint8_t { INACTIF, DELAI_INITIAL, ATTENTE, POMPAGE, TERMINE };

// Séquence : délai initial → pompe(pos 0) → attente → pompe(pos 1) → attente → pompe(pos 2).
// Les positions inactives sont sautées. Tout est cadencé par millis().
class Sequenceur {
public:
  Sequenceur(Pompes& p, const Config& c) : _pompes(p), _cfg(c) {}
  bool demarrer(uint32_t now);                 // false si aucune position active
  void arreter(uint32_t now);
  void update(uint32_t now);

  Phase phase() const { return _phase; }
  bool  active() const { return _phase != Phase::INACTIF && _phase != Phase::TERMINE; }
  int   position() const { return _pos; }                                   // -1 si inactif
  int   pompeCourante() const { return _pos >= 0 ? _cfg.ordre[_pos] : -1; }
  uint32_t restant_ms(uint32_t now) const;
  const char* nomPhase() const;

private:
  int  premierePositionActive(int depuis) const;
  void entrerPosition(uint32_t now);
  void lancerPompe(uint32_t now);

  Pompes&       _pompes;
  const Config& _cfg;
  Phase    _phase = Phase::INACTIF;
  int      _pos   = -1;
  uint32_t _fin   = 0;
};
