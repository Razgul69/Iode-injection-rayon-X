#pragma once
#include <pgmspace.h>

// Interface web embarquée (HTML + JS vanilla, aucune ressource externe).
static const char PAGE_WEB[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="fr"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Iode injection</title>
<style>
:root{--bg:#f4f6f8;--card:#fff;--txt:#222;--muted:#666;--ok:#2e7d32;--warn:#e65100;--stop:#c62828;--p1:#d32f2f;--p2:#388e3c;--p3:#1976d2}
*{box-sizing:border-box}body{margin:0;font-family:system-ui,sans-serif;background:var(--bg);color:var(--txt)}
header{background:#263238;color:#fff;padding:12px 16px;display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:8px}
header h1{font-size:1.1rem;margin:0}#lien{font-size:.85rem;color:#b0bec5}
main{max-width:720px;margin:0 auto;padding:12px}
.card{background:var(--card);border-radius:10px;padding:14px;margin-bottom:12px;box-shadow:0 1px 3px rgba(0,0,0,.08)}
.card h2{font-size:1rem;margin:0 0 10px}
.etat{display:grid;grid-template-columns:repeat(auto-fit,minmax(140px,1fr));gap:8px}
.etat div{background:#eceff1;border-radius:8px;padding:8px}.etat b{display:block;font-size:1.3rem}
.etat small{color:var(--muted)}
.pompe{display:grid;grid-template-columns:auto 1fr auto auto auto;gap:8px;align-items:center;padding:8px 0;border-top:1px solid #eee}
.pompe:first-of-type{border-top:0}
.badge{width:14px;height:14px;border-radius:50%;background:#ccc;display:inline-block}
.on .badge{box-shadow:0 0 0 4px rgba(0,0,0,.08)}
.p1.on .badge{background:var(--p1)}.p2.on .badge{background:var(--p2)}.p3.on .badge{background:var(--p3)}
input[type=number]{width:72px;padding:6px;border:1px solid #ccc;border-radius:6px;font-size:1rem}
select{padding:6px;border:1px solid #ccc;border-radius:6px;font-size:1rem}
button{padding:8px 12px;border:0;border-radius:8px;font-size:.95rem;cursor:pointer;color:#fff;background:#546e7a}
button.ok{background:var(--ok)}button.stop{background:var(--stop)}button.big{width:100%;padding:14px;font-size:1.1rem;margin-top:8px}
button:disabled{opacity:.45;cursor:default}
table{width:100%;border-collapse:collapse}td,th{padding:6px 4px;text-align:left;font-size:.95rem}th{color:var(--muted);font-weight:500}
.row{display:flex;gap:10px;flex-wrap:wrap;align-items:center;margin:8px 0}
.row label{display:flex;align-items:center;gap:6px}
.phase{font-weight:600}.phase.pompage{color:var(--ok)}.phase.attente,.phase.delai_initial{color:var(--warn)}
#msg{color:var(--stop);min-height:1.2em;font-size:.9rem}
</style></head><body>
<header><h1>Iode injection — 3 pompes</h1><span id="lien">connexion…</span></header>
<main>

<div class="card"><h2>État</h2>
<div class="etat">
 <div><small>Batterie</small><b id="vbat">–</b></div>
 <div><small>Séquence</small><b class="phase" id="phase">–</b></div>
 <div><small>Reste</small><b id="reste">–</b></div>
 <div><small>Pompe en cours</small><b id="pcour">–</b></div>
</div>
<button class="stop big" onclick="post('/api/stop')">ARRÊT GÉNÉRAL</button>
</div>

<div class="card"><h2>Commande manuelle</h2>
<div id="pompes"></div>
<small style="color:var(--muted)">Durée en secondes ; 0 = continu (arrêt automatique après 10 min).</small>
</div>

<div class="card"><h2>Séquence automatique</h2>
<div class="row"><label>Délai initial avant la 1<sup>re</sup> pompe <input type="number" id="delai" min="0" step="1"> s</label><span id="delaiMin" style="color:var(--muted)"></span></div>
<table><thead><tr><th>#</th><th>Pompe</th><th>Active</th><th>Attente avant (s)</th><th>Durée (s)</th></tr></thead>
<tbody id="seq"></tbody></table>
<div class="row"><label>Seuil batterie <input type="number" id="vbatMin" min="0" step="0.1"> V (0 = désactivé)</label></div>
<div class="row">
 <button onclick="sauver()">Enregistrer les réglages</button>
 <button class="ok" id="bStart" onclick="post('/api/sequence?action=start')">Démarrer la séquence</button>
 <button class="stop" id="bStop" onclick="post('/api/sequence?action=stop')">Arrêter la séquence</button>
</div>
<div id="msg"></div>
</div>

<div class="card"><h2>Sens de rotation</h2>
<div class="row" id="sens"></div>
<small style="color:var(--muted)">Enregistrer pour appliquer.</small>
</div>

</main>
<script>
const N=3,C=['p1','p2','p3'];let cfgCharge=false,derniereCfg=null;
const $=id=>document.getElementById(id);
function mmss(ms){const s=Math.ceil(ms/1000);return s>=60?`${Math.floor(s/60)} min ${String(s%60).padStart(2,'0')} s`:`${s} s`}

(function init(){
 let h='';for(let i=0;i<N;i++)h+=`<div class="pompe ${C[i]}" id="pm${i}"><span class="badge"></span><span>Pompe ${i+1} <small id="pr${i}" style="color:var(--muted)"></small></span>
 <input type="number" id="pd${i}" min="0" step="1" value="3" title="Durée (s)">
 <button class="ok" onclick="marche(${i})">Marche</button><button class="stop" onclick="post('/api/pompe?n=${i+1}&action=arret')">Arrêt</button></div>`;
 $('pompes').innerHTML=h;
 h='';for(let p=0;p<N;p++){let opt='';for(let i=0;i<N;i++)opt+=`<option value="${i}">Pompe ${i+1}</option>`;
 h+=`<tr><td>${p+1}</td><td><select id="o${p}">${opt}</select></td><td><input type="checkbox" id="a${p}"></td>
 <td><input type="number" id="t${p}" min="0" step="1" ${p==0?'disabled title="Pas d\'attente pour la première pompe"':''}></td>
 <td><input type="number" id="dd${p}" min="1" step="1" disabled title="Réglée dans Commande manuelle"></td></tr>`}
 $('seq').innerHTML=h;
 h='';for(let i=0;i<N;i++)h+=`<label><input type="checkbox" id="i${i}"> Pompe ${i+1} inversée</label>`;
 $('sens').innerHTML=h;
 $('delai').oninput=()=>$('delaiMin').textContent=`(${mmss($('delai').value*1000)})`;
 for(let p=0;p<N;p++)$('o'+p).onchange=majDurees;
 for(let i=0;i<N;i++)$('pd'+i).oninput=majDurees;
})();

function majDurees(){for(let p=0;p<N;p++){const i=+$('o'+p).value;$('dd'+p).value=$('pd'+i).value}}

function remplir(c){
 $('delai').value=c.delai;$('delai').oninput();$('vbatMin').value=c.vbatMin;
 for(let p=0;p<N;p++){$('o'+p).value=c.ordre[p];$('a'+p).checked=!!c.actif[p];$('t'+p).value=c.attente[p]}
 for(let i=0;i<N;i++){$('pd'+i).value=c.duree[i];$('i'+i).checked=!!c.inverse[i]}
 majDurees();
}

function afficher(e){
 $('lien').textContent='connecté';
 $('vbat').textContent=e.vbat.toFixed(2)+' V';
 const ph=$('phase');ph.className='phase '+e.seq.phase;
 ph.textContent={inactif:'Inactive',delai_initial:'Délai initial',attente:'Attente',pompage:'Pompage',termine:'Terminée'}[e.seq.phase]||e.seq.phase;
 $('reste').textContent=e.seq.active?mmss(e.seq.restant_ms):'–';
 $('pcour').textContent=e.seq.pompe>=0?'Pompe '+(e.seq.pompe+1):'–';
 for(let i=0;i<N;i++){const d=$('pm'+i);d.classList.toggle('on',e.pompes[i].marche);
  $('pr'+i).textContent=e.pompes[i].marche?'• '+mmss(e.pompes[i].restant_ms):''}
 $('bStart').disabled=e.seq.active;$('bStop').disabled=!e.seq.active;
 if(!cfgCharge){remplir(e.cfg);cfgCharge=true}
}

async function maj(){try{const r=await fetch('/api/etat',{cache:'no-store'});afficher(await r.json())}catch(x){$('lien').textContent='hors ligne'}}
async function post(url){$('msg').textContent='';try{const r=await fetch(url,{method:'POST'});
 if(!r.ok){$('msg').textContent=await r.text();return}afficher(await r.json())}catch(x){$('msg').textContent='Erreur de communication'}}
function marche(i){post(`/api/pompe?n=${i+1}&action=marche&duree=${$('pd'+i).value||0}`)}
function sauver(){
 const o=[];for(let p=0;p<N;p++)o.push(+$('o'+p).value);
 if(new Set(o).size!==N){$('msg').textContent='Chaque pompe doit apparaître une seule fois dans la séquence';return}
 let q=`delai=${$('delai').value}&vbatMin=${$('vbatMin').value}`;
 for(let p=0;p<N;p++)q+=`&o${p+1}=${o[p]+1}&a${p+1}=${$('a'+p).checked?1:0}&t${p+1}=${$('t'+p).value||0}`;
 for(let i=0;i<N;i++)q+=`&d${i+1}=${$('pd'+i).value||1}&i${i+1}=${$('i'+i).checked?1:0}`;
 cfgCharge=false;post('/api/config?'+q).then(()=>{$('msg').style.color='var(--ok)';$('msg').textContent='Réglages enregistrés';setTimeout(()=>{$('msg').textContent='';$('msg').style.color=''},2000)});
}
maj();setInterval(maj,1000);
</script></body></html>)HTML";
