#ifndef PROCESSO_H
#define PROCESSO_H

typedef struct {
    long id;                     // Identificador único do processo
    char numero[20];             // Número do processo
    char data_ajuizamento[20];   // Data de ajuizamento no formato "YYYY-MM-DD HH:MM:SS"
    int id_classe;               // Classe vinculada ao processo
    int id_assunto;              // Assunto vinculado ao processo
    int ano_eleicao;             // Ano da eleição relacionada
} Processo;

// Função para carregar os processos de um arquivo CSV
Processo* carregarProcessos(const char* nomeArquivo, int* qtd);

// Função para ordenar os processos por ID (ordem crescente)
void ordenarPorId(Processo* processos, int qtd);

// Função para ordenar os processos por data de ajuizamento (ordem decrescente)
void ordenarPorData(Processo* processos, int qtd);

// Função para contar o número de processos vinculados a um ID de classe específico
int contarPorClasse(Processo* processos, int qtd, int id_classe);

// Função para contar o número de assuntos únicos nos processos
int contarAssuntosUnicos(Processo* processos, int qtd);

// Função para calcular quantos dias um processo está em tramitação
int calcularDiasTramitacao(const char* dataAjuizamento);

// Função para salvar os processos ordenados em um arquivo CSV
void salvarCSV(Processo* processos, int qtd, const char* nomeArquivo);

#endif // PROCESSO_H
