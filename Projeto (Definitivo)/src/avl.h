#ifndef AVL_H
#define AVL_H

// Definição do Nó da AVL
typedef struct No {
    double nounce;        // Mantive sua definição de double
    unsigned int bloco; 
    struct No *esq;
    struct No *dir;
    int fc;               
} No;

// Protótipos das funções
No *criarNo(double nounce, unsigned int bloco);
int insere(double nounce, unsigned int bloco, No **p);
void rotDir(No **p);
void rotEsq(No **p);

// Função de busca necessária para a análise
int buscarNaAVL(No *raiz, double nonceAlvo);

#endif  