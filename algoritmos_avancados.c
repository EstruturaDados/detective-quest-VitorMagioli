#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//--------------------------DEFINIÇÕES GLOBAIS-----------------------
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
    char pista[512];
    struct No *esquerda;
    struct No *direita;
} No;

// --- ESTRUTURA DAS PISTAS (Árvore Binária de BUSCA - BST) ---
typedef struct NoPista{
    char pista[512];
    struct NoPista *esquerda;
    struct NoPista *direita;
} NoPista;

// --- ESTRUTURAS DA TABELA HASH (Suspeitos) ---
typedef struct ItemHash {
    char pista[512];
    char suspeito[30];
    struct ItemHash *proximo;
} ItemHash;

ItemHash *tabelaHash[TAMANHO_TABELA]; // Tabela Hash Global

//--------------------------PROTÓTIPO DAS FUNÇÕES-----------------------
// Funções da Mansão
struct No* criarSala(char *valor, char *pista);
struct No* explorarSalas(struct No *raiz, char escolha, char *historico[], int *numvisitas, NoPista **raizPistas);
void liberarMemoria(struct No *raiz);
void pausa();

// Funções da Árvore de Pistas (BST)
NoPista* criarNoPista(char *pista);
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
void contarPistas(NoPista *raiz, int contagemPistas[]);


// Variáveis Globais de Suspeitos
const char *NOMES_SUSPEITOS[MAX_SUSPEITOS] = {
    "Sofia Mendes (Governanta)", // O Culpado
    "Chefe Marcelo (Cozinheiro)", 
    "Sra. Elara (Hóspede)",
    "Sr. Arthur (Jardineiro)"
};

