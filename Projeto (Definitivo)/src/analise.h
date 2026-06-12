#ifndef ANALISE_H
#define ANALISE_H

#include <stdio.h>
#include "blockchain.h"

void exibirMenuAnalise(); // Função principal do menu de análise

// Funções internas
void exibirEstatisticasGerais();
void buscarBlocoPeloNumero();
void listarNPrimeirosOrdenados();
void consultarPorEndereco();
void consultarPorNonce();
void gerarArquivoTextoCompleto();

#endif