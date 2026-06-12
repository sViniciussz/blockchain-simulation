#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <stdint.h>
#include <openssl/sha.h>

// --- CONSTANTES GLOBAIS ---
#define SHA256_DIGEST_LENGTH 32
#define TAM_DATA 184
#define TOTAL_BLOCOS 30000 
#define QTD_ENDERECOS 256
#define RECOMPENSA 50
#define TAM_BUFFER 16

// --- STRUCTS (PADRÃO PARA TODO O PROJETO) ---
typedef struct BlocoNaoMinerado {
    unsigned int numero;
    unsigned int nonce;
    unsigned char data[TAM_DATA];
    unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
} BlocoNaoMinerado;

typedef struct BlocoMinerado {
    BlocoNaoMinerado bloco;
    unsigned char hash[SHA256_DIGEST_LENGTH];
} BlocoMinerado;

// --- PROTÓTIPO DO GERADOR ---
void executarGerador();

#endif