#ifndef PROCESSO_H
#define PROCESSO_H

#include <time.h>

// Tipo Abstrato de Dados para um Processo
typedef struct {
    long   id;                    // identificador
    char   numero[32];            // número do processo
    char   data_ajuizamento[24];  // "YYYY-MM-DD hh:mm:ss.fff"
    int    id_classe;             // classe vinculada
    char  *assuntos;              // multivalorados "{123,456}"
    int    ano_eleicao;           // ano da eleição
} Processo;

// Lê todo o CSV e devolve um vetor dinâmico de Processo.
// outN recebe o número de registros lidos.
// header recebe a linha de cabeçalho (que deve ser reimpressa nos CSVs de saída).
Processo *lerProcessos(const char *arquivo, int *outN, char **header);

// Libera toda a memória alocada pelo vetor de Processo
void liberarProcessos(Processo *v, int n, char *header);

// Itens 1 e 2: ordenação manual + escrita CSV mantendo cabeçalho
void ordenarPorId(Processo *v, int n);
void escreverOrdenadoId(Processo *v, int n, const char *header, const char *saida);

void ordenarPorData(Processo *v, int n);
void escreverOrdenadoData(Processo *v, int n, const char *header, const char *saida);

// Item 3: conta quantos processos têm determinado id_classe
int contarPorClasse(Processo *v, int n, int classe);

// Item 4: conta quantos id_assuntos distintos aparecem na base
int contarAssuntosUnicos(Processo *v, int n);

// Item 5: imprime todos os processos com >1 assunto
void listarProcessosMultiplosAssuntos(Processo *v, int n);

// Item 6: retorna quantos dias o processo está em tramitação
int diasEmTramitacao(const char *data);

#endif // PROCESSO_H
