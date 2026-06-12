#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <openssl/sha.h>
#include "blockchain.h"
#include "mtwister.h" // Precisa estar na mesma pasta

// Variáveis locais para o gerador
static unsigned int carteira_gen[QTD_ENDERECOS];
static MTRand r;

// Funções auxiliares (locais)
static void inicializarCarteiraGen() {
    memset(carteira_gen, 0, sizeof(carteira_gen));
}

static void calcularHashGen(BlocoNaoMinerado *bloco, unsigned char *hashResult) {
    SHA256((unsigned char*)bloco, sizeof(BlocoNaoMinerado), hashResult);
}

static int hashValidoGen(unsigned char *hash) {
    return (hash[0] == 0); // 00 hexa
}

static unsigned char gerarTransacoesGen(unsigned char *data, int numeroBloco) {
    memset(data, 0, TAM_DATA);
    unsigned char minerador = (unsigned char)(genRandLong(&r) % QTD_ENDERECOS);
    
    if (numeroBloco == 1) {
        const char *genesisMsg = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
        memcpy(data, genesisMsg, strlen(genesisMsg));
        data[TAM_DATA - 1] = minerador;
        return minerador;
    }

    int qtdTransacoes = genRandLong(&r) % 62;
    int pos = 0;
    for (int i = 0; i < qtdTransacoes; i++) {
        int tentativas = 0, origem = -1;
        while (tentativas < 100) {
            int cand = genRandLong(&r) % QTD_ENDERECOS;
            if (carteira_gen[cand] > 0) { origem = cand; break; }
            tentativas++;
        }
        if (origem == -1) break;

        unsigned char destino = (unsigned char)(genRandLong(&r) % QTD_ENDERECOS);
        unsigned char valor = (unsigned char)(genRandLong(&r) % (carteira_gen[origem] + 1));
        
        data[pos++] = (unsigned char)origem;
        data[pos++] = destino;
        data[pos++] = valor;
        
        carteira_gen[origem] -= valor;
        carteira_gen[destino] += valor;
    }
    data[TAM_DATA - 1] = minerador;
    return minerador;
}

// A FUNÇÃO PRINCIPAL DO GERADOR
void executarGerador() {
    printf("\n=== INICIANDO GERADOR DE BLOCKCHAIN ===\n");
    r = seedRand(1234567);
    inicializarCarteiraGen();
    
    FILE *arquivo = fopen("blockchain.bin", "wb");
    if (!arquivo) { perror("Erro ao criar arquivo"); return; }

    BlocoMinerado buffer[TAM_BUFFER];
    int bufferIndex = 0;
    unsigned char hashAnterior[SHA256_DIGEST_LENGTH] = {0};

    printf("Minerando %d blocos...\n", TOTAL_BLOCOS);

    for (int i = 1; i <= TOTAL_BLOCOS; i++) {
        BlocoNaoMinerado b;
        b.numero = i;
        b.nonce = 0;
        memcpy(b.hashAnterior, hashAnterior, SHA256_DIGEST_LENGTH);

        unsigned char minerador = gerarTransacoesGen(b.data, i);

        unsigned char hash[SHA256_DIGEST_LENGTH];
        while(1) {
            calcularHashGen(&b, hash);
            if (hashValidoGen(hash)) break;
            b.nonce++;
        }

        carteira_gen[minerador] += RECOMPENSA;
        memcpy(hashAnterior, hash, SHA256_DIGEST_LENGTH);

        BlocoMinerado bm;
        bm.bloco = b;
        memcpy(bm.hash, hash, SHA256_DIGEST_LENGTH);
        
        buffer[bufferIndex++] = bm;

        if (bufferIndex == TAM_BUFFER) {
            fwrite(buffer, sizeof(BlocoMinerado), TAM_BUFFER, arquivo);
            bufferIndex = 0;
            if (i % 5000 == 0) printf("Minerados: %d/%d\n", i, TOTAL_BLOCOS);
        }
    }
    if (bufferIndex > 0) fwrite(buffer, sizeof(BlocoMinerado), bufferIndex, arquivo);
    
    fclose(arquivo);
    printf("Blockchain gerada com sucesso em 'blockchain.bin'!\n");
}