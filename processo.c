#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "processo.h"

#define MAX_LINHA 256

// Função para carregar processos de um arquivo CSV
Processo* carregarProcessos(const char* nomeArquivo, int* qtd) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    char linha[MAX_LINHA];
    int capacidade = 10;
    *qtd = 0;

    Processo* processos = (Processo*)malloc(capacidade * sizeof(Processo));
    if (!processos) {
        perror("Erro ao alocar memória");
        fclose(arquivo);
        return NULL;
    }

    // Ignorar a primeira linha (cabeçalho)
    fgets(linha, MAX_LINHA, arquivo);

    while (fgets(linha, MAX_LINHA, arquivo)) {
        // Remover caracteres especiais no final da linha, como '\n' ou '\r'
        linha[strcspn(linha, "\r\n")] = 0;

        if (*qtd >= capacidade) {
            capacidade *= 2;
            processos = (Processo*)realloc(processos, capacidade * sizeof(Processo));
            if (!processos) {
                perror("Erro ao realocar memória");
                fclose(arquivo);
                return NULL;
            }
        }

        // Parse da linha CSV
        char* token = strtok(linha, ",");
        if (token) processos[*qtd].id = atol(token);

        token = strtok(NULL, ",");
        if (token) strncpy(processos[*qtd].numero, token, sizeof(processos[*qtd].numero) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(processos[*qtd].data_ajuizamento, token, sizeof(processos[*qtd].data_ajuizamento) - 1);

        token = strtok(NULL, ",{}");
        if (token) processos[*qtd].id_classe = atoi(token);

        token = strtok(NULL, ",{}");
        if (token) processos[*qtd].id_assunto = atoi(token);

        token = strtok(NULL, ",");
        if (token) processos[*qtd].ano_eleicao = atoi(token);

        (*qtd)++;
    }

    fclose(arquivo);
    return processos;
}


// Função para ordenar processos por ID (ordem crescente)
void ordenarPorId(Processo* processos, int qtd) {
    for (int i = 0; i < qtd - 1; i++) {
        for (int j = 0; j < qtd - i - 1; j++) {
            if (processos[j].id > processos[j + 1].id) {
                Processo temp = processos[j];
                processos[j] = processos[j + 1];
                processos[j + 1] = temp;
            }
        }
    }
}

// Função para converter string de data para time_t
time_t stringParaTime(const char* data) {
    struct tm tm_data = {0};
    sscanf(data, "%d-%d-%d %d:%d:%d",
           &tm_data.tm_year, &tm_data.tm_mon, &tm_data.tm_mday,
           &tm_data.tm_hour, &tm_data.tm_min, &tm_data.tm_sec);
    tm_data.tm_year -= 1900; // Ajuste para struct tm
    tm_data.tm_mon -= 1;     // Ajuste para struct tm
    return mktime(&tm_data);
}

// Função para ordenar processos por data (ordem decrescente)
void ordenarPorData(Processo* processos, int qtd) {
    for (int i = 0; i < qtd - 1; i++) {
        for (int j = 0; j < qtd - i - 1; j++) {
            time_t data1 = stringParaTime(processos[j].data_ajuizamento);
            time_t data2 = stringParaTime(processos[j + 1].data_ajuizamento);
            if (data1 < data2) {
                Processo temp = processos[j];
                processos[j] = processos[j + 1];
                processos[j + 1] = temp;
            }
        }
    }
}

// Função para contar processos vinculados a um ID de classe específico
int contarPorClasse(Processo* processos, int qtd, int id_classe) {
    int count = 0;
    for (int i = 0; i < qtd; i++) {
        if (processos[i].id_classe == id_classe) {
            count++;
        }
    }
    return count;
}

// Função para contar o número de assuntos únicos
int contarAssuntosUnicos(Processo* processos, int qtd) {
    int* assuntos = (int*)malloc(qtd * sizeof(int));
    int count = 0;

    for (int i = 0; i < qtd; i++) {
        int found = 0;
        for (int j = 0; j < count; j++) {
            if (assuntos[j] == processos[i].id_assunto) {
                found = 1;
                break;
            }
        }
        if (!found) {
            assuntos[count++] = processos[i].id_assunto;
        }
    }

    free(assuntos);
    return count;
}

// Função para calcular os dias de tramitação de um processo
int calcularDiasTramitacao(const char* dataAjuizamento) {
    struct tm data;
    time_t agora, ajuizamento;

    sscanf(dataAjuizamento, "%d-%d-%d %d:%d:%d",
           &data.tm_year, &data.tm_mon, &data.tm_mday,
           &data.tm_hour, &data.tm_min, &data.tm_sec);

    data.tm_year -= 1900;
    data.tm_mon -= 1;
    data.tm_isdst = -1;

    ajuizamento = mktime(&data);
    agora = time(NULL);

    return (int)difftime(agora, ajuizamento) / (60 * 60 * 24);
}

// Função para salvar os processos em arquivo CSV
void salvarCSV(Processo* processos, int qtd, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "w");
    if (!arquivo) {
        perror("Erro ao criar o arquivo");
        return;
    }

    fprintf(arquivo, "id,numero,data_ajuizamento,id_classe,id_assunto,ano_eleicao\n");
    for (int i = 0; i < qtd; i++) {
        fprintf(arquivo, "%ld,%s,%s,{%d},{%d},%d\n",
                processos[i].id, processos[i].numero, processos[i].data_ajuizamento,
                processos[i].id_classe, processos[i].id_assunto, processos[i].ano_eleicao);
    }

    fclose(arquivo);
}
