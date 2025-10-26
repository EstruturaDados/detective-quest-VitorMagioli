#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_VISITAS 100
#define TAMANHO_TABELA 5 
#define MAX_SUSPEITOS 4

/**
 * @brief Estrutura do Nó (Sala) da Mansão.
 * Nome - nome do cômodo.
 * Pista - pista estática associada ao cômodo.
 * esquerda/direita - ramificações do nó.
 */
typedef struct No {
    char nome [30];
    char pista[256];
    struct No *esquerda;
    struct No *direita;
} No;

// --- ESTRUTURA DAS PISTAS (Árvore Binária de BUSCA - BST) ---
// armazena as pistas em ordem alfabética
typedef struct NoPista{
    char pista[256];
    struct NoPista *esquerda;
    struct NoPista *direita;
} NoPista;

// --- ESTRUTURAS DA TABELA HASH (Suspeitos) ---

/**
 * @brief Estrutura para a Lista Encadeada dentro da Tabela Hash.
 */
typedef struct ItemHash {
    char pista[256];
    char suspeito[30];
    struct ItemHash *proximo;
} ItemHash;

/**
 * @brief A Tabela Hash é um array de ponteiros para ItemHash (Listas Encadeadas).
 */
ItemHash *tabelaHash[TAMANHO_TABELA];

//--------------------------PROTÓTIPO DAS FUNÇÕES-----------------------
// Funções da Mansão
struct No* criarSala(char *valor, char *pista);
struct No* explorarSalas(struct No *raiz, char escolha, char *historico[], int *numvisitas, NoPista **raizPistas);
void liberarMemoria(struct No *raiz);
void pausa();

// Funções da Árvore de Pistas (BST)
NoPista* inserirPista(NoPista* raiz, char* pista);
void exibirPistas(NoPista* raiz);
void liberarPistas(NoPista* raiz);

// Funções da Tabela Hash
int funcaoHash(const char *chave);
void inserirNaHash(const char *pista, const char *suspeito);
const char* encontrarSuspeito(const char *pista);
void inicializarHash();
void liberarHash();

// Funções de Julgamento
void verificarSuspeitoFinal(NoPista *raizPistas);
void contarPistas(NoPista *raiz);


// Variáveis Globais de Suspeitos (Simplificação - Nível Mestre)
const char *NOMES_SUSPEITOS[MAX_SUSPEITOS] = {
    "Helena Varga (Esposa)", 
    "Lucas Varga (Filho)", 
    "Sofia Mendes (Assistente)", 
    "Professor Otto Schmidt (Rival)"
};

