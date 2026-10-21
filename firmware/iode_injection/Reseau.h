#pragma once
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "Pompes.h"
#include "Sequenceur.h"
#include "Stockage.h"

// Point d'accès WiFi + serveur HTTP : page web embarquée et API JSON.
//   GET  /               page de commande
//   GET  /api/etat       état complet (JSON)
//   POST /api/pompe      n=1..3  action=marche|arret  [duree=s, 0 = continu]
//   POST /api/sequence   action=start|stop
//   POST /api/config     delai, vbatMin, o1..o3, a1..a3, t1..t3, d1..d3, i1..i3
//   POST /api/stop       arrêt général
class Reseau {
public:
  Reseau(Pompes& p, Sequenceur& s, Config& c, Stockage& st)
    : _pompes(p), _seq(s), _cfg(c), _stock(st) {}
  void begin();
  void update() { _server.handleClient(); }

private:
  void page();
  void etat();
  void pompe();
  void sequence();
  void config();
  void arretGeneral();
  bool batterieOk() const;
  void erreur(int code, const char* msg) { _server.send(code, "text/plain; charset=utf-8", msg); }

  WebServer   _server{80};
  Pompes&     _pompes;
  Sequenceur& _seq;
  Config&     _cfg;
  Stockage&   _stock;
};
