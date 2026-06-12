#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "analise.h"
#include "avl.h"      
#include "blockchain.h"

// --- ESTRUTURAS PRIVADAS (CACHE) ---
#define MAX_BLOCOS_POR_MINERADOR 5000 
typedef struct {
    int qtdBlocos;
    int listaNumerosBlocos[MAX_BLOCOS_POR_MINERADOR];
} IndiceMinerador;

static IndiceMinerador indiceEnderecos[QTD_ENDERECOS];
static int indiceEnderecosCarregado = 0;

static No *raizIndiceNonce = NULL; 
static int indiceNonceCarregado = 0;

// --- FUNÇÕES AUXILIARES ---

void gerarArquivoTextoCompleto() {
    FILE *bin = fopen("blockchain.bin", "rb");
    FILE *txt = fopen("blockchain_completa.txt", "w");
    
    if (!bin || !txt) {
        printf("[ERRO] Nao foi possivel abrir os arquivos para conversao.\n");
        if(bin) fclose(bin);
        return;
    }

    printf("[SISTEMA] Gerando arquivo de texto 'blockchain_completa.txt' (Isso pode demorar)...\n");

    BlocoMinerado bm;
    while(fread(&bm, sizeof(BlocoMinerado), 1, bin)) {
        fprintf(txt, "=== BLOCO %u ===\n", bm.bloco.numero);
        fprintf(txt, "Nonce: %u\n", bm.bloco.nonce);
        fprintf(txt, "Hash: ");
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) fprintf(txt, "%02x", bm.hash[i]);
        fprintf(txt, "\nHash Ant: ");
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) fprintf(txt, "%02x", bm.bloco.hashAnterior[i]);
        fprintf(txt, "\nMinerador: %d\n", bm.bloco.data[TAM_DATA - 1]);
        
        // Dados das transações
        if (bm.bloco.numero == 1) {
             fprintf(txt, "Dados: %s\n", bm.bloco.data);
        } else {
            fprintf(txt, "Transacoes:\n");
            for (int i = 0; i < TAM_DATA - 1; i += 3) {
                if (bm.bloco.data[i] == 0 && bm.bloco.data[i+1] == 0 && bm.bloco.data[i+2] == 0) continue;
                fprintf(txt, "   De: %d -> Para: %d | Valor: %d\n", 
                    bm.bloco.data[i], bm.bloco.data[i+1], bm.bloco.data[i+2]);
            }
        }
        fprintf(txt, "--------------------------------------------------\n");
    }

    fclose(bin);
    fclose(txt);
    printf("[SUCESSO] Arquivo texto gerado!\n");
}

void printHash(unsigned char *hash) {
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) printf("%02x", hash[i]);
}

void fprintHash(FILE *f, unsigned char *hash) {
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) fprintf(f, "%02x", hash[i]);
}

int contarTransacoes(BlocoMinerado *bm) {
    if (bm->bloco.numero == 1) return 0; 
    int qtd = 0;
    for (int i = 0; i < TAM_DATA - 1; i += 3) {
        if (bm->bloco.data[i] == 0 && bm->bloco.data[i+1] == 0 && bm->bloco.data[i+2] == 0) continue;
        qtd++;
    }
    return qtd;
}

int compararBlocos(const void *a, const void *b) {
    BlocoMinerado *b1 = (BlocoMinerado *)a;
    BlocoMinerado *b2 = (BlocoMinerado *)b;
    return contarTransacoes(b1) - contarTransacoes(b2);
}

void imprimirConteudoData(unsigned int numeroBloco, unsigned char *data) {
    printf("   [DADOS DO BLOCO]:\n");
    if (numeroBloco == 1) {
        printf("      Mensagem: %s\n", data); 
    } else {
        int temTransacao = 0;
        printf("      Transacoes:\n");
        for (int i = 0; i < TAM_DATA - 1; i += 3) {
            unsigned char orig = data[i];
            unsigned char dest = data[i+1];
            unsigned char val  = data[i+2];
            
            if (orig == 0 && dest == 0 && val == 0) continue;
            
            printf("         -> De: %d | Para: %d | Valor: %d BTC\n", orig, dest, val);
            temTransacao = 1;
        }
        if (!temTransacao) printf("         (Nenhuma transacao)\n");
    }
    printf("      Minerador (Recebeu Recompensa): %d\n", data[TAM_DATA - 1]);
}