int main() {

    // Inicializa a Tabela Hash
    inicializarHash();

    // === DEFINIÇÃO DAS ASSOCIAÇÕES PISTA-SUSPEITO ===
    // As pistas cruciais (6 ao todo) apontam para a Governanta
    inserirNaHash("O cofre principal, ao lado da escadaria, está arrombado. No chão, você vê algumas folhas secas e um pouco de terra, como se tivessem caído de um sapato. As pegadas de terra parecem ir para a direita (Cozinha), mas as folhas secas parecem ter sido sopradas para a esquerda (Sala de Estar).", "Sofia Mendes (Governanta)");
    inserirNaHash("Na penteadeira, você vê uma pequena caixa de joias, vazia, com o forro de veludo arranhado. Ao lado, um grampo de cabelo de metal, torto.", "Sofia Mendes (Governanta)");
    inserirNaHash("No lixo, você encontra um pedaço de pano manchado de graxa preta. Cheira a óleo de máquina.", "Sofia Mendes (Governanta)");
    inserirNaHash("A porta do armário de ferramentas no fim do corredor está arrombada, e a madeira está lascada. Parece que algo de metal foi usado para forçá-la.", "Sofia Mendes (Governanta)");
    inserirNaHash("O porão está úmido e cheio de ferramentas de manutenção. Você encontra uma caixa de ferramentas aberta. Nela, um espaço vazio coberto de graxa preta, com o formato de um... Pé de Cabra.", "Sofia Mendes (Governanta)");
    inserirNaHash("O sótão está empoeirado e cheio de móveis velhos. Escondido debaixo de um lençol, você encontra um pequeno baú. Dentro dele... o colar de diamantes roubado! Ao lado, um uniforme de Governanta com manchas de graxa na manga.", "Sofia Mendes (Governanta)");

    // Pistas Falsas (Red Herrings)
    inserirNaHash("Uma sala luxuosa. A janela que dá para o Jardim está entreaberta, e mais folhas secas estão no carpete. Em cima da mesa de centro, uma xícara de chá fria e um livro de romance.", "Sra. Elara (Hóspede)");
    inserirNaHash("Um quarto de hóspedes, arrumado. A cama está feita. Na gaveta, você encontra um diário trancado. Parece pertencer a uma visitante recente.", "Sra. Elara (Hóspede)");
    inserirNaHash("O jardim está bem cuidado. Perto da janela da Sala de Estar, você vê um ancinho e um saco de adubo. As ferramentas de jardinagem estão todas aqui, limpas.", "Sr. Arthur (Jardineiro)");
    inserirNaHash("A cozinha está movimentada. O Chefe reclama que o jantar será atrasado. Perto da porta de serviço (que leva ao Quarto 2), você vê um par de botas de trabalho sujas de terra, grandes demais para uma mulher.", "Chefe Marcelo (Cozinheiro)");

    printf("=============================================\n");
    printf("============JOGO DETECTIVE QUEST=============\n");
    printf("=============================================\n");
    printf("\nBem-vindo à Mansão! Você pode explorar usando 'e' (esquerda), 'd' (direita) ou 's' (sair).\n");
        
    // === CRIAÇÃO E MONTAGEM DA ÁRVORE (MANSÃO) ===
    // As strings de pista nas salas DEVEM ser idênticas às strings na Tabela Hash.
    struct No* raiz = criarSala("Hall de Entrada", "O cofre principal, ao lado da escadaria, está arrombado. No chão, você vê algumas folhas secas e um pouco de terra, como se tivessem caído de um sapato. As pegadas de terra parecem ir para a direita (Cozinha), mas as folhas secas parecem ter sido sopradas para a esquerda (Sala de Estar).");
    raiz->esquerda = criarSala("Sala de Estar", "Uma sala luxuosa. A janela que dá para o Jardim está entreaberta, e mais folhas secas estão no carpete. Em cima da mesa de centro, uma xícara de chá fria e um livro de romance.");
    raiz->direita = criarSala("Cozinha", "A cozinha está movimentada. O Chefe reclama que o jantar será atrasado. Perto da porta de serviço (que leva ao Quarto 2), você vê um par de botas de trabalho sujas de terra, grandes demais para uma mulher.");
    raiz->esquerda->esquerda = criarSala("Quarto 1", "Um quarto de hóspedes, arrumado. A cama está feita. Na gaveta, você encontra um diário trancado. Parece pertencer a uma visitante recente.");
    raiz->esquerda->esquerda->esquerda = criarSala("Jardim", "O jardim está bem cuidado. Perto da janela da Sala de Estar, você vê um ancinho e um saco de adubo. As ferramentas de jardinagem estão todas aqui, limpas.");
    raiz->esquerda->direita = criarSala("Banheiro 1", NULL); // Sem pista
    raiz->direita->esquerda = criarSala("Quarto 2 (Governanta)", "Na penteadeira, você vê uma pequena caixa de joias, vazia, com o forro de veludo arranhado. Ao lado, um grampo de cabelo de metal, torto.");
    raiz->direita->esquerda->direita = criarSala("Sótão", "O sótão está empoeirado e cheio de móveis velhos. Escondido debaixo de um lençol, você encontra um pequeno baú. Dentro dele... o colar de diamantes roubado! Ao lado, um uniforme de Governanta com manchas de graxa na manga.");
    raiz->direita->esquerda->esquerda = criarSala("Banheiro 2", "No lixo, você encontra um pedaço de pano manchado de graxa preta. Cheira a óleo de máquina.");
    raiz->direita->direita = criarSala("Corredor", "A porta do armário de ferramentas no fim do corredor está arrombada, e a madeira está lascada. Parece que algo de metal foi usado para forçá-la.");
    raiz->direita->direita->direita = criarSala("Porão", "O porão está úmido e cheio de ferramentas de manutenção. Você encontra uma caixa de ferramentas aberta. Nela, um espaço vazio coberto de graxa preta, com o formato de um... Pé de Cabra.");


    // === INICIALIZAÇÃO DA ÁRVORE DE PISTAS (BST) ===
    NoPista *raizPista = NULL;

    // === INÍCIO DA EXPLORAÇÃO INTERATIVA ===

    struct No *atual = raiz;
    char escolha = '\0';

    // === CRIAÇÃO DO HISTÓRICO ===
    char* historicoVisitas[MAX_VISITAS]; 
    int numeroVisitas = 0; 

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
    { // Exibe a sala atual e opções
    printf("\n----------------------\n");
    printf("Você está no: %s \n\n", atual->nome);
    printf("O que você deseja fazer:\n");
        // Opções de movimento
    if(atual->esquerda != NULL){
        printf("(e) Para esquerda você vai para %s\n", atual->esquerda->nome);
    } 
    if (atual->direita != NULL) {
        printf("(d) Para direita você vai para %s\n", atual->direita->nome);
    }
    printf("(s) Sair da Mansão\n");
    printf("Escolha: ");
    scanf(" %c", &escolha);
    escolha = tolower(escolha); // Converte para minúscula
    while (getchar() != '\n'); // Limpa o buffer: consome o \n deixado pelo ENTER (e qualquer outro caractere digitado em excesso)

    // Chamada da função explorarSalas (a lógica da hash é feita dentro dela)
    atual = explorarSalas(atual, escolha, historicoVisitas, &numeroVisitas, &raizPista);

    } while (escolha != 's'); 

    // === FASE DE JULGAMENTO FINAL ===
    verificarSuspeitoFinal(raizPista);

    // === EXIBE O HISTÓRICO ===

    printf("\n\n---- HISTÓRICO DE LUGARES VISITADOS ----\n");
    for(int i = 0; i < numeroVisitas; i++){ // Itera sobre o histórico
        printf("%d. %s\n", i + 1, historicoVisitas[i]); // Exibe cada visita
    }
    printf("------------------------------------\n");
    
    // === EXIBE AS PISTAS COLETADAS ===
    printf("\n\n---- PISTAS COLETADAS (EM ORDEM ALFABÉTICA) ----\n");
    if(raizPista == NULL){ // Verifica se alguma pista foi coletada
        printf("Nenhuma Pista Coletada.\n");
    } else{
        exibirPistas(raizPista);  // Exibe as pistas em ordem alfabética
    }

    // === LIBERAÇÃO DE MEMÓRIA ===
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
    getchar();
}

