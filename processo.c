#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "processo.h"

#define LINHA_MAX 512

// strdup portátil (alguns Windows não têm)
static char *xstrdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *d = malloc(len);
    if (d) memcpy(d, s, len);
    return d;
}

// Separa uma linha CSV em até maxCampos, ignorando vírgulas dentro de {…}
static int splitCSV(const char *linha, char campos[][LINHA_MAX], int maxCampos) {
    int inBraces = 0, campo = 0, pos = 0;
    for (const char *p = linha; *p && campo < maxCampos; ++p) {
        if (*p == '{') inBraces = 1;
        else if (*p == '}') inBraces = 0;
        if (*p == ',' && !inBraces) {
            campos[campo][pos] = '\0';
            campo++; pos = 0;
        } else if (*p != '\r' && *p != '\n') {
            if (pos < LINHA_MAX - 1)
                campos[campo][pos++] = *p;
        }
    }
    campos[campo][pos] = '\0';
    return campo + 1;
}

Processo *lerProcessos(const char *arquivo, int *outN, char **header) {
    FILE *fp = fopen(arquivo, "r");
    if (!fp) { perror("Erro ao abrir arquivo"); exit(EXIT_FAILURE); }

    char linha[LINHA_MAX];
    // lê e guarda o cabeçalho
    if (!fgets(linha, sizeof(linha), fp)) {
        fprintf(stderr, "Arquivo vazio ou inválido\n");
        exit(EXIT_FAILURE);
    }
    *header = xstrdup(linha);

    int cap = 100, n = 0;
    Processo *vet = malloc(cap * sizeof *vet);
    if (!vet) { perror("malloc"); exit(EXIT_FAILURE); }

    while (fgets(linha, sizeof(linha), fp)) {
        if (n >= cap) {
            cap *= 2;
            vet = realloc(vet, cap * sizeof *vet);
            if (!vet) { perror("realloc"); exit(EXIT_FAILURE); }
        }
        char campos[6][LINHA_MAX];
        if (splitCSV(linha, campos, 6) < 6) continue;

        vet[n].id = atol(campos[0]);
        strncpy(vet[n].numero, campos[1], 31);
        vet[n].numero[31] = '\0';
        strncpy(vet[n].data_ajuizamento, campos[2], 23);
        vet[n].data_ajuizamento[23] = '\0';
        vet[n].id_classe = atoi(campos[3] + 1);
        vet[n].assuntos = xstrdup(campos[4]);
        vet[n].ano_eleicao = atoi(campos[5]);
        n++;
    }

    fclose(fp);
    *outN = n;
    return vet;
}

void liberarProcessos(Processo *v, int n, char *header) {
    for (int i = 0; i < n; i++) free(v[i].assuntos);
    free(v);
    free(header);
}

// seleção simples
void ordenarPorId(Processo *v, int n) {
    for (int i = 0; i < n - 1; i++) {
        int min = i;
        for (int j = i + 1; j < n; j++)
            if (v[j].id < v[min].id) min = j;
        if (min != i) {
            Processo tmp = v[i];
            v[i] = v[min];
            v[min] = tmp;
        }
    }
}

void ordenarPorData(Processo *v, int n) {
    for (int i = 0; i < n - 1; i++) {
        int max = i;
        for (int j = i + 1; j < n; j++)
            if (strcmp(v[j].data_ajuizamento, v[max].data_ajuizamento) > 0)
                max = j;
        if (max != i) {
            Processo tmp = v[i];
            v[i] = v[max];
            v[max] = tmp;
        }
    }
}

void escreverOrdenadoId(Processo *v, int n, const char *h, const char *s) {
    Processo *c = malloc(n * sizeof *c);
    for (int i = 0; i < n; i++) {
        c[i] = v[i];
        c[i].assuntos = xstrdup(v[i].assuntos);
    }
    ordenarPorId(c, n);
    FILE *fp = fopen(s, "w");
    fprintf(fp, "%s", h);
    for (int i = 0; i < n; i++)
        fprintf(fp, "%ld,%s,%s,{%d},%s,%d\n",
                c[i].id, c[i].numero, c[i].data_ajuizamento,
                c[i].id_classe, c[i].assuntos, c[i].ano_eleicao);
    fclose(fp);
    for (int i = 0; i < n; i++) free(c[i].assuntos);
    free(c);
}

void escreverOrdenadoData(Processo *v, int n, const char *h, const char *s) {
    Processo *c = malloc(n * sizeof *c);
    for (int i = 0; i < n; i++) {
        c[i] = v[i];
        c[i].assuntos = xstrdup(v[i].assuntos);
    }
    ordenarPorData(c, n);
    FILE *fp = fopen(s, "w");
    fprintf(fp, "%s", h);
    for (int i = 0; i < n; i++)
        fprintf(fp, "%ld,%s,%s,{%d},%s,%d\n",
                c[i].id, c[i].numero, c[i].data_ajuizamento,
                c[i].id_classe, c[i].assuntos, c[i].ano_eleicao);
    fclose(fp);
    for (int i = 0; i < n; i++) free(c[i].assuntos);
    free(c);
}

int contarPorClasse(Processo *v, int n, int classe) {
    int cnt = 0;
    for (int i = 0; i < n; i++)
        if (v[i].id_classe == classe) cnt++;
    return cnt;
}

int contarAssuntosUnicos(Processo *v, int n) {
    int total = 0, *lista = NULL;
    for (int i = 0; i < n; i++) {
        char *s = xstrdup(v[i].assuntos + 1);
        s[strlen(s) - 1] = '\0';
        char *tok = strtok(s, ",");
        while (tok) {
            int id = atoi(tok), ach = 0;
            for (int j = 0; j < total; j++)
                if (lista[j] == id) { ach = 1; break; }
            if (!ach) lista = realloc(lista, (++total) * sizeof(int)), lista[total-1] = id;
            tok = strtok(NULL, ",");
        }
        free(s);
    }
    free(lista);
    return total;
}

void listarProcessosMultiplosAssuntos(Processo *v, int n) {
    printf("\nProcessos com >1 assunto:\n");
    for (int i = 0; i < n; i++) {
        int virg = 0;
        for (char *p = v[i].assuntos; *p; p++)
            if (*p == ',') virg++;
        if (virg > 0)
            printf("ID %ld: %s\n", v[i].id, v[i].assuntos);
    }
}

int diasEmTramitacao(const char *data) {
    struct tm tm = {0};
    sscanf(data, "%d-%d-%d %d:%d:%d",
           &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
           &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    tm.tm_year -= 1900; tm.tm_mon -= 1;
    time_t t0 = mktime(&tm);
    double diff = difftime(time(NULL), t0);
    return (int)(diff / (60*60*24));
}