// --- FUNÇÕES DE CARREGAMENTO DE ÍNDICE ---
void carregarIndiceEnderecos() {
    if (indiceEnderecosCarregado) return;

    FILE *fIdx = fopen("indice_enderecos.bin", "rb");
    if (fIdx) {
        fread(indiceEnderecos, sizeof(IndiceMinerador), QTD_ENDERECOS, fIdx);
        fclose(fIdx);
        indiceEnderecosCarregado = 1;
        return;
    }

    printf("[SISTEMA] Criando indice de enderecos (RAM)...\n");
    FILE *arquivo = fopen("blockchain.bin", "rb");
    if (!arquivo) return;

    for(int i=0; i<QTD_ENDERECOS; i++) indiceEnderecos[i].qtdBlocos = 0;

    BlocoMinerado bm;
    while(fread(&bm, sizeof(BlocoMinerado), 1, arquivo)) {
        unsigned char minerador = bm.bloco.data[TAM_DATA - 1];
        int qtd = indiceEnderecos[minerador].qtdBlocos;
        if(qtd < MAX_BLOCOS_POR_MINERADOR) {
            indiceEnderecos[minerador].listaNumerosBlocos[qtd] = bm.bloco.numero;
            indiceEnderecos[minerador].qtdBlocos++;
        }
    }
    fclose(arquivo);

    fIdx = fopen("indice_enderecos.bin", "wb");
    if(fIdx) { fwrite(indiceEnderecos, sizeof(IndiceMinerador), QTD_ENDERECOS, fIdx); fclose(fIdx); }
    indiceEnderecosCarregado = 1;
}

void carregarIndiceNonce() {
    if(indiceNonceCarregado) return;

    printf("[SISTEMA] Carregando AVL de Nonces na RAM...\n");
    FILE *arquivo = fopen("blockchain.bin", "rb");
    if(!arquivo) return;

    BlocoMinerado bm;
    while(fread(&bm, sizeof(BlocoMinerado), 1, arquivo)) {
        insere((double)bm.bloco.nonce, bm.bloco.numero, &raizIndiceNonce);
    }
    fclose(arquivo);
    indiceNonceCarregado = 1;
}

