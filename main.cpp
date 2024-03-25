#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Corrigido para ctype.h, que contém a função tolower()

#define MAX_PALAVRA 50
#define MAX_LINHA 100

struct Dimensoes {
    int linhas;
    int colunas;
};

struct Palavra {
    char palavra[MAX_PALAVRA];
    int encontrada;
    int coordenadas[2]; // [linha, coluna]
    char direcao[50]; // Aumentado o tamanho para acomodar todas as direções
};

// Função para abrir um arquivo
FILE* abre_arquivo(char* nome_arquivo, char* modo) {
    FILE* arquivo = fopen(nome_arquivo, modo);
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nome_arquivo);
        exit(1);
    }
    return arquivo;
}

// Função para ler as dimensões da matriz do arquivo letras.txt
struct Dimensoes ler_dimensoes(FILE* arquivo) {
    struct Dimensoes dim;
    fscanf(arquivo, "%d %d", &dim.linhas, &dim.colunas);
    return dim;
}

// Função para criar e preencher uma matriz com os caracteres do arquivo
char** cria_matriz_e_preenche(FILE* arquivo, struct Dimensoes dim) {
    char** matriz = (char**)malloc(dim.linhas * sizeof(char*));
    for (int i = 0; i < dim.linhas; i++) {
        matriz[i] = (char*)malloc(dim.colunas * sizeof(char));
        for (int j = 0; j < dim.colunas; j++) {
            fscanf(arquivo, " %c", &matriz[i][j]);
        }
    }
    for (int i = 0; i < dim.linhas; i++) {
        for (int j = 0; j < dim.colunas; j++) {
            printf("%c", matriz[i][j]);
        }
        printf("\n"); // Nova linha após cada linha da matriz
    }
    return matriz;
}

// Função para ler o arquivo de palavras e criar uma lista de palavras
void ler_arquivo_palavras(FILE* arquivo, char palavras[][MAX_PALAVRA], int* num_palavras) {
    *num_palavras = 0;
    while (fscanf(arquivo, "%s", palavras[*num_palavras]) == 1) {
        (*num_palavras)++;
    }
}

// Função para converter uma string para maiúsculas
void converter_para_maiusculas(char* palavra) {
    for (int i = 0; palavra[i] != '\0'; i++) {
        palavra[i] = toupper(palavra[i]);
    }
}

void escrever_resultados(FILE* arquivo_saida, char** matriz, struct Dimensoes dim, struct Palavra* resultados, int num_palavras) {
    // Escrever a BOM (Byte Order Mark) para indicar UTF-8
    fputc(0xEF, arquivo_saida);
    fputc(0xBB, arquivo_saida);
    fputc(0xBF, arquivo_saida);
    // Escrever a matriz no arquivo de saída com as palavras encontradas em maiúsculas
    for (int i = 0; i < dim.linhas; i++) {
        for (int j = 0; j < dim.colunas; j++) {
            int palavra_encontrada = 0; // Flag para verificar se uma palavra foi encontrada nesta posição
            // Verificar se a posição atual contém uma palavra encontrada
            for (int k = 0; k < num_palavras; k++) {
                if (resultados[k].encontrada && resultados[k].coordenadas[0] == i && resultados[k].coordenadas[1] == j) {
                    palavra_encontrada = 1;
                    converter_para_maiusculas(resultados[k].palavra); // Converter para maiúsculas
                    fprintf(arquivo_saida, "%s ", resultados[k].palavra);
                    break; // Parar de procurar mais palavras na mesma posição
                }
            }
            // Se não for uma palavra encontrada, escrever o caractere da matriz normalmente
            if (!palavra_encontrada) {
                fprintf(arquivo_saida, "%c ", matriz[i][j]);
            } else {
                fprintf(arquivo_saida, "  "); // Manter o espaço entre as palavras
            }
        }
        fprintf(arquivo_saida, "\n");
    }
    fprintf(arquivo_saida, "\n");

    // Escrever os resultados das palavras no arquivo de saída
    for (int i = 0; i < num_palavras; i++) {
        fprintf(arquivo_saida, "%d° palavra: %s, ", i + 1, resultados[i].palavra);
        if (resultados[i].encontrada) {
            fprintf(arquivo_saida, "foi encontrada! Sua primeira letra está nas coordenadas linha %d coluna %d, [%d, %d], na direção %s.\n",
                    resultados[i].coordenadas[0] + 1, resultados[i].coordenadas[1] + 1,
                    resultados[i].coordenadas[1], resultados[i].coordenadas[0], // Correção da ordem das coordenadas
                    resultados[i].direcao);
        } else {
            fprintf(arquivo_saida, "não foi encontrada.\n");
        }
    }
}