int main() {

    // Inicializa a Tabela Hash
    inicializarHash();

    // === DEFINIÇÃO DAS ASSOCIAÇÕES PISTA-SUSPEITO (Nível Mestre) ===
    // Associações de Pistas (Pistas Reais)
    inserirNaHash("O cofre está arrombado. Há terra e folhas secas no chão.", "Lucas Varga (Filho)");
    inserirNaHash("Uma janela para o Jardim está aberta. Há uma xícara de chá fria na mesa.", "Helena Varga (Esposa)");
    inserirNaHash("Um par de botas de trabalho grandes, sujas de terra, está perto da porta.", "Lucas Varga (Filho)");
    inserirNaHash("Na gaveta, um diário trancado.", "Helena Varga (Esposa)");
    inserirNaHash("As ferramentas de jardinagem estão todas limpas e guardadas.", "Professor Otto Schmidt (Rival)");
    inserirNaHash("Uma caixa de joias vazia e um grampo de cabelo torto estão na penteadeira.", "Sofia Mendes (Assistente)");
    inserirNaHash("O COLAR! Está num baú, ao lado de um uniforme com graxa na manga.", "Lucas Varga (Filho)");
    inserirNaHash("No lixo, um pano manchado de graxa preta.", "Lucas Varga (Filho)");
    inserirNaHash("O armário de ferramentas está com a fechadura lascada.", "Lucas Varga (Filho)");
    inserirNaHash("Uma caixa de ferramentas aberta. O espaço de um Pé de Cabra está vazio e sujo de graxa.", "Lucas Varga (Filho)");

    printf("=============================================\n");
    printf("============JOGO DETECTIVE QUEST=============\n");
    printf("=============================================\n");
    printf("\nBem-vindo à Mansão! Você pode explorar usando 'e' (esquerda), 'd' (direita) ou 's' (sair).\n");
        
    // === CRIAÇÃO E MONTAGEM DA ÁRVORE ===
    // Agora passa a pista para cada sala.
    // NULL usado para salas sem pistas.

    struct No* raiz = criarSala("Hall de Entrada", "O cofre está arrombado. Há terra e folhas secas no chão.");
        raiz->esquerda = criarSala("Sala de Estar", "Uma janela para o Jardim está aberta. Há uma xícara de chá fria na mesa.");
        raiz->direita = criarSala("Cozinha", "Um par de botas de trabalho grandes, sujas de terra, está perto da porta.");
        raiz->esquerda->esquerda = criarSala("Quarto 1", "Na gaveta, um diário trancado.");
        raiz->esquerda->esquerda->esquerda = criarSala("Jardim", "As ferramentas de jardinagem estão todas limpas e guardadas.");
        raiz->esquerda->direita = criarSala("Banheiro 1", NULL); // Sem pista
        raiz->direita->esquerda = criarSala("Quarto 2 (Governanta)", "Uma caixa de joias vazia e um grampo de cabelo torto estão na penteadeira.");
        raiz->direita->esquerda->direita = criarSala("Sótão", "O COLAR! Está num baú, ao lado de um uniforme com graxa na manga.");
        raiz->direita->esquerda->esquerda = criarSala("Banheiro 2", "No lixo, um pano manchado de graxa preta.");
        raiz->direita->direita = criarSala("Corredor", "O armário de ferramentas está com a fechadura lascada.");
        raiz->direita->direita->direita = criarSala("Porão", "Uma caixa de ferramentas aberta. O espaço de um Pé de Cabra está vazio e sujo de graxa.");


    // === INICIALIZAÇÃO DA ÁRVORE DE PISTAS (BST) ===
    NoPista *raizPista = NULL;// A árvore de pistas começa VAZIA.

    // === INÍCIO DA EXPLORAÇÃO INTERATIVA ===

    struct No *atual = raiz;
    char escolha = '\0';

    // === CRIAÇÃO DO HISTÓRICO ===
    char* historicoVisitas[MAX_VISITAS]; //array para guardar os nomes
    int numeroVisitas = 0; //contador de visitas

    // Salva a primeira sala (Hall de Entrada) no histórico
    if(numeroVisitas < MAX_VISITAS){
        historicoVisitas[numeroVisitas] = atual->nome;
        numeroVisitas++;
    }
    // Pista inicial no Hall de Entrada
    if(atual->pista[0] != '\0'){
        printf("\nPISTA ENCONTRADA: %s\n", atual->pista);
        raizPista = inserirPista(raizPista, atual->pista);
        pausa();
    }

    do
    {
    printf("Você está no: %s \n\n", atual->nome);
    printf("O que você deseja fazer:\n");

    if(atual->esquerda != NULL){
        printf("(e) Para esquerda você vai para %s\n", atual->esquerda->nome);
    } 
    if (atual->direita != NULL) {
        printf("(d) Para direita você vai para %s\n", atual->direita->nome);
    }
    printf("(s) Sair da Mansão\n");
    printf("Escolha: ");
    scanf(" %c", &escolha);
    escolha = tolower(escolha); //converte a entrada do usuário para minúscula caso digite maiúscula
    getchar();

    // Passamos o ENDEREÇO da raizPistas (&raizPistas)
    // para que a função possa modificá-la
    atual = explorarSalas(atual, escolha, historicoVisitas, &numeroVisitas, &raizPista);

    } while (escolha != 's');

    // === FASE DE JULGAMENTO FINAL ===
    verificarSuspeitoFinal(raizPista);

    // === EXIBE O HISTÓRICO ===

    printf("\n\n---- HISTÓRICO DE LUGARES VISITADOS ----\n");
    for(int i = 0; i < numeroVisitas; i++){
        printf("%d. %s\n", i + 1, historicoVisitas[i]);
    }
    printf("------------------------------------\n");
    // --- FIM DA EXIBIÇÃO ---
    // === EXIBE AS PISTAS COLETADAS (EM ORDEM ALFABÉTICA) ===
    printf("\n\n---- PISTAS COLETADAS (EM ORDEM ALFABÉTICA) ----\n");
    if(raizPista == NULL){
        printf("Nenhuma Pista Coletada.\n");
    } else{
        exibirPistas(raizPista); // Chama a exibição em-ordem
    }


    printf("------------------------------------\n");
    printf("Limpando a memória da mansão...\n");
    liberarMemoria(raiz);
    printf("Limpando a memória das pistas...\n");
    liberarPistas(raizPista);
    printf("Limpando a memória da Tabela Hash...\n");
    liberarHash();
    return 0;
}