// --- FUNÇÕES DE RELATÓRIO E CONSULTA ---
void exibirEstatisticasGerais() {
    FILE *arquivo = fopen("blockchain.bin", "rb");
    if (!arquivo) { printf("\n[ERRO] 'blockchain.bin' nao encontrado!\n"); return; }
    
    // Alocação eficiente usando calloc (zera a memória)
    unsigned int *carteira = (unsigned int *)calloc(QTD_ENDERECOS, sizeof(unsigned int));
    unsigned int *blocosMinerados = (unsigned int *)calloc(QTD_ENDERECOS, sizeof(unsigned int));
    
    int maxTransacoes = -1, minTransacoes = INT_MAX;
    unsigned long long totalBitcoinsTransacionados = 0; // unsigned long long para evitar overflow
    BlocoMinerado bm;

    printf("\n[ANALISE] Lendo blockchain para gerar estatisticas...\n");
    
    // --- PASSADA 1: Calcular saldos, contagens e identificar limites ---
    while(fread(&bm, sizeof(BlocoMinerado), 1, arquivo)) {
        unsigned char minerador = bm.bloco.data[TAM_DATA - 1];
        carteira[minerador] += RECOMPENSA;
        blocosMinerados[minerador]++;

        int qtd = contarTransacoes(&bm);
        
        // Atualiza saldos com base nas transações
        if (bm.bloco.numero > 1) {
            for (int i = 0; i < TAM_DATA - 1; i += 3) {
                unsigned char o = bm.bloco.data[i];     
                unsigned char d = bm.bloco.data[i+1];   
                unsigned char v = bm.bloco.data[i+2];   
                
                // Transação válida (não nula)
                if (o == 0 && d == 0 && v == 0) continue;
                
                if (carteira[o] >= v) {
                    carteira[o] -= v; 
                    carteira[d] += v; 
                    totalBitcoinsTransacionados += v;
                }
            }
        }
        
        if (qtd > maxTransacoes) maxTransacoes = qtd;
        if (qtd < minTransacoes) minTransacoes = qtd;
    }

    // --- CÁLCULO DOS RECORDES (COM EMPATE) ---
    unsigned int maiorSaldo = 0;
    unsigned int recordeMineracao = 0;

    // 1. Achar os valores máximos
    for(int i=0; i<QTD_ENDERECOS; i++) {
        if(carteira[i] > maiorSaldo) maiorSaldo = carteira[i];
        if(blocosMinerados[i] > recordeMineracao) recordeMineracao = blocosMinerados[i];
    }

    // --- IMPRESSÃO FORMATADA PARA O RELATÓRIO PDF ---
    printf("\n=======================================================\n");
    printf("         DADOS PARA O RELATORIO\n");
    printf("=======================================================\n");

    printf("\nEndereco(s) com mais bitcoins (Total: %u BTC):\n", maiorSaldo);
    for(int i=0; i<QTD_ENDERECOS; i++) {
        if(carteira[i] == maiorSaldo) printf("    - Endereco ID: %d\n", i);
    }

    printf("\nEndereco(s) que minerou mais blocos (Total: %u blocos):\n", recordeMineracao);
    for(int i=0; i<QTD_ENDERECOS; i++) {
        if(blocosMinerados[i] == recordeMineracao) printf("    - Endereco ID: %d\n", i);
    }

    printf("\nBloco(s) com MAIS transacoes (Total: %d transacoes):\n", maxTransacoes);
    rewind(arquivo); // Volta para o inicio do arquivo
    while(fread(&bm, sizeof(BlocoMinerado), 1, arquivo)) {
        if (contarTransacoes(&bm) == maxTransacoes) {
            printf("    - Bloco %d | Hash: ", bm.bloco.numero);
            printHash(bm.hash);
            printf("\n");
        }
    }

    printf("\nBloco(s) com MENOS transacoes (Total: %d transacoes):\n", minTransacoes);
    rewind(arquivo); // Volta para o inicio do arquivo
    while(fread(&bm, sizeof(BlocoMinerado), 1, arquivo)) {
        if (contarTransacoes(&bm) == minTransacoes) {
            printf("    - Bloco %d | Hash: ", bm.bloco.numero);
            printHash(bm.hash);
            printf("\n");
        }
    }

    double media = (double)totalBitcoinsTransacionados / TOTAL_BLOCOS;
    printf("\nQuantidade media de bitcoins transacionados por bloco:\n");
    printf("    - %.2f BTC\n", media);
    
    printf("=======================================================\n");

    free(carteira);
    free(blocosMinerados);
    fclose(arquivo);
}

void buscarBlocoPeloNumero() {
    int num;
    printf("Digite o numero do bloco (1 a %d): ", TOTAL_BLOCOS);
    if(scanf("%d", &num) != 1) return;
    if (num < 1 || num > TOTAL_BLOCOS) { printf("[ERRO] Invalido.\n"); return; }

    FILE *arquivo = fopen("blockchain.bin", "rb");
    if (!arquivo) return;

    long offset = (long)(num - 1) * sizeof(BlocoMinerado);
    fseek(arquivo, offset, SEEK_SET);
    BlocoMinerado bm;
    fread(&bm, sizeof(BlocoMinerado), 1, arquivo);
    fclose(arquivo);

    printf("\n--- BLOCO %d ---\n", num);
    printf("Nonce: %u\n", bm.bloco.nonce);
    printHash(bm.hash); printf("\n");
    imprimirConteudoData(bm.bloco.numero, bm.bloco.data);
}

