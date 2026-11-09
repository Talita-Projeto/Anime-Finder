const SUPABASE_URL = "https://cmvyjchtzocaxqqceerp.supabase.co";
const SUPABASE_KEY = "sb_publishable_NLkFhCR9SwyB4YY-3NFNxQ_iLyKWDuj";
const BASE_CAPAS = SUPABASE_URL + "/storage/v1/object/public/capas/";


const CAPAS = {
  "Vivy: Fluorite Eye's Song": "vivy.webp",
  "I Want to Love You Till Your Dying Day": "i-want-to-love-you.webp",
  "Kumo desu ga, Nani ka?": "kumo-desu-ga.webp",
  "Tantei wa Mou, Shindeiru": "tantei-wa-mou-shindeiru.webp",
  "Lord of Mysteries": "lord-of-mysteries-poster.webp",
  "Migi to Dali": "magi-to-dali.jpg",
  "Yofukashi no Uta": "yofukashi-no-uta.webp",
  "Youjo Senki": "youjo-senki.webp",
  "Spice and Wolf": "spice-and-wolf.jpg",
  "Mahoutsukai no Yome": "mahoutsukai-no-yome.webp",
  "Kimi to Boku no Saigo no Senjou, Aruiwa Sekai ga Hajimaru Seisen": "kimi-to-boku-no-saigo.webp",
  "Chuunibyou demo Koi ga Shitai": "chuunibyou-demo-koi-ga-shitai.jpg",
  "White Album 2": "white-album-2.avif",
  "Fate/stay night": "fate-stay-night.jpg",
  "Clannad": "clannad.jpg",
  "Hibike! Euphonium": "hibike-euphonium.webp",
  "Gintama": "gintama.webp",
  "Angel Beats!": "angel-beats.webp",
  "Kara no Kyoukai": "kara-no-kyoukai.jpg",
  "Toradora": "toradora.webp"
};


const CATALOGO = [
  ["Vivy: Fluorite Eye's Song", "Ficção Científica", 14, 13],
  ["I Want to Love You Till Your Dying Day", "Drama", 16, 13],
  ["Kumo desu ga, Nani ka?", "Fantasia", 14, 24],
  ["Tantei wa Mou, Shindeiru", "Mistério", 14, 12],
  ["Lord of Mysteries", "Fantasia", 14, 12],
  ["Migi to Dali", "Mistério", 14, 13],
  ["Yofukashi no Uta", "Romance", 16, 26],
  ["Youjo Senki", "Ação", 16, 24],
  ["Spice and Wolf", "Fantasia", 12, 25],
  ["Mahoutsukai no Yome", "Fantasia", 14, 48],
  ["Kimi to Boku no Saigo no Senjou, Aruiwa Sekai ga Hajimaru Seisen", "Ação", 14, 24],
  ["Chuunibyou demo Koi ga Shitai", "Romance", 12, 25],
  ["White Album 2", "Romance", 14, 13],
  ["Fate/stay night", "Ação", 16, 24],
  ["Clannad", "Drama", 12, 48],
  ["Hibike! Euphonium", "Drama", 12, 41],
  ["Gintama", "Comédia", 14, 397],
  ["Angel Beats!", "Drama", 14, 14],
  ["Kara no Kyoukai", "Mistério", 16, 10],
  ["Toradora", "Romance", 12, 25]
].map(([titulo, genero, classificacao, episodios]) => ({
  titulo, genero, classificacao, episodios,
  capa: CAPAS[titulo] ? BASE_CAPAS + CAPAS[titulo] : ""
}));


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
  return linhas.map(a => ({
    ...a,
    capa: a.capa || (CAPAS[a.titulo] ? BASE_CAPAS + CAPAS[a.titulo] : "")
  }));
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


const pagina = a => "anime.html?t=" + encodeURIComponent(a.titulo);

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


document.addEventListener("error", e => {
  if (e.target.tagName === "IMG") { console.warn("Capa falhou:", e.target.src); e.target.remove(); }
}, true);

rota();
init();
