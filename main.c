#include <stdio.h>
#include <stdlib.h>
#include "processo.h"

void menu() {
    printf("=== Gerenciador de Processos ===\n");
    printf("1. Ordenar por ID (crescente) e salvar em CSV\n");
    printf("2. Ordenar por Data de Ajuizamento (decrescente) e salvar em CSV\n");
    printf("3. Contar processos por ID de Classe\n");
    printf("4. Contar Assuntos Únicos\n");
    printf("5. Calcular Dias de Tramitação de um Processo\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
}

int main() {
    int qtd;
    Processo* processos = carregarProcessos("processo_043_202409032338.csv", &qtd);

    if (!processos) {
        fprintf(stderr, "Erro ao carregar os processos.\n");
        return 1;
    }

    int opcao;
    do {
        menu();
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: {
                ordenarPorId(processos, qtd);
                salvarCSV(processos, qtd, "processos_ordenados_por_id.csv");
                printf("Processos ordenados por ID (crescente) e salvos em 'processos_ordenados_por_id.csv'.\n");
                break;
            }
                
            case 2: {
                ordenarPorData(processos, qtd);
                salvarCSV(processos, qtd, "processos_ordenados_por_data.csv");
                printf("Processos ordenados por Data de Ajuizamento (decrescente) e salvos em 'processos_ordenados_por_data.csv'.\n");
                break;
            }

            case 3: {
                int id_classe;
                printf("Digite o ID da classe: ");
                scanf("%d", &id_classe);
                int count = contarPorClasse(processos, qtd, id_classe);
                printf("Número de processos vinculados ao ID de classe %d: %d\n", id_classe, count);
                break;
            }

            case 4: {
                int count = contarAssuntosUnicos(processos, qtd);
                printf("Número de assuntos únicos: %d\n", count);
                break;
            }

            case 5: {
                char data[20];
                printf("Digite a data de ajuizamento (YYYY-MM-DD HH:MM:SS): ");
                scanf("%s", data);
                int dias = calcularDiasTramitacao(data);
                printf("O processo está em tramitação há %d dias.\n", dias);
                break;
            }

            case 0:
                printf("Saindo...\n");
                break;

            default:
                printf("Opção inválida.\n");
        }

    } while (opcao != 0);

    free(processos);
    return 0;
}
