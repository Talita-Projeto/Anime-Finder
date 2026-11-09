const SUPABASE_URL = "https://cmvyjchtzocaxqqceerp.supabase.co";
const SUPABASE_KEY = "sb_publishable_NLkFhCR9SwyB4YY-3NFNxQ_iLyKWDuj";


const CAPAS = {
  "Naruto": "Naruto-Classico-Capa.webp",
  "One Piece": "",
  "Death Note": "",
  "Spy x Family": "",
  "Your Name": "",
  "Attack on Titan": "",
  "Demon Slayer": "",
  "Haikyuu": "",
  "Frieren": "",
  "K-On": "",
  "Fullmetal Alchemist: Brotherhood": "",
  "Violet Evergarden": "",
  "Mob Psycho 100": "",
  "Dr. Stone": "",
  "A Silent Voice": ""
};


const CATALOGO = [
  ["Naruto", "Aventura", 12, 220],
  ["One Piece", "Aventura", 12, 1100],
  ["Death Note", "Suspense", 16, 37],
  ["Spy x Family", "Comedia", 12, 37],
  ["Your Name", "Romance", 10, 1],
  ["Attack on Titan", "Acao", 16, 87],
  ["Demon Slayer", "Acao", 16, 63],
  ["Haikyuu", "Esporte", 10, 85],
  ["Frieren", "Fantasia", 12, 28],
  ["K-On", "Comedia", 10, 39],
  ["Fullmetal Alchemist: Brotherhood", "Aventura", 14, 64],
  ["Violet Evergarden", "Drama", 12, 13],
  ["Mob Psycho 100", "Acao", 12, 37],
  ["Dr. Stone", "Aventura", 12, 57],
  ["A Silent Voice", "Drama", 12, 1]
].map(([titulo, genero, classificacao, episodios]) => ({ titulo, genero, classificacao, episodios, capa: CAPAS[titulo] || "" }));


async function carregarCatalogo() {
  if (!SUPABASE_URL || !SUPABASE_KEY) return CATALOGO;
  const headers = { apikey: SUPABASE_KEY };
  if (SUPABASE_KEY.startsWith("eyJ")) headers.Authorization = "Bearer " + SUPABASE_KEY;
  const url = SUPABASE_URL.replace(/\/+$/, "") +
    "/rest/v1/v_animes?select=titulo,genero,classificacao,episodios,sinopse,capa&order=titulo";
  const resp = await fetch(url, { headers });
  if (!resp.ok) {

    let det = "";
    try { const j = await resp.json(); det = [j.code, j.message].filter(Boolean).join(" - "); } catch (_) { }
    throw new Error("HTTP " + resp.status + (det ? " - " + det : ""));
  }
  const linhas = await resp.json();
  return linhas.map(a => ({ ...a, capa: a.capa || CAPAS[a.titulo] || "" }));
}