void buscar_horizontal_ed(char** matriz, struct Dimensoes dim, char* palavra, struct Palavra* resultado) {
    int tamanho_palavra = strlen(palavra);
    for (int i = 0; i < dim.linhas; i++) {
        for (int j = 0; j <= dim.colunas - tamanho_palavra; j++) {
            int k;
            for (k = 0; k < tamanho_palavra; k++) {
                // Converter ambos os caracteres para minúsculas antes de comparar
                if (tolower(matriz[i][j + k]) != tolower(palavra[k]))
                    break;
            }
            if (k == tamanho_palavra) {
                resultado->encontrada = 1;
                resultado->coordenadas[0] = i;
                resultado->coordenadas[1] = j;
                strcpy(resultado->direcao, "Horizontal da esquerda para a direita");
                return;
            }
        }
    }
    // Se a palavra não for encontrada, define o resultado como não encontrada
    resultado->encontrada = 0;
}

// Função para buscar uma palavra na matriz na horizontal da direita para a esquerda
void buscar_horizontal_de(char** matriz, struct Dimensoes dim, char* palavra, struct Palavra* resultado) {
    int tamanho_palavra = strlen(palavra);
    for (int i = 0; i < dim.linhas; i++) {
        for (int j = dim.colunas - 1; j >= tamanho_palavra - 1; j--) {
            int k;
            for (k = 0; k < tamanho_palavra; k++) {
                if (tolower(matriz[i][j - k]) != tolower(palavra[k]))
                    break;
            }
            if (k == tamanho_palavra) {
                resultado->encontrada = 1;
                resultado->coordenadas[0] = i;
                resultado->coordenadas[1] = j - tamanho_palavra + 1;
                strcpy(resultado->direcao, "Horizontal da direita para a esquerda");
                return;
            }
        }
    }
    // Se a palavra não for encontrada, define o resultado como não encontrada
    resultado->encontrada = 0;
}

// Função para buscar uma palavra na matriz na vertical de cima para baixo
void buscar_vertical_cb(char** matriz, struct Dimensoes dim, char* palavra, struct Palavra* resultado) {
    int tamanho_palavra = strlen(palavra);
    for (int j = 0; j < dim.colunas; j++) {
        for (int i = 0; i <= dim.linhas - tamanho_palavra; i++) {
            int k;
            for (k = 0; k < tamanho_palavra; k++) {
                if (tolower(matriz[i + k][j]) != tolower(palavra[k]))
                    break;
            }
            if (k == tamanho_palavra) {
                resultado->encontrada = 1;
                resultado->coordenadas[0] = i;
                resultado->coordenadas[1] = j;
                strcpy(resultado->direcao, "Vertical de cima para baixo");
                return;
            }
        }
    }
    // Se a palavra não for encontrada, define o resultado como não encontrada
    resultado->encontrada = 0;
}

// Função para buscar uma palavra na matriz na vertical de baixo para cima
void buscar_vertical_bc(char** matriz, struct Dimensoes dim, char* palavra, struct Palavra* resultado) {
    int tamanho_palavra = strlen(palavra);
    for (int j = 0; j < dim.colunas; j++) {
        for (int i = dim.linhas - 1; i >= tamanho_palavra - 1; i--) {
            int k;
            for (k = 0; k < tamanho_palavra; k++) {
                if (tolower(matriz[i - k][j]) != tolower(palavra[k]))
                    break;
            }
            if (k == tamanho_palavra) {
                resultado->encontrada = 1;
                resultado->coordenadas[0] = i - tamanho_palavra + 1;
                resultado->coordenadas[1] = j;
                strcpy(resultado->direcao, "Vertical de baixo para cima");
                return;
            }
        }
    }
    // Se a palavra não for encontrada, define o resultado como não encontrada
    resultado->encontrada = 0;
}


// Função para buscar uma palavra na matriz na diagonal de cima para baixo da esquerda para a direita
void buscar_diagonal_cb_ed(char** matriz, struct Dimensoes dim, char* palavra, struct Palavra* resultado) {
    for (int i = 0; i <= dim.linhas - strlen(palavra); i++) {
        for (int j = 0; j <= dim.colunas - strlen(palavra); j++) {
            int k;
            for (k = 0; k < strlen(palavra); k++) {
                if (matriz[i + k][j + k] != palavra[k])
                    break;
            }
            if (k == strlen(palavra)) {
                resultado->encontrada = 1;
                resultado->coordenadas[0] = i;
                resultado->coordenadas[1] = j;
                strcpy(resultado->direcao, "Diagonal de cima para baixo da esquerda para a direita");
                return;
            }
        }
    }
}

// Função para buscar uma palavra na matriz na diagonal de cima para baixo da direita para a esquerda
void buscar_diagonal_cb_de(char** matriz, struct Dimensoes dim, char* palavra, struct Palavra* resultado) {
    for (int i = 0; i <= dim.linhas - strlen(palavra); i++) {
        for (int j = dim.colunas - 1; j >= strlen(palavra) - 1; j--) {
            int k;
            for (k = 0; k < strlen(palavra); k++) {
                if (matriz[i + k][j - k] != palavra[k])
                    break;
            }
            if (k == strlen(palavra)) {
                resultado->encontrada = 1;
                resultado->coordenadas[0] = i;
                resultado->coordenadas[1] = j;
                strcpy(resultado->direcao, "Diagonal de cima para baixo da direita para a esquerda");
                return;
            }
        }
    }
}

// Função para buscar uma palavra na matriz na diagonal de baixo para cima da esquerda para a direita
void buscar_diagonal_bc_ed(char** matriz, struct Dimensoes dim, char* palavra, struct Palavra* resultado) {
    for (int i = dim.linhas - 1; i >= strlen(palavra) - 1; i--) {
        for (int j = 0; j <= dim.colunas - strlen(palavra); j++) {
            int k;
            for (k = 0; k < strlen(palavra); k++) {
                if (matriz[i - k][j + k] != palavra[k])
                    break;
            }
            if (k == strlen(palavra)) {
                resultado->encontrada = 1;
                resultado->coordenadas[0] = i;
                resultado->coordenadas[1] = j;
                strcpy(resultado->direcao, "Diagonal de baixo para cima da esquerda para a direita");
                return;
            }
        }
    }
}

// Função para buscar uma palavra na matriz na diagonal de baixo para cima da direita para a esquerda
void buscar_diagonal_bc_de(char** matriz, struct Dimensoes dim, char* palavra, struct Palavra* resultado) {
    for (int i = dim.linhas - 1; i >= strlen(palavra) - 1; i--) {
        for (int j = dim.colunas - 1; j >= strlen(palavra) - 1; j--) {
            int k;
            for (k = 0; k < strlen(palavra); k++) {
                if (matriz[i - k][j - k] != palavra[k])
                    break;
            }
            if (k == strlen(palavra)) {
                resultado->encontrada = 1;
                resultado->coordenadas[0] = i;
                resultado->coordenadas[1] = j;
                strcpy(resultado->direcao, "Diagonal de baixo para cima da direita para a esquerda");
                return;
            }
        }
    }
}

// Função para escrever os resultados no arquivo de saída
//void escrever_resultados(FILE* arquivo_saida, char** matriz, struct Dimensoes dim, struct Palavra* resultados, int num_palavras) {
//    // Escrever a matriz no arquivo de saída
//    for (int i = 0; i < dim.linhas; i++) {
//        for (int j = 0; j < dim.colunas; j++) {
//            fprintf(arquivo_saida, "%c ", matriz[i][j]);
//        }
//        fprintf(arquivo_saida, "\n");
//    }
//    fprintf(arquivo_saida, "\n");
//
//    // Escrever os resultados das palavras no arquivo de saída
//    for (int i = 0; i < num_palavras; i++) {
//        fprintf(arquivo_saida, "%d° palavra: %s, ", i + 1, resultados[i].palavra);
//        if (resultados[i].encontrada) {
//            fprintf(arquivo_saida, "foi encontrada! Sua primeira letra está nas coordenadas linha %d coluna %d, [%d, %d], na direção %s.\n",
//                    resultados[i].coordenadas[0] + 1, resultados[i].coordenadas[1] + 1,
//                    resultados[i].coordenadas[0], resultados[i].coordenadas[1],
//                    resultados[i].direcao);
//        } else {
//            fprintf(arquivo_saida, "não foi encontrada.\n");
//        }
//    }
//}


int main() {
    // Abrir arquivo letras.txt e ler dimensões
    FILE* arquivo_letras = abre_arquivo("C:\\Users\\laion\\CLionProjects\\SO_casa_palavras_MI_VF\\letras.txt", "r");
    struct Dimensoes dim = ler_dimensoes(arquivo_letras);

    // Criar e preencher a matriz com os caracteres do arquivo letras.txt
    char** matriz = cria_matriz_e_preenche(arquivo_letras, dim);
    fclose(arquivo_letras);

    // Abrir arquivo palavras.txt e ler palavras
    FILE* arquivo_palavras = abre_arquivo("C:\\Users\\laion\\CLionProjects\\SO_casa_palavras_MI_VF\\palavras.txt", "r");
    char palavras[MAX_LINHA][MAX_PALAVRA];
    int num_palavras;
    ler_arquivo_palavras(arquivo_palavras, palavras, &num_palavras);
    fclose(arquivo_palavras);

    // Cria aqui a estrutura para armazenar os resultados da busca
    struct Palavra resultados[num_palavras];

    // Buscar cada palavra na matriz em diferentes direções
    for (int i = 0; i < num_palavras; i++) {
        // Buscar na horizontal da esquerda para a direita
        buscar_horizontal_ed(matriz, dim, palavras[i], &resultados[i]);

        // Buscar na horizontal da direita para a esquerda
        buscar_horizontal_de(matriz, dim, palavras[i], &resultados[i]);

        // Buscar na vertical de cima para baixo
        buscar_vertical_cb(matriz, dim, palavras[i], &resultados[i]);

        // Buscar na vertical de baixo para cima
        buscar_vertical_bc(matriz, dim, palavras[i], &resultados[i]);

        // Buscar na diagonal de cima para baixo da esquerda para a direita
        buscar_diagonal_cb_ed(matriz, dim, palavras[i], &resultados[i]);

        // Buscar na diagonal de cima para baixo da direita para a esquerda
        buscar_diagonal_cb_de(matriz, dim, palavras[i], &resultados[i]);

        // Buscar na diagonal de baixo para cima da esquerda para a direita
        buscar_diagonal_bc_ed(matriz, dim, palavras[i], &resultados[i]);

        // Buscar na diagonal de baixo para cima da direita para a esquerda
        buscar_diagonal_bc_de(matriz, dim, palavras[i], &resultados[i]);
    }

    // Abrir arquivo de saída
    FILE* arquivo_saida = abre_arquivo("C:\\Users\\laion\\CLionProjects\\SO_casa_palavras_MI_VF\\saida.txt", "w");

    // Escrever a matriz no arquivo de saída com as palavras encontradas em maiúsculas e os resultados da busca
    escrever_resultados(arquivo_saida, matriz, dim, resultados, num_palavras);

    // Fechar arquivo de saída
    fclose(arquivo_saida);

    // Liberar memória da matriz
    for (int i = 0; i < dim.linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);

    return 0;
}
