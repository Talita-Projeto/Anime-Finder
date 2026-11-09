
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
    {"Nisekoi", "Romance", 14, 32},
    {"Death Note", "Suspense", 16, 37},
    {"Spy x Family", "Comedia", 12, 37},
    {"The Quintessential Quintuplets", "Romance", 14, 29},
    {"Attack on Titan", "Acao", 16, 87},
    {"JoJo's Bizarre Adventure", "Acao", 18, 190},
    {"Umamusume Cinderella Gray", "Esporte", 10, 85},
    {"Frieren", "Fantasia", 12, 28},
    {"Machikado Mazoku", "Comedia", 10, 24},
    {"Fullmetal Alchemist: Brotherhood", "Aventura", 14, 64},
    {"Violet Evergarden", "Drama", 12, 13},
    {"Mob Psycho 100", "Acao", 12, 37},
    {"Monster", "Suspense", 16, 57},
    {"Steins;Gate", "Drama", 14, 25}
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

/* Recomenda animes conforme genero e idade informados. */
void recomendarAnimes(void) {
    char generoEscolhido[30];
    int idade;
    int encontrados = 0;

    printf("\n========== RECOMENDAR ANIMES ==========\n");
    listarGeneros();

    lerTexto("\nDigite o genero desejado: ",
             generoEscolhido, sizeof(generoEscolhido));

    idade = lerInteiro("Digite sua idade: ", 1, 120);

    printf("\nResultados para o genero %s e idade %d:\n",
           generoEscolhido, idade);

    for (int i = 0; i < TOTAL_ANIMES; i++) {
        if (compararSemDiferenciarMaiusculas(
                catalogo[i].genero, generoEscolhido) == 0 &&
            catalogo[i].classificacao <= idade) {
            exibirAnime(&catalogo[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("\nNenhum anime encontrado com esses criterios.\n");
        printf("Tente outro genero ou uma idade diferente.\n");
    } else {
        printf("\nForam encontrados %d anime(s).\n", encontrados);
    }

    printf("\nObservacao: a classificacao e um filtro informativo; "
           "consulte a classificacao oficial da obra.\n");
}

/* Busca animes por parte do titulo. */
void buscarAnime(void) {
    char termo[MAX_TEXTO];
    int encontrados = 0;

    printf("\n========== BUSCAR ANIME ==========\n");
    lerTexto("Digite o titulo ou parte dele: ", termo, sizeof(termo));

    if (termo[0] == '\0') {
        printf("A busca nao pode ficar vazia.\n");
        return;
    }

    for (int i = 0; i < TOTAL_ANIMES; i++) {
        if (contemTexto(catalogo[i].titulo, termo)) {
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
    printf("2 - Recomendar por genero e idade\n");
    printf("3 - Buscar anime pelo titulo\n");
    printf("4 - Ver generos disponiveis\n");
    printf("0 - Sair\n");
    printf("========================================\n");
}

int main(void) {
    int opcao;

    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ", 0, 4);

        switch (opcao) {
            case 1:
                listarAnimes();
                break;

            case 2:
                recomendarAnimes();
                break;

            case 3:
                buscarAnime();
                break;

            case 4:
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
