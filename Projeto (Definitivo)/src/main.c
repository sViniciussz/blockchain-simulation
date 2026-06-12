/*
 * PROJETO FINAL: SIMULAÇÃO DE BLOCKCHAIN (ED2)
 *
 * Integrantes e RA: 
 * - Arthur Hanczuruk Borba / 2606208
 * - Vinicius dos Santos de Souza / 2665484
 * - Felipe de Brito Santos / 2713861
 * 
 * Funções:
 * FUNÇÃO 1 (CARTEIRA, HASH E GERADOR) - ARTHUR
 * FUNÇÃO 2 (INDEXADOR) - FELIPE
 * FUNÇÃO 3 (ANALISE, MENU) - VINICIUS
 * 
 * Compilação (Linux/WSL): 
 * "gcc projeto_blockchain.c mtwister.c -o projeto -lssl -lcrypto" 
 */


#include <stdio.h>
#include <stdlib.h>
#include "blockchain.h"
#include "analise.h"

int main() {
    int opcao;
    
    do {
        printf("\n#########################################\n");
        printf("   PROJETO FINAL BLOCKCHAIN (ED2)        \n");
        printf("#########################################\n");
        printf("1. GERAR Blockchain (Cria .bin)\n");
        printf("2. ANALISAR Blockchain\n");
        printf("3. GERAR ARQUIVO TEXTO \n");
        printf("0. Sair\n");
        printf("Escolha: ");
        
        if (scanf("%d", &opcao) != 1) { 
            while(getchar()!='\n'); opcao=-1; 
        }

        switch(opcao) {
            case 1: 
                executarGerador(); 
                break;
            case 2: 
                exibirMenuAnalise(); 
                break;
            case 3:
                gerarArquivoTextoCompleto();
                break;
            case 0: 
                printf("Encerrando...\n"); 
                break;
            default: 
                printf("Opcao invalida.\n");
        }
    } while(opcao != 0);

    return 0;
}