const $ = s => document.querySelector(s);
const esc = s => String(s).replace(/[&<>"']/g, m => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[m]));
const norm = s => s.normalize("NFD").replace(/[\u0300-\u036f]/g, "").toLowerCase().trim();
const pl = (n, s, p) => n + " " + (n === 1 ? s : p);
const ROTULOS = { Acao: "Ação", Comedia: "Comédia" };
const rot = g => ROTULOS[g] || g;
let dados = [];

const capa = a => `<div class="capa"><span>${esc(a.titulo.charAt(0))}</span><small>Sem capa</small>${a.capa
  ? `<img src="${esc(a.capa)}" alt="Capa de ${esc(a.titulo)}" loading="lazy">` : ""}</div>`;

const PAGINAS = ["Naruto"];
const slug = t => norm(t).replace(/[^a-z0-9]+/g, "-").replace(/^-+|-+$/g, "");
const pagina = a => PAGINAS.includes(a.titulo)
  ? "anime-" + slug(a.titulo) + ".html"
  : "anime.html?t=" + encodeURIComponent(a.titulo);

const card = a => `<a class="card" href="${esc(pagina(a))}">${capa(a)}<h3>${esc(a.titulo)}</h3>
  <div class="meta"><span class="tag">${esc(rot(a.genero))}</span>
  <span class="age" data-r="${a.classificacao}" title="Classificação indicativa">${a.classificacao}+</span></div>
  <p>${pl(a.episodios, "episódio", "episódios")}</p></a>`;

const generos = () => {
  const m = new Map();
  dados.forEach(a => m.set(a.genero, (m.get(a.genero) || 0) + 1));
  return [...m];
};

const MSG_BUSCA = "Digite um título ou escolha um gênero ou uma classificação para buscar.";
const rotCl = c => c === 0 ? "Livre" : c + "+";
const classificacoes = () => [...new Set(dados.map(a => a.classificacao))].sort((x, y) => x - y);
const marcado = nome => { const el = document.querySelector(`input[name=${nome}]:checked`); return el ? el.value : ""; };
function buscar() {
  const t = norm($("#termo").value), info = $("#bus-info"), grid = $("#bus-grid");
  const g = marcado("gen"), c = marcado("cl");
  if (!t && !g && c === "") { info.textContent = MSG_BUSCA; grid.innerHTML = ""; return; }
  const r = dados.filter(a => (!g || a.genero === g) && (c === "" || String(a.classificacao) === c) &&
    (!t || norm(a.titulo).includes(t)));
  const filtros = [g && rot(g), c !== "" && rotCl(Number(c))].filter(Boolean).join(", ");
  info.textContent = r.length
    ? `Foram encontrados ${pl(r.length, "resultado", "resultados")}${filtros ? " (" + filtros + ")" : ""}.`
    : "Nenhum anime corresponde à busca.";
  grid.innerHTML = r.map(card).join("");
}

const VIEWS = ["catalogo", "buscar", "generos"];
function rota() {
  const h = location.hash.slice(1), v = VIEWS.includes(h) ? h : "catalogo";
  VIEWS.forEach(n => $("#" + n).classList.toggle("on", n === v));
  document.querySelectorAll("nav a").forEach(a => a.setAttribute("aria-current", a.hash === "#" + v ? "page" : "false"));
}

async function init() {
  let aviso = "";
  try { dados = await carregarCatalogo(); }
  catch (e) {

    console.error(e);
    dados = CATALOGO;
    aviso = "Banco de dados indisponível (" + e.message + "). Exibindo o catálogo local. ";
  }
  $("#cat-info").textContent = aviso + "Total de animes: " + dados.length;
  $("#cat-grid").innerHTML = dados.map(card).join("");
  $("#chips").innerHTML = `<label class="chip"><input type="radio" name="gen" value="" checked><span>Todos</span></label>` +
    generos().map(([g]) =>
      `<label class="chip"><input type="radio" name="gen" value="${esc(g)}"><span>${esc(rot(g))}</span></label>`).join("");
  $("#chips").addEventListener("change", buscar);
  $("#chips-cl").innerHTML = classificacoes().map(c =>
    `<label class="chip"><input type="radio" name="cl" value="${c}"><span>${rotCl(c)}</span></label>`).join("");

  $("#chips-cl").addEventListener("click", e => {
    const i = e.target.closest("input"); if (!i) return;
    const era = i.dataset.on === "1";
    document.querySelectorAll("#chips-cl input").forEach(x => { x.dataset.on = ""; });
    if (era) i.checked = false; else i.dataset.on = "1";
    buscar();
  });
  $("#gen-grid").innerHTML = generos().map(([g, n]) =>
    `<button class="gen" type="button" data-g="${esc(g)}"><strong>${esc(rot(g))}</strong><span>${pl(n, "anime", "animes")}</span></button>`).join("");
  $("#gen-grid").addEventListener("click", e => {
    const b = e.target.closest(".gen"); if (!b) return;
    document.querySelector(`input[name=gen][value="${b.dataset.g}"]`).checked = true;
    location.hash = "#buscar";
    buscar();
    $("#termo").focus();
  });
}

$("#termo").addEventListener("input", buscar);
addEventListener("hashchange", rota);

document.addEventListener("error", e => { if (e.target.tagName === "IMG") e.target.remove(); }, true);
rota();
init();
