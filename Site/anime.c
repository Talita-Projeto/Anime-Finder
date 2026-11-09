#include "anime.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static const char MAPA_LATIN1[] =
    "aaaaaaaceeeeiiiidnooooo*ouuuuyts"
    "aaaaaaaceeeeiiiidnooooo/ouuuuyty";

void normalizar(const char *src, char *out, size_t tam) {
    size_t o = 0;
    if (tam == 0) return;
    for (size_t i = 0; src[i] != '\0' && o + 1 < tam; i++) {
        unsigned char c = (unsigned char)src[i];
        unsigned char d = (unsigned char)src[i + 1];
        if ((c == 0xC2 || c == 0xC3) && (d & 0xC0) == 0x80) {

            unsigned cp = ((c & 0x03u) << 6) | (d & 0x3Fu);
            out[o++] = (cp >= 0xC0) ? MAPA_LATIN1[cp - 0xC0] : '?';
            i++;
        } else if (c < 0x80) {
            out[o++] = (char)tolower(c);
        } else {
            out[o++] = (char)c;
        }
    }
    out[o] = '\0';
}

#define TAM_NORM 512

int textos_iguais(const char *a, const char *b) {
    char x[TAM_NORM], y[TAM_NORM];
    normalizar(a, x, sizeof x);
    normalizar(b, y, sizeof y);
    return strcmp(x, y) == 0;
}

int contem_texto(const char *texto, const char *termo) {
    char x[TAM_NORM], y[TAM_NORM];
    normalizar(texto, x, sizeof x);
    normalizar(termo, y, sizeof y);
    if (y[0] == '\0') return 1;
    return strstr(x, y) != NULL;
}

static char *copiar(const char *s) {
    if (!s) s = "";
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

int catalogo_add(Catalogo *c, const char *titulo, const char *genero,
                 int classificacao, int episodios,
                 const char *sinopse, const char *capa) {
    Anime *novo = realloc(c->v, (c->n + 1) * sizeof *novo);
    if (!novo) return -1;
    c->v = novo;
    Anime *a = &c->v[c->n];
    a->titulo = copiar(titulo);
    a->genero = copiar(genero);
    a->sinopse = copiar(sinopse);
    a->capa = copiar(capa);
    a->classificacao = classificacao;
    a->episodios = episodios;
    if (!a->titulo || !a->genero || !a->sinopse || !a->capa) {
        free(a->titulo); free(a->genero); free(a->sinopse); free(a->capa);
        return -1;
    }
    c->n++;
    return 0;
}

void catalogo_liberar(Catalogo *c) {
    for (size_t i = 0; i < c->n; i++) {
        free(c->v[i].titulo); free(c->v[i].genero);
        free(c->v[i].sinopse); free(c->v[i].capa);
    }
    free(c->v);
    c->v = NULL;
    c->n = 0;
}

#define BASE_CAPAS "https://cmvyjchtzocaxqqceerp.supabase.co/storage/v1/object/public/capas/"

static const struct { const char *t, *g, *capa; int cl, ep; } EMBUTIDO[] = {
    {"Naruto", "Aventura", "Naruto-Classico-Capa.webp", 12, 220},
    {"One Piece", "Aventura", "one-piece.jpg", 12, 1100},
    {"Death Note", "Suspense", "death-note.jpg", 16, 37},
    {"Spy x Family", "Comédia", "spy-x-family.jpg", 12, 37},
    {"Your Name", "Romance", "your-name.jpg", 10, 1},
    {"Attack on Titan", "Ação", "attack-on-titan.jpg", 16, 87},
    {"Demon Slayer", "Ação", "demon-slayer.jpg", 16, 63},
    {"Haikyuu", "Esporte", "haikyuu.jpg", 10, 85},
    {"Frieren", "Fantasia", "frieren.jpg", 12, 28},
    {"K-On", "Comédia", "k-on.jpg", 10, 39},
    {"Fullmetal Alchemist: Brotherhood", "Aventura", "fullmetal-alchemist-brotherhood.jpg", 14, 64},
    {"Violet Evergarden", "Drama", "violet-evergarden.jpg", 12, 13},
    {"Mob Psycho 100", "Ação", "mob-psycho-100.jpg", 12, 37},
    {"Dr. Stone", "Aventura", "dr-stone.jpg", 12, 57},
    {"A Silent Voice", "Drama", "a-silent-voice.jpg", 12, 1},
};

int catalogo_embutido(Catalogo *c) {
    char capa[256];
    for (size_t i = 0; i < sizeof EMBUTIDO / sizeof EMBUTIDO[0]; i++) {
        snprintf(capa, sizeof capa, "%s%s", BASE_CAPAS, EMBUTIDO[i].capa);
        if (catalogo_add(c, EMBUTIDO[i].t, EMBUTIDO[i].g, EMBUTIDO[i].cl,
                         EMBUTIDO[i].ep, "", capa) != 0) {
            catalogo_liberar(c);
            return -1;
        }
    }
    return 0;
}