/**
 * @brief Cria dinamicamente um cômodo (Nó) da mansão.
 */
struct No* criarSala(char *valor, char *pista){
    struct No* novo = (struct No*) malloc(sizeof(struct No)); // Aloca memória para o novo nó

    if (novo == NULL) { // Verifica se a alocação foi bem-sucedida
        printf("Erro de alocacao de memoria para criarSala!\n");
        exit(1); // Sai do programa em caso de erro
    }

    strcpy(novo->nome, valor); // Copia o nome do cômodo
    if (pista != NULL){ // Verifica se a pista não é NULL
        strcpy(novo->pista, pista);
    } else {
        strcpy(novo->pista, ""); //Se a pista for NULL, define como string vazia
    }
    // Inicializa os ponteiros dos filhos como NULL
    novo->esquerda = NULL;
    novo->direita = NULL;

    return novo; // Retorna o novo nó criado
}

/**
 * @brief Navega pela árvore (mansão) e ativa o sistema de pistas.
 */
struct No* explorarSalas(struct No *raiz, char escolha, char *historico[], int *numvisitas, NoPista** raizPistas){ 
    struct No *proximoNo = raiz; // Nó temporário para checagem

    if (escolha == 'e'){ // Movimento para a esquerda
        if(raiz->esquerda != NULL){ 
            proximoNo = raiz->esquerda; 
        }
        else{ 
            printf("[CAMINHO BLOQUEADO] Não há nada à esquerda.\n");
            pausa();
            return raiz; // Retorna o nó atual (não se moveu)
        }
    }
    else if(escolha == 'd'){ // Movimento para a direita
        if(raiz->direita != NULL){
            proximoNo = raiz->direita; // Atualiza o nó temporário
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
        historico[*numvisitas] = raiz->nome; // Adiciona o nome da sala ao histórico
        (*numvisitas)++; // Incrementa o número de visitas
    }

    //4. Checa se há pista e a adiciona na ÁRVORE BST
    if(raiz->pista[0] != '\0'){ 
        printf("PISTA ENCONTRADA: %s\n", raiz->pista);

        // Insere a pista na BST.
        *raizPistas = inserirPista(*raizPistas, raiz->pista);

    }
    printf("------------------------------------------------");
    pausa();
    return raiz; // Retorna o NOVO nó atual
}


/**
 * @brief Libera a memória alocada para os ponteiros das estruturas da Árvore da Mansão (Pós-Ordem).
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
    NoPista* novo = (NoPista*) malloc(sizeof(NoPista)); // Aloca memória para o novo nó
    if (novo == NULL) { // Verifica se a alocação foi bem-sucedida
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
 */
int funcaoHash(const char *chave) { // Pista como chave
    unsigned long hash = 0; // Usamos unsigned long para evitar overflow
    for (int i = 0; i < strlen(chave); i++) {  // Percorre cada caractere da chave
        hash = hash + chave[i]; // Soma o valor ASCII do caractere ao hash
    }
    return hash % TAMANHO_TABELA; // Retorna o índice dentro do tamanho da tabela
}

/**
 * @brief Insere associação pista/suspeito na tabela hash.
 */
void inserirNaHash(const char *pista, const char *suspeito) {
    int indice = funcaoHash(pista); // Calcula o índice usando a função hash

    // Cria um novo item
    ItemHash *novoItem = (ItemHash*) malloc(sizeof(ItemHash)); // Aloca memória para o novo item
    if (novoItem == NULL) { // Verifica se a alocação foi bem-sucedida
        printf("Erro de alocacao de memoria para inserirNaHash!\n");
        return;
    }

    // Copia os dados
    strncpy(novoItem->pista, pista, sizeof(novoItem->pista) - 1); // Usa strncpy para evitar overflow
    novoItem->pista[sizeof(novoItem->pista) - 1] = '\0'; // Garante terminação nula
    strncpy(novoItem->suspeito, suspeito, sizeof(novoItem->suspeito) - 1);
    novoItem->suspeito[sizeof(novoItem->suspeito) - 1] = '\0';

    // Insere no início da lista encadeada (tratamento de colisão)
    novoItem->proximo = tabelaHash[indice]; // Aponta para o item atual na posição
    tabelaHash[indice] = novoItem; // Atualiza a cabeça da lista para o novo item
}

/**
 * @brief Consulta o suspeito correspondente a uma pista.
 */
const char* encontrarSuspeito(const char *pista) {
    int indice = funcaoHash(pista); // Calcula o índice usando a função hash
    ItemHash *atual = tabelaHash[indice]; // Ponteiro para percorrer a lista encadeada

    // Percorre a lista encadeada no índice
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) { // Pista encontrada
            return atual->suspeito; // Encontrado! Retorna o nome do suspeito
        }
        atual = atual->proximo; // Avança para o próximo item na lista
    }
    return NULL; // Não encontrado
}

/**
 * @brief Inicializa a tabela hash, definindo todos os ponteiros como NULL.
 */
void inicializarHash() {
    for (int i = 0; i < TAMANHO_TABELA; i++) { // Itera sobre cada posição da tabela
        tabelaHash[i] = NULL; // Inicializa como NULL
    }
}

/**
 * @brief Libera a memória alocada para a Tabela Hash.
 */
void liberarHash() {
    for (int i = 0; i < TAMANHO_TABELA; i++) { // Itera sobre cada posição da tabela
        ItemHash *atual = tabelaHash[i]; // Ponteiro para percorrer a lista encadeada
        while (atual != NULL) {
            ItemHash *temp = atual; // Armazena o item atual
            atual = atual->proximo; // Avança para o próximo item
            free(temp); // Libera a memória do item armazenado
        }
        tabelaHash[i] = NULL; // Define a posição como NULL após liberar
    }
}

// --- FUNÇÕES DE JULGAMENTO FINAL ---

/**
 * @brief Percorre a BST de pistas e incrementa o contador do suspeito associado.
 * @param raiz A raiz da BST de pistas.
 * @param contagemPistas O array a ser atualizado com a contagem de pistas.
 * (Função movida e corrigida para aceitar o array)
 */
void contarPistas(NoPista *raiz, int contagemPistas[]) {
    if (raiz == NULL) return; // Caso base: árvore vazia 

    // Percurso Em-Ordem 
    contarPistas(raiz->esquerda, contagemPistas);

    const char *suspeitoPista = encontrarSuspeito(raiz->pista); // Consulta o suspeito associado à pista
    if (suspeitoPista != NULL) {
        // Relaciona a pista coletada ao suspeito e incrementa o contador
        for (int i = 0; i < MAX_SUSPEITOS; i++) {  
            if (strcmp(suspeitoPista, NOMES_SUSPEITOS[i]) == 0) { // Suspeito encontrado
                contagemPistas[i]++; // Incrementa o contador do suspeito
                break;
            }
        }
    }

    contarPistas(raiz->direita, contagemPistas); // Continua o percurso
}

/**
 * @brief Conduz à fase de julgamento final, solicita a acusação e verifica a evidência.
 * @param raizPistas A raiz da BST de pistas coletadas.
 * Verifica se o jogador tem evidência suficiente para acusar um suspeito.
 * Uma acusação bem-sucedida requer pelo menos 2 pistas que apontem para o suspeito acusado.
 * O culpado real, segundo a nova história, é a Sofia Mendes (Governanta).
 */
void verificarSuspeitoFinal(NoPista *raizPistas) {
    char acusacao[30]; // Armazena o nome do suspeito acusado
    int contagemPistas[MAX_SUSPEITOS] = {0}; // Inicializa o array de contagem de pistas
    int escolhaSuspeito = -1; // Inicializa a escolha do suspeito

    printf("\n\n************************************\n");
    printf("****** FASE DE ACUSAÇÃO FINAL ******\n");
    printf("************************************\n");

    if (raizPistas == NULL) { // Verifica se alguma pista foi coletada
        printf("Você não coletou nenhuma pista. Acusação indisponível.\n");
        return;
    }
    // 1. CONTAGEM DAS PISTAS COLETADAS
    contarPistas(raizPistas, contagemPistas); // Chamada corrigida

    printf("\n** Pistas Coletadas por Suspeito **\n");
    for (int i = 0; i < MAX_SUSPEITOS; i++) { // Exibe a contagem de pistas para cada suspeito
        printf("%d. %s: %d Pista(s)\n", i + 1, NOMES_SUSPEITOS[i], contagemPistas[i]);
    }

    // 2. SOLICITAÇÃO DA ACUSAÇÃO
    do {
        printf("\nAcuse o culpado (digite o número do suspeito 1 a %d): ", MAX_SUSPEITOS); 
        // se a entrada for inválida, ou seja, não for um número entre 1 e MAX_SUSPEITOS, pede novamente
        if (scanf("%d", &escolhaSuspeito) != 1 || escolhaSuspeito < 1 || escolhaSuspeito > MAX_SUSPEITOS) { 
            printf("Opção inválida. Tente novamente.\n");
            // Limpa o buffer de entrada
            while (getchar() != '\n');
            escolhaSuspeito = -1; 
        }
        // Consome a nova linha após o número
        while (getchar() != '\n'); 
    } while (escolhaSuspeito == -1); // Repete até uma escolha válida

    // Obtém o nome do suspeito acusado
    strcpy(acusacao, NOMES_SUSPEITOS[escolhaSuspeito - 1]);

    // 3. VERIFICAÇÃO FINAL
    int indiceAcusado = escolhaSuspeito - 1;

    printf("\n------------------------------------\n");
    printf("ACUSADO: %s\n", acusacao);

    // O Culpado REAL, segundo a nova história, é a Sofia Mendes (Governanta).
    const char *CULPADO_REAL = "Sofia Mendes (Governanta)"; 

    if (contagemPistas[indiceAcusado] >= 2) { // Verifica se há evidência suficiente
        printf("EVIDÊNCIA SUFICIENTE: Você possui %d pista(s) que apontam para %s.\n", contagemPistas[indiceAcusado], acusacao);
        
        if (strcmp(acusacao, CULPADO_REAL) == 0) { // Acusação correta
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
    pausa();
}