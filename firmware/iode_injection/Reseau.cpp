#include "Reseau.h"
#include "Batterie.h"
#include "page_web.h"
#include <ESPmDNS.h>

void Reseau::begin() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, WIFI_MDP);
  Serial.printf("WiFi AP '%s' -> http://%s/\n", WIFI_SSID, WiFi.softAPIP().toString().c_str());
  if (MDNS.begin(MDNS_NOM)) Serial.printf("mDNS   -> http://%s.local/\n", MDNS_NOM);

  _server.on("/",             HTTP_GET,  [this] { page(); });
  _server.on("/api/etat",     HTTP_GET,  [this] { etat(); });
  _server.on("/api/pompe",    HTTP_POST, [this] { pompe(); });
  _server.on("/api/sequence", HTTP_POST, [this] { sequence(); });
  _server.on("/api/config",   HTTP_POST, [this] { config(); });
  _server.on("/api/stop",     HTTP_POST, [this] { arretGeneral(); });
  _server.onNotFound([this] { erreur(404, "Introuvable"); });
  _server.begin();
}

bool Reseau::batterieOk() const {
  return _cfg.vbatMin <= 0 || lireVbat() >= _cfg.vbatMin;
}

void Reseau::page() {
  _server.send_P(200, "text/html; charset=utf-8", PAGE_WEB);
}

void Reseau::etat() {
  uint32_t now = millis();
  char buf[900];
  int n = 0;
  n += snprintf(buf + n, sizeof buf - n,
    "{\"vbat\":%.2f,\"uptime_s\":%lu,"
    "\"seq\":{\"phase\":\"%s\",\"active\":%s,\"position\":%d,\"pompe\":%d,\"restant_ms\":%lu},"
    "\"pompes\":[",
    lireVbat(), (unsigned long)(now / 1000),
    _seq.nomPhase(), _seq.active() ? "true" : "false", _seq.position(), _seq.pompeCourante(),
    (unsigned long)_seq.restant_ms(now));
  for (int i = 0; i < NB_POMPES; i++)
    n += snprintf(buf + n, sizeof buf - n, "%s{\"marche\":%s,\"restant_ms\":%lu}",
      i ? "," : "", _pompes.enMarche(i) ? "true" : "false", (unsigned long)_pompes.restant_ms(i, now));
  n += snprintf(buf + n, sizeof buf - n,
    "],\"cfg\":{\"delai\":%lu,\"vbatMin\":%.1f,"
    "\"ordre\":[%d,%d,%d],\"actif\":[%d,%d,%d],\"attente\":[%lu,%lu,%lu],"
    "\"duree\":[%lu,%lu,%lu],\"inverse\":[%d,%d,%d]}}",
    (unsigned long)_cfg.delaiInitial_s, _cfg.vbatMin,
    _cfg.ordre[0], _cfg.ordre[1], _cfg.ordre[2],
    _cfg.actif[0], _cfg.actif[1], _cfg.actif[2],
    (unsigned long)_cfg.attente_s[0], (unsigned long)_cfg.attente_s[1], (unsigned long)_cfg.attente_s[2],
    (unsigned long)_cfg.duree_s[0], (unsigned long)_cfg.duree_s[1], (unsigned long)_cfg.duree_s[2],
    _cfg.inverse[0], _cfg.inverse[1], _cfg.inverse[2]);
  _server.send(200, "application/json", buf);
}

void Reseau::pompe() {
  int n = _server.arg("n").toInt() - 1;
  if (n < 0 || n >= NB_POMPES) { erreur(400, "Numéro de pompe invalide"); return; }
  String action = _server.arg("action");

  if (action == "marche") {
    if (_seq.active())  { erreur(409, "Séquence en cours : arrêtez-la d'abord"); return; }
    if (!batterieOk())  { erreur(409, "Batterie faible : démarrage refusé"); return; }
    uint32_t d = _server.hasArg("duree") ? _server.arg("duree").toInt() : _cfg.duree_s[n];
    _pompes.marche(n, d * 1000UL, _cfg.inverse[n]);
  } else if (action == "arret") {
    _pompes.arret(n);
  } else {
    erreur(400, "Action inconnue"); return;
  }
  etat();
}

void Reseau::sequence() {
  String action = _server.arg("action");
  uint32_t now = millis();
  if (action == "start") {
    if (_seq.active()) { erreur(409, "Séquence déjà en cours"); return; }
    if (!batterieOk()) { erreur(409, "Batterie faible : démarrage refusé"); return; }
    if (!_seq.demarrer(now)) { erreur(409, "Aucune pompe active dans la séquence"); return; }
  } else if (action == "stop") {
    _seq.arreter(now);
  } else {
    erreur(400, "Action inconnue"); return;
  }
  etat();
}

void Reseau::config() {
  if (_seq.active()) { erreur(409, "Séquence en cours : modification refusée"); return; }
  Config c = _cfg;
  if (_server.hasArg("delai"))   c.delaiInitial_s = _server.arg("delai").toInt();
  if (_server.hasArg("vbatMin")) c.vbatMin = _server.arg("vbatMin").toFloat();
  char k[4];
  for (int i = 0; i < NB_POMPES; i++) {
    snprintf(k, sizeof k, "o%d", i + 1); if (_server.hasArg(k)) c.ordre[i]     = _server.arg(k).toInt() - 1;
    snprintf(k, sizeof k, "a%d", i + 1); if (_server.hasArg(k)) c.actif[i]     = _server.arg(k) == "1";
    snprintf(k, sizeof k, "t%d", i + 1); if (_server.hasArg(k)) c.attente_s[i] = _server.arg(k).toInt();
    snprintf(k, sizeof k, "d%d", i + 1); if (_server.hasArg(k)) c.duree_s[i]   = _server.arg(k).toInt();
    snprintf(k, sizeof k, "i%d", i + 1); if (_server.hasArg(k)) c.inverse[i]   = _server.arg(k) == "1";
  }
  Stockage::valider(c);
  _cfg = c;
  _stock.sauver(_cfg);
  Serial.println("Config enregistree");
  etat();
}

void Reseau::arretGeneral() {
  _seq.arreter(millis());
  _pompes.arretTous();
  etat();
}