//--------------------------INÍCIO DAS FUNÇÕES--------------------------
void pausa() {
    printf("\nPressione [ENTER] para continuar...\n");
    getchar(); // Espera o usuário pressionar Enter
}

/**
 * @brief Cria uma função criarSala, para armazenar os nomes dos comodos do jogo
 * @param Recebe um valor que será armazenado como nome do comodo
 * armazena o ponteiro do endereço de memória dinamicamente pelo comando malloc
 * copia o nome recebido para a variavel nome indicada pelo ponteiro
 * cria e inicializa os ponteiros esquerda e direita
 * retorna o endereço da memória armazenado pela variável novo
 */
struct No* criarSala(char *valor, char *pista){
    struct No* novo = (struct No*) malloc(sizeof(struct No));

    if (novo == NULL) {
        printf("Erro de alocacao de memoria para criarSala!\n");
        exit(1);
    }

    strcpy(novo->nome, valor);
    if (pista != NULL){
        strcpy(novo->pista, pista);
    } else {
        strcpy(novo->pista, ""); //Se a pista for NULL, define como string vazia
    }
    novo->esquerda = NULL;
    novo->direita = NULL;

    return novo;
}

/**
 * @brief Processa a escolha, salva no histórico e retorna o novo local.
 * @param raiz O nó ATUAL.
 * @param escolha A letra do jogador.
 * @param historico O array do histórico (passado por referência).
 * @param numvisitas Um PONTEIRO para o contador de visitas.
 * @param raizPistas Um PONTEIRO PARA O PONTEIRO da raiz da árvore de pistas.
 * @return O ponteiro para o NOVO nó.
 */
struct No* explorarSalas(struct No *raiz, char escolha, char *historico[], int *numvisitas, NoPista** raizPistas){ // Recebe 'char', retorna 'struct No*'
    struct No *proximoNo = raiz; // Nó temporário para checagem

    if (escolha == 'e'){
        if(raiz->esquerda != NULL){ //move o ponteiro
            proximoNo = raiz->esquerda; // Pega o próximo nó
          
            pausa();
        }
        else{
            printf("[CAMINHO BLOQUEADO] Não há nada à esquerda.\n");
            pausa();
            return raiz; // Retorna o nó atual (não se moveu)
        }
    }
    else if(escolha == 'd'){
        if(raiz->direita != NULL){
            proximoNo = raiz->direita;
            printf("Você entrou no(a) %s\n", raiz->nome);

            if (*numvisitas < MAX_VISITAS) {
            historico[*numvisitas] = raiz->nome;
            (*numvisitas)++;
            }
            pausa();
        } else{
            printf("[CAMINHO BLOQUEADO] Não há nada à direita.\n");
            pausa();
            return raiz; // Retorna o nó atual (não se moveu)
        }
    }
    else if(escolha == 's'){
        printf("\nAté logo! Saindo da mansão... Volte Sempre!\n");
        return raiz; // Retorna o nó atual
    }
    else{
        printf("[OPÇÃO INVÁLIDA] Tente 'e', 'd' ou 's'.\n");
        return raiz; // Retorna o nó atual
    }
    
    // Se o movimento foi válido atualiza tudo
    raiz = proximoNo; // 1.Move o Jogador

    //2. Imprime o Local
    printf("\n----------------------\n");
    printf("Você entrou no(a) %s\n", raiz->nome);
    printf("----------------------\n");

    // 3. Adiciona ao histórico de visitas
    if(*numvisitas < MAX_VISITAS){
    historico[*numvisitas] = raiz->nome;
    (*numvisitas)++;
    }

    //4. (NOVO) Checa se há pista e a adiciona na ÁRVORE BST
    if(raiz->pista[0] != '\0'){ // '\0' é o fim de uma string. Se [0] é '\0', a string está vazia.
        printf("PISTA ENCONTRADA: %s\n", raiz->pista);

        // Insere a pista na BST.
        // *raizPistas usada para "referenciar" o ponteiro para ponteiro
        *raizPistas = inserirPista(*raizPistas, raiz->pista);

    }
    printf("------------------------------------------------");
    pausa();
    return raiz; // Retorna o NOVO nó atual
}


/**
 * @brief Libera a memória alocada para os ponteiros das estruturas da Árvore da Mansão
 * @param Estrutura raiz criada para armazenar a árvore
 * usa a função free
 * usa a recursividade para liberar primeiro dos galhos da esquerda e depois da direita
 * após isso, libera o nó principal
 * PÓS-ORDEM
 */
void liberarMemoria(struct No *raiz){
    if(raiz != NULL){
        liberarMemoria(raiz->esquerda);
        liberarMemoria(raiz->direita);
        free(raiz);
    }
}

