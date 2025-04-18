#include <stdio.h>
#include <stdlib.h>
#include "processo.h"

int main(void) {
    // Parte 4: nome fixo do CSV
    const char *arquivo = "processo_043_202409032338.csv";

    int n;
    char *header;
    Processo *vet = lerProcessos(arquivo, &n, &header);

    // Ordenar por ID e gerar CSV
    ordenarPorId(vet, n);
    escreverOrdenadoId(vet, n, header, "ordenado_por_id.csv");
    printf("Arquivo 'ordenado_por_id.csv' criado.\n");

    printf("\n");

    // Ordenar por DATA e gerar CSV
    ordenarPorData(vet, n);
    escreverOrdenadoData(vet, n, header, "ordenado_por_data.csv");
    printf("Arquivo 'ordenado_por_data.csv' criado.\n");

    printf("\n");

    // Contar processos com ID_CLASSE = 12554
    int id_classe_fixo = 12554;
    printf("Total de processos com ID_CLASSE=%d: %d\n",
           id_classe_fixo, contarPorClasse(vet, n, id_classe_fixo));

    printf("\n");

    // Contar ID_ASSUNTOS únicos
    printf("Assuntos únicos: %d\n", contarAssuntosUnicos(vet, n));

    printf("\n");

    // Listar processos com >1 assunto
    listarProcessosMultiplosAssuntos(vet, n);

    printf("\n");
    
    // Calcular dias em tramitação para ID = 638633058
    long id_processo_fixo = 638633058;
    int encontrado = 0;
    for (int i = 0; i < n; i++) {
        if (vet[i].id == id_processo_fixo) {
            printf("ID %ld está há %d dias em tramitação.\n",
                   id_processo_fixo,
                   diasEmTramitacao(vet[i].data_ajuizamento));
            encontrado = 1;
            break;
        }
    }
    if (!encontrado)
        printf("Processo com ID %ld não encontrado.\n", id_processo_fixo);

    liberarProcessos(vet, n, header);
    return 0;
}
