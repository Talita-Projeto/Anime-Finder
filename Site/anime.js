const SUPABASE_URL = "https://cmvyjchtzocaxqqceerp.supabase.co";
const SUPABASE_KEY = "sb_publishable_NLkFhCR9SwyB4YY-3NFNxQ_iLyKWDuj";

const $ = s => document.querySelector(s);
const esc = s => String(s).replace(/[&<>"']/g, m => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[m]));
const pl = (n, s, p) => n + " " + (n === 1 ? s : p);
const ROTULOS = { Acao: "Ação", Comedia: "Comédia" };
const rot = g => ROTULOS[g] || g;

function mostrarErro(msg) {
  $("#detalhe").hidden = true;
  $("#status").textContent = msg;
}

async function buscarAnime(titulo) {
  const headers = { apikey: SUPABASE_KEY };
  if (SUPABASE_KEY.startsWith("eyJ")) headers.Authorization = "Bearer " + SUPABASE_KEY;
  const url = SUPABASE_URL.replace(/\/+$/, "") +
    "/rest/v1/v_animes?select=titulo,genero,classificacao,episodios,sinopse,capa" +
    "&titulo=eq." + encodeURIComponent(titulo) + "&limit=1";
  const resp = await fetch(url, { headers });
  if (!resp.ok) throw new Error("HTTP " + resp.status);
  const linhas = await resp.json();
  return linhas[0] || null;
}

function desenhar(a) {
  const classif = a.classificacao === 0 ? "Livre" : a.classificacao + "+";
  const classifLonga = a.classificacao === 0 ? "Livre" : a.classificacao + " anos";
  const sinopse = a.sinopse && a.sinopse.trim()
    ? esc(a.sinopse)
    : "Sinopse ainda não cadastrada.";

  document.title = a.titulo + " · AnimeFinder";
  const desc = document.querySelector('meta[name="description"]');
  if (desc) {
    desc.content = a.titulo + ": " + rot(a.genero) + ", " + pl(a.episodios, "episódio", "episódios") +
      ", classificação " + classif + ". Veja a sinopse e os detalhes no AnimeFinder.";
  }

  $("#detalhe").innerHTML = `
    <div class="capa">
      <span>${esc(a.titulo.charAt(0))}</span><small>Sem capa</small>
      ${a.capa ? `<img src="${esc(a.capa)}" alt="Capa de ${esc(a.titulo)}">` : ""}
    </div>
    <div class="detalhe-info">
      <h2>${esc(a.titulo)}</h2>
      <div class="meta">
        <span class="tag">${esc(rot(a.genero))}</span>
        <span class="age" data-r="${a.classificacao}" title="Classificação indicativa">${classif}</span>
      </div>
      <p class="sinopse">${sinopse}</p>
      <dl class="ficha">
        <div><dt>Gênero</dt><dd>${esc(rot(a.genero))}</dd></div>
        <div><dt>Classificação indicativa</dt><dd>${classifLonga}</dd></div>
        <div><dt>Episódios</dt><dd>${pl(a.episodios, "episódio", "episódios")}</dd></div>
      </dl>
    </div>`;
  $("#detalhe").hidden = false;
  $("#status").textContent = "";
}

async function init() {
  const titulo = new URLSearchParams(location.search).get("t");
  if (!titulo) { mostrarErro("Nenhum anime foi informado. Volte ao catálogo e escolha um."); return; }
  try {
    const anime = await buscarAnime(titulo);
    if (!anime) { mostrarErro("Anime não encontrado no catálogo."); return; }
    desenhar(anime);
  } catch (e) {
    console.error(e);
    mostrarErro("Não foi possível carregar o anime agora (" + e.message + "). Tente novamente.");
  }
}

// Capa quebrada: remove a imagem e deixa a letra inicial visível
document.addEventListener("error", e => {
  if (e.target.tagName === "IMG") { console.warn("Capa falhou:", e.target.src); e.target.remove(); }
}, true);

init();