void listarNPrimeirosOrdenados() {
    int n;
    printf("Qtd de blocos para ordenar: ");
    if(scanf("%d", &n) != 1) return;

    FILE *arquivo = fopen("blockchain.bin", "rb");
    if(!arquivo) return;

    BlocoMinerado *vetor = (BlocoMinerado *)malloc(n * sizeof(BlocoMinerado));
    size_t lidos = fread(vetor, sizeof(BlocoMinerado), n, arquivo);
    fclose(arquivo);

    if(lidos < n) n = lidos;
    qsort(vetor, n, sizeof(BlocoMinerado), compararBlocos);

    printf("\n--- TOP %d (Ordenado por Transacoes) ---\n", n);
    for(int i=0; i<n; i++) {
        printf("Bloco %d: %d transacoes\n", vetor[i].bloco.numero, contarTransacoes(&vetor[i]));
    }
    free(vetor);
}

void consultarPorEndereco() {
    carregarIndiceEnderecos();
    int id;
    printf("ID do Endereco (0-255): ");
    if(scanf("%d", &id) != 1) return;
    if(id < 0 || id >= QTD_ENDERECOS) return;

    int qtd = indiceEnderecos[id].qtdBlocos;
    printf("Minerou %d blocos.\n", qtd);
    if(qtd == 0) return;

    int n;
    printf("Ver quantos? ");
    scanf("%d", &n);
    if(n > qtd) n = qtd;

    FILE *arquivo = fopen("blockchain.bin", "rb");
    BlocoMinerado bm;
    for(int i=0; i<n; i++) {
        int num = indiceEnderecos[id].listaNumerosBlocos[i];
        fseek(arquivo, (long)(num-1)*sizeof(BlocoMinerado), SEEK_SET);
        fread(&bm, sizeof(BlocoMinerado), 1, arquivo);
        printf(" -> Bloco %d (Nonce: %u)\n", num, bm.bloco.nonce);
    }
    fclose(arquivo);
}

void consultarPorNonce() {
    carregarIndiceNonce();
    double nonce;
    printf("Digite o Nonce: ");
    if(scanf("%lf", &nonce) != 1) return;

    int num = buscarNaAVL(raizIndiceNonce, nonce);
    if(num == -1) printf("Nao encontrado.\n");
    else {
        printf("Encontrado no Bloco %d\n", num);
        // Exibir detalhes
        FILE *arquivo = fopen("blockchain.bin", "rb");
        fseek(arquivo, (long)(num-1)*sizeof(BlocoMinerado), SEEK_SET);
        BlocoMinerado bm;
        fread(&bm, sizeof(BlocoMinerado), 1, arquivo);
        fclose(arquivo);
        imprimirConteudoData(bm.bloco.numero, bm.bloco.data);
    }
}

// --- MENU PRINCIPAL DA ANÁLISE ---
void exibirMenuAnalise() {
    int opcao;
    do {
        printf("\n==== MENU ANALISE ====\n");
        printf("1. Estatisticas Gerais\n");
        printf("2. Consultar Bloco (ID)\n");
        printf("3. Ordenar N Primeiros\n");
        printf("4. Consultar por Endereco\n");
        printf("5. Consultar por Nonce (AVL)\n");
        printf("0. Voltar\n");
        printf("Escolha: ");
        if(scanf("%d", &opcao)!=1) { while(getchar()!='\n'); opcao=-1; }

        switch(opcao) {
            case 1: exibirEstatisticasGerais(); break;
            case 2: buscarBlocoPeloNumero(); break;
            case 3: listarNPrimeirosOrdenados(); break;
            case 4: consultarPorEndereco(); break;
            case 5: consultarPorNonce(); break;
            case 0: break;
            default: printf("Opcao invalida.\n");
        }
    } while(opcao != 0);
}