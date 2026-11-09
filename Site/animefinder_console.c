/*
 * AnimeFinder - Catalogo e recomendacao de animes
 * Projeto academico em C
 *
 * Funcionalidades:
 * - Listar o catalogo
 * - Buscar anime por titulo (busca parcial, sem diferenciar maiusculas),
 *   genero e classificacao indicativa (filtros combinaveis)
 * - Listar generos disponiveis
 *
 * Compilacao (GCC):
 *   gcc -std=c11 -Wall -Wextra -pedantic main.c -o animefinder
 *
 * Windows:
 *   animefinder.exe
 *
 * Linux/macOS:
 *   ./animefinder
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_TEXTO 100
#define TOTAL_ANIMES 15

typedef struct {
    char titulo[MAX_TEXTO];
    char genero[30];
    int classificacao;
    int episodios;
} Anime;

/* Catalogo inicial: dados ilustrativos para o projeto. */
Anime catalogo[TOTAL_ANIMES] = {
    {"Naruto", "Aventura", 12, 220},
    {"One Piece", "Aventura", 12, 1100},
    {"Death Note", "Suspense", 16, 37},
    {"Spy x Family", "Comedia", 12, 37},
    {"Your Name", "Romance", 10, 1},
    {"Attack on Titan", "Acao", 16, 87},
    {"Demon Slayer", "Acao", 16, 63},
    {"Haikyuu", "Esporte", 10, 85},
    {"Frieren", "Fantasia", 12, 28},
    {"K-On", "Comedia", 10, 39},
    {"Fullmetal Alchemist: Brotherhood", "Aventura", 14, 64},
    {"Violet Evergarden", "Drama", 12, 13},
    {"Mob Psycho 100", "Acao", 12, 37},
    {"Dr. Stone", "Aventura", 12, 57},
    {"A Silent Voice", "Drama", 12, 1}
};

/* Le uma linha inteira e remove o caractere de quebra de linha. */
void lerTexto(const char *mensagem, char texto[], size_t tamanho) {
    printf("%s", mensagem);

    if (fgets(texto, (int)tamanho, stdin) == NULL) {
        texto[0] = '\0';
        return;
    }

    texto[strcspn(texto, "\n")] = '\0';

    /* Descarta o restante caso a entrada seja maior que o vetor. */
    if (strchr(texto, '\0') != NULL && strlen(texto) == tamanho - 1) {
        int caractere;
        while ((caractere = getchar()) != '\n' && caractere != EOF) {
            /* descarta */
        }
    }
}

/* Le um numero inteiro e valida a entrada. */
int lerInteiro(const char *mensagem, int minimo, int maximo) {
    char entrada[40];
    char *fim;
    long valor;

    while (1) {
        lerTexto(mensagem, entrada, sizeof(entrada));

        valor = strtol(entrada, &fim, 10);

        while (isspace((unsigned char)*fim)) {
            fim++;
        }

        if (entrada[0] != '\0' && *fim == '\0' &&
            valor >= minimo && valor <= maximo) {
            return (int)valor;
        }

        printf("Entrada invalida. Digite um numero entre %d e %d.\n",
               minimo, maximo);
    }
}

/* Compara textos ignorando diferencas entre maiusculas e minusculas. */
int compararSemDiferenciarMaiusculas(const char *a, const char *b) {
    while (*a != '\0' && *b != '\0') {
        int caractereA = tolower((unsigned char)*a);
        int caractereB = tolower((unsigned char)*b);

        if (caractereA != caractereB) {
            return caractereA - caractereB;
        }

        a++;
        b++;
    }

    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

/* Verifica se um texto aparece dentro de outro, sem diferenciar caixa. */
int contemTexto(const char *texto, const char *termo) {
    size_t tamanhoTexto = strlen(texto);
    size_t tamanhoTermo = strlen(termo);

    if (tamanhoTermo == 0) {
        return 1;
    }

    if (tamanhoTermo > tamanhoTexto) {
        return 0;
    }

    for (size_t i = 0; i <= tamanhoTexto - tamanhoTermo; i++) {
        size_t j = 0;

        while (j < tamanhoTermo &&
               tolower((unsigned char)texto[i + j]) ==
               tolower((unsigned char)termo[j])) {
            j++;
        }

        if (j == tamanhoTermo) {
            return 1;
        }
    }

    return 0;
}

/* Exibe os dados de um anime. */
void exibirAnime(const Anime *anime) {
    printf("\nTitulo: %s\n", anime->titulo);
    printf("Genero: %s\n", anime->genero);
    printf("Classificacao indicativa: %d anos\n", anime->classificacao);
    printf("Episodios: %d\n", anime->episodios);
    printf("----------------------------------------\n");
}

/* Lista todos os animes cadastrados. */
void listarAnimes(void) {
    printf("\n========== CATALOGO DE ANIMES ==========\n");

    for (int i = 0; i < TOTAL_ANIMES; i++) {
        exibirAnime(&catalogo[i]);
    }

    printf("Total de animes: %d\n", TOTAL_ANIMES);
}

/* Mostra os generos cadastrados, sem repetir. */
void listarGeneros(void) {
    char generos[TOTAL_ANIMES][30];
    int totalGeneros = 0;

    printf("\n========== GENEROS DISPONIVEIS ==========\n");

    for (int i = 0; i < TOTAL_ANIMES; i++) {
        int jaExiste = 0;

        for (int j = 0; j < totalGeneros; j++) {
            if (compararSemDiferenciarMaiusculas(
                    catalogo[i].genero, generos[j]) == 0) {
                jaExiste = 1;
                break;
            }
        }

        if (!jaExiste) {
            strcpy(generos[totalGeneros], catalogo[i].genero);
            totalGeneros++;
        }
    }

    for (int i = 0; i < totalGeneros; i++) {
        printf("- %s\n", generos[i]);
    }
}

/* Mostra as classificacoes indicativas cadastradas, sem repetir. */
void listarClassificacoes(void) {
    int existe[19] = {0};

    printf("\nClassificacoes disponiveis:\n");

    for (int i = 0; i < TOTAL_ANIMES; i++) {
        existe[catalogo[i].classificacao] = 1;
    }

    for (int c = 0; c <= 18; c++) {
        if (existe[c]) {
            printf("- %d anos\n", c);
        }
    }
}

/* Busca animes por titulo, genero e classificacao (filtros combinaveis). */
void buscarAnime(void) {
    char termo[MAX_TEXTO];
    char generoEscolhido[30];
    int classificacao;
    int encontrados = 0;

    printf("\n========== BUSCAR ANIME ==========\n");
    lerTexto("Titulo ou parte dele (Enter para ignorar): ",
             termo, sizeof(termo));

    listarGeneros();
    lerTexto("\nGenero (Enter para ignorar): ",
             generoEscolhido, sizeof(generoEscolhido));

    listarClassificacoes();
    classificacao = lerInteiro("\nClassificacao em anos (0 para ignorar): ",
                               0, 18);

    if (termo[0] == '\0' && generoEscolhido[0] == '\0' && classificacao == 0) {
        printf("\nInforme pelo menos um filtro: titulo, genero ou classificacao.\n");
        return;
    }

    for (int i = 0; i < TOTAL_ANIMES; i++) {
        int tituloOk = contemTexto(catalogo[i].titulo, termo);
        int generoOk = generoEscolhido[0] == '\0' ||
                       compararSemDiferenciarMaiusculas(
                           catalogo[i].genero, generoEscolhido) == 0;
        int classificacaoOk = classificacao == 0 ||
                              catalogo[i].classificacao == classificacao;

        if (tituloOk && generoOk && classificacaoOk) {
            exibirAnime(&catalogo[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("\nNenhum anime corresponde a busca.\n");
    } else {
        printf("\nForam encontrados %d resultado(s).\n", encontrados);
    }
}

/* Exibe o menu principal. */
void exibirMenu(void) {
    printf("\n========================================\n");
    printf("              ANIMEFINDER\n");
    printf("       Catalogo e recomendacao\n");
    printf("========================================\n");
    printf("1 - Listar todos os animes\n");
    printf("2 - Buscar por titulo, genero e classificacao\n");
    printf("3 - Ver generos disponiveis\n");
    printf("0 - Sair\n");
    printf("========================================\n");
}

int main(void) {
    int opcao;

    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ", 0, 3);

        switch (opcao) {
            case 1:
                listarAnimes();
                break;

            case 2:
                buscarAnime();
                break;

            case 3:
                listarGeneros();
                break;

            case 0:
                printf("\nObrigado por utilizar o AnimeFinder!\n");
                break;

            default:
                /* A validacao de lerInteiro impede chegar aqui. */
                printf("\nOpcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}
