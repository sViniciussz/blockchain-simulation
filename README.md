# Simulação de Blockchain - Estrutura de Dados 2

Projeto final desenvolvido para a disciplina de **Estrutura de Dados 2** no curso de Ciência da Computação. O objetivo do projeto é simular o funcionamento de uma rede Blockchain utilizando conceitos avançados de estruturas de dados para indexação e busca eficiente de informações, além de geração pseudoaleatória de dados.

## 👥 Integrantes e Contribuições
* **Arthur Hanczuruk Borba** - Função 1: Sistema de Carteira, Cálculo de Hash e Gerador de Blocos.
* **Felipe de Brito Santos** - Função 2: Indexador (Árvore AVL).
* **Vinicius dos Santos de Souza** - Função 3: Módulo de Análise e Sistema de Menus.

---

## 🛠️ Tecnologias e Algoritmos Utilizados
* **Linguagem C**: Toda a base do projeto foi desenvolvida em C padrão.
* **Mersenne Twister (MT19937)**: Algoritmo robusto utilizado para a geração pseudoaleatória de dados das transações e blocos.
* **Árvore AVL (Balanceada)**: Utilizada como indexador secundário para realizar buscas rápidas de blocos através do campo `Nonce` (O(\log n)).
* **Biblioteca OpenSSL (SHA256)**: Utilizada para garantir a integridade criptográfica dos blocos através do encadeamento de hashes.

---

## 📂 Estrutura do Projeto
```text
├── src/                  # Código-fonte do sistema
│   ├── main.c            # Ponto de entrada e menu principal
│   ├── blockchain.h      # Definição das estruturas globais (BlocoMinerado, BlocoNaoMinerado)
│   ├── gerador.c         # Lógica de mineração de 30.000 blocos
│   ├── analise.h/.c      # Funções de auditoria, estatísticas e busca
│   ├── avl.h/.c          # Implementação e balanceamento da árvore AVL
│   └── mtwister.h/.c     # Gerador Mersenne Twister
├── doc/                  # Documentação e relatórios acadêmicos
│   └── relatorio.pdf     # Relatório impresso com os resultados da análise
└── .gitignore            # Filtros para não subir arquivos binários