/**
 * @brief Cria um nó da árvore de Pistas.
 */
NoPista* criarNoPista(char *pista){
    NoPista* novo = (NoPista*) malloc(sizeof(NoPista));
    if (novo == NULL) {
        printf("Erro de alocacao de memoria para criarNoPista!\n");
        exit(1);
    }
    strcpy(novo->pista, pista);
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

/**
 * @brief Insere uma nova pista na BST (em ordem alfabética).
 * Esta é uma função recursiva padrão de inserção em BST.
 */
NoPista* inserirPista(NoPista *raiz, char *pista){
    // 1. Se a árvore está vazia, cria um novo nó.
    if(raiz == NULL){
        return criarNoPista(pista);
    }
    // 2. Compara a nova pista com a pista da raiz
    int comparacao = strcmp(pista, raiz->pista);

    // 3. Passo Recursivo:
    if(comparacao < 0){
        // Se a nova pista é "menor" (vem antes no alfabeto), insere na esquerda.
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    }
    else if(comparacao > 0){
        // Se a nova pista é "maior" (vem depois no alfabeto), insere na direita.
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    // Se (comparacao == 0), a pista já existe, então não faz nada.
    return raiz; // Retorna a raiz (potencialmente atualizada)
}

/**
 * @brief Exibe as pistas da BST em ordem alfabética.
 * Isso é um percurso "em-ordem" (esquerda, raiz, direita).
 */
void exibirPistas(NoPista *raiz){
    if(raiz == NULL){
        return;
    }
    // 1. Visita a sub-árvore esquerda (pistas que vêm antes)
    exibirPistas(raiz->esquerda);

    // 2. Visita a raiz (imprime a pista atual)
    printf("- %s\n", raiz->pista);

    // 3. Visita a sub-árvore direita (pistas que vêm depois)
    exibirPistas(raiz->direita);
}

/**
 * @brief Libera a memória da Árvore de Pistas (Pós-Ordem).
 */
void liberarPistas(NoPista * raiz){
    if(raiz != NULL){
        liberarPistas(raiz->esquerda);
        liberarPistas(raiz->direita);
        free(raiz);
    }
}

// --- FUNÇÕES DA TABELA HASH ---

/**
 * @brief Função de Hashing simples baseada na soma dos valores ASCII.
 * @param chave A string da pista.
 * @return O índice da Tabela Hash (0 a TAMANHO_TABELA - 1).
 */
int funcaoHash(const char *chave) {
    unsigned long hash = 0;
    for (int i = 0; i < strlen(chave); i++) {
        hash = hash + chave[i];
    }
    return hash % TAMANHO_TABELA;
}

/**
 * @brief Insere associação pista/suspeito na tabela hash.
 * @param pista A string da pista (chave).
 * @param suspeito O nome do suspeito (valor).
 */
void inserirNaHash(const char *pista, const char *suspeito) {
    int indice = funcaoHash(pista);

    // Cria um novo item
    ItemHash *novoItem = (ItemHash*) malloc(sizeof(ItemHash));
    if (novoItem == NULL) {
        printf("Erro de alocacao de memoria para inserirNaHash!\n");
        return;
    }

    // Copia os dados
    strncpy(novoItem->pista, pista, sizeof(novoItem->pista) - 1);
    novoItem->pista[sizeof(novoItem->pista) - 1] = '\0';
    strncpy(novoItem->suspeito, suspeito, sizeof(novoItem->suspeito) - 1);
    novoItem->suspeito[sizeof(novoItem->suspeito) - 1] = '\0';

    // Insere no início da lista encadeada (tratamento de colisão)
    novoItem->proximo = tabelaHash[indice];
    tabelaHash[indice] = novoItem;
}

/**
 * @brief Consulta o suspeito correspondente a uma pista.
 * @param pista A string da pista (chave).
 * @return O nome do suspeito (valor) ou NULL se não encontrado.
 */
const char* encontrarSuspeito(const char *pista) {
    int indice = funcaoHash(pista);
    ItemHash *atual = tabelaHash[indice];

    // Percorre a lista encadeada no índice
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito; // Encontrado!
        }
        atual = atual->proximo;
    }
    return NULL; // Não encontrado
}

/**
 * @brief Inicializa a tabela hash, definindo todos os ponteiros como NULL.
 */
void inicializarHash() {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        tabelaHash[i] = NULL;
    }
}

/**
 * @brief Libera a memória alocada para a Tabela Hash.
 */
void liberarHash() {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        ItemHash *atual = tabelaHash[i];
        while (atual != NULL) {
            ItemHash *temp = atual;
            atual = atual->proximo;
            free(temp);
        }
        tabelaHash[i] = NULL;
    }
}

// --- FUNÇÕES DE JULGAMENTO FINAL ---

/**
 * @brief Conduz à fase de julgamento final, solicita a acusação e verifica a evidência.
 * @param raizPistas A raiz da BST contendo as pistas coletadas pelo jogador.
 */
void verificarSuspeitoFinal(NoPista *raizPistas) {
    char acusacao[30];
    int contagemPistas[MAX_SUSPEITOS] = {0};
    int escolhaSuspeito = -1;

    printf("\n\n************************************\n");
    printf("****** FASE DE ACUSAÇÃO FINAL ******\n");
    printf("************************************\n");

    if (raizPistas == NULL) {
        printf("Você não coletou nenhuma pista. Acusação indisponível.\n");
        return;
    }

    contarPistas(raizPistas, contagemPistas);

    printf("\n** Pistas Coletadas por Suspeito **\n");
    for (int i = 0; i < MAX_SUSPEITOS; i++) {
        printf("%d. %s: %d Pista(s)\n", i + 1, NOMES_SUSPEITOS[i], contagemPistas[i]);
    }

    // 2. SOLICITAÇÃO DA ACUSAÇÃO
    do {
        printf("\nAcuse o culpado (digite o número do suspeito 1 a %d): ", MAX_SUSPEITOS);
        if (scanf("%d", &escolhaSuspeito) != 1 || escolhaSuspeito < 1 || escolhaSuspeito > MAX_SUSPEITOS) {
            printf("Opção inválida. Tente novamente.\n");
            // Limpa o buffer de entrada
            while (getchar() != '\n');
            escolhaSuspeito = -1; 
        }
        // Consome a nova linha após o número
        while (getchar() != '\n'); 
    } while (escolhaSuspeito == -1);

    // Obtém o nome do suspeito acusado
    strcpy(acusacao, NOMES_SUSPEITOS[escolhaSuspeito - 1]);

    // 3. VERIFICAÇÃO FINAL
    int indiceAcusado = escolhaSuspeito - 1;

    printf("\n------------------------------------\n");
    printf("ACUSADO: %s\n", acusacao);

    // O Culpado REAL, segundo as pistas inseridas na Hash, é o Lucas Varga (Filho).
    const char *CULPADO_REAL = "Lucas Varga (Filho)"; 

    if (contagemPistas[indiceAcusado] >= 2) {
        printf("EVIDÊNCIA SUFICIENTE: Você possui %d pista(s) que apontam para %s.\n", contagemPistas[indiceAcusado], acusacao);
        
        if (strcmp(acusacao, CULPADO_REAL) == 0) {
            printf("\nPARABÉNS, DETETIVE! VOCÊ ACERTOU!\n");
            printf("Com base nas pistas, a sua acusação de **%s** está correta.\n", CULPADO_REAL);
        } else {
            printf("\nACUSAÇÃO FALHOU!\n");
            printf("Você tinha evidência, mas a pessoa acusada, **%s**, era inocente.\n", acusacao);
            printf("O verdadeiro culpado era: **%s**.\n", CULPADO_REAL);
        }
    } else {
        printf("EVIDÊNCIA INSUFICIENTE: Você só possui %d pista(s) para sustentar a acusação contra %s.\n", contagemPistas[indiceAcusado], acusacao);
        printf("\nACUSAÇÃO FALHOU!\n");
        printf("Você não conseguiu provar o caso. O verdadeiro culpado, **%s**, escapou!\n", CULPADO_REAL);
    }
    printf("------------------------------------\n");
}

/**
 * @brief Percorre a BST de pistas e incrementa o contador do suspeito associado.
 * @param raiz A raiz da BST de pistas.
 * @param contagemPistas O array a ser atualizado com a contagem de pistas.
 */
void contarPistas(NoPista *raiz, int contagemPistas[]) {
    if (raiz == NULL) return;

    // Percurso Em-Ordem 
    contarPistas(raiz->esquerda, contagemPistas);

    const char *suspeitoPista = encontrarSuspeito(raiz->pista);
    if (suspeitoPista != NULL) {
        // Relaciona a pista coletada ao suspeito e incrementa o contador
        for (int i = 0; i < MAX_SUSPEITOS; i++) {
            if (strcmp(suspeitoPista, NOMES_SUSPEITOS[i]) == 0) {
                contagemPistas[i]++;
                break;
            }
        }
    }

    contarPistas(raiz->direita, contagemPistas);
}