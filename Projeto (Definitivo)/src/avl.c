#include <stdlib.h>
#include <stdio.h>
#include "avl.h"

No *criarNo(double nounce, unsigned int bloco) {
    No *p = (No *)malloc(sizeof(No));
    if(p) {
        p->dir = NULL; p->esq = NULL;
        p->bloco = bloco; p->nounce = nounce;
        p->fc = 0;
    }
    return p;
}

void rotDir(No **p) {
    No *aux2 = *p;
    *p = (*p)->esq;
    aux2->esq = (*p)->dir;
    (*p)->dir = aux2;
    aux2->fc = 0; (*p)->fc = 0;
}

void rotEsq(No **p) {
    No *aux2 = *p;
    *p = (*p)->dir;
    aux2->dir = (*p)->esq;
    (*p)->esq = aux2;
    aux2->fc = 0; (*p)->fc = 0;
}

int insere(double nounce, unsigned int bloco, No **p) {
    int aux;
    if(!(*p)) { *p = criarNo(nounce, bloco); return 1; }
    
    if((*p)->nounce > nounce) {
        aux = insere(nounce, bloco, &((*p)->esq));
        if(aux == 1) {
            (*p)->fc--;
            if((*p)->fc == -2) {
                if(((*p)->esq)->fc <= 0) rotDir(p);
                else {
                    No *neto = ((*p)->esq)->dir;
                    if (neto->fc == -1) { ((*p)->esq)->fc = 0; (*p)->fc = 1; }
                    else if (neto->fc == 1) { ((*p)->esq)->fc = -1; (*p)->fc = 0; }
                    else { ((*p)->esq)->fc = 0; (*p)->fc = 0; }
                    neto->fc = 0;
                    rotEsq(&((*p)->esq)); rotDir(p);
                }
            }
            if((*p)->fc == -1) return 1; else return 0;
        }
    }
    else if((*p)->nounce < nounce) {
        aux = insere(nounce, bloco, &((*p)->dir));
        if(aux == 1) {
            (*p)->fc++;
            if((*p)->fc == 2) {
                if(((*p)->dir)->fc >= 0) rotEsq(p);
                else {
                    No *neto = ((*p)->dir)->esq;
                    if (neto->fc == 1) { ((*p)->dir)->fc = 0; (*p)->fc = -1; }
                    else if (neto->fc == -1) { ((*p)->dir)->fc = -1; (*p)->fc = 0; }
                    else { ((*p)->dir)->fc = 0; (*p)->fc = 0; }
                    neto->fc = 0;
                    rotDir(&((*p)->dir)); rotEsq(p);
                }
            }
            if((*p)->fc == 1) return 1; else return 0;
        }
    }
    return 0;
}

// --- FUNÇÃO ADICIONADA PARA O PROJETO FUNCIONAR ---
int buscarNaAVL(No *raiz, double nonceAlvo) {
    if(!raiz) return -1;
    
    // Comparação direta de double 
    if(raiz->nounce == nonceAlvo) return raiz->bloco;
    
    if(nonceAlvo < raiz->nounce) 
        return buscarNaAVL(raiz->esq, nonceAlvo);
    else 
        return buscarNaAVL(raiz->dir, nonceAlvo);
}