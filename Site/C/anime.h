#ifndef ANIME_H
#define ANIME_H

#include <stddef.h>

typedef struct {
    char *titulo;
    char *genero;
    char *sinopse;
    char *capa;
    int classificacao;
    int episodios;
} Anime;

typedef struct {
    Anime *v;
    size_t n;
} Catalogo;


int  catalogo_embutido(Catalogo *c);

int  catalogo_add(Catalogo *c, const char *titulo, const char *genero,
                  int classificacao, int episodios,
                  const char *sinopse, const char *capa);
void catalogo_liberar(Catalogo *c);


void normalizar(const char *src, char *out, size_t tam);

int  textos_iguais(const char *a, const char *b);

int  contem_texto(const char *texto, const char *termo);

#endif
