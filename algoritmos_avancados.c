#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_VISITAS 100

/**
 * @brief Estrutura do No raiz
 * Nome - recebe um nome de até 30 caracteres
 * No direito e esquerdo - ramificação do nó Pai
 */
typedef struct No {
    char nome [30];
    struct No *esquerda;
    struct No *direita;
} No;

//--------------------------PROTÓTIPO DAS FUNÇÕES-----------------------
struct No* criarSala(char *valor);
struct No* explorarSalas(struct No *raiz, char escolha, char *historico[], int *numvisitas);
void liberarMemoria(struct No *raiz);
void pausa();

int main() {

    // === CRIAÇÃO E MONTAGEM DA ÁRVORE ===

struct No* raiz = criarSala("Hall de Entrada"); // Cria o nó raiz da árvore
raiz->esquerda = criarSala("Sala de Estar");
raiz->direita = criarSala("Cozinha");
raiz->esquerda->esquerda = criarSala("Quarto 1");
raiz->esquerda->esquerda->esquerda = criarSala("Jardim");
raiz->esquerda->direita = criarSala("Banheiro 1");
raiz->direita->esquerda = criarSala("Quarto 2");
raiz->direita->esquerda->direita = criarSala("Sótão");
raiz->direita->esquerda->esquerda = criarSala("Banheiro 2");
raiz->direita->direita = criarSala("Corredor");
raiz->direita->direita->direita = criarSala("Porão");

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

do
{
printf("=============================================\n");
printf("============JOGO DETECTIVE QUEST=============\n");
printf("=============================================\n");
printf("\nBem-vindo à Mansão! Você pode explorar usando 'e' (esquerda), 'd' (direita) ou 's' (sair).\n");
printf("Você está no: %s \n", atual->nome);
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

atual = explorarSalas(atual, escolha, historicoVisitas, &numeroVisitas);

} while (escolha != 's');

// === EXIBE O HISTÓRICO ===

printf("\n\n---- HISTÓRICO DE SALAS VISITADAS ----\n");
for(int i = 0; i < numeroVisitas; i++){
    printf("%d. %s\n", i + 1, historicoVisitas[i]);
}
printf("------------------------------------\n");
// --- FIM DA EXIBIÇÃO ---

printf("Limpando a memória da mansão...\n");
liberarMemoria(raiz);

    return 0;
}


//--------------------------INÍCIO DAS FUNÇÕES--------------------------
void pausa() {
    printf("\nPressione [ENTER] para continuar...");
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
struct No* criarSala(char *valor){
    struct No* novo = (struct No*) malloc(sizeof(struct No));

    strcpy(novo->nome, valor);
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
 * @return O ponteiro para o NOVO nó.
 */
struct No* explorarSalas(struct No *raiz, char escolha, char *historico[], int *numvisitas){ // Recebe 'char', retorna 'struct No*'
    if (escolha == 'e'){
        if(raiz->esquerda != NULL){ //move o ponteiro
            raiz = raiz->esquerda;
            printf("Você entrou no(a) %s\n", raiz->nome);

            if(*numvisitas < MAX_VISITAS){
            historico[*numvisitas] = raiz->nome;
            (*numvisitas)++;
            }
        }
        else{
        printf("[CAMINHO BLOQUEADO] Não há nada à esquerda.\n");
        }
    }
    else if(escolha == 'd'){
        if(raiz->direita != NULL){
            raiz = raiz->direita;
            printf("Você entrou no(a) %s\n", raiz->nome);

            if (*numvisitas < MAX_VISITAS) {
            historico[*numvisitas] = raiz->nome;
            (*numvisitas)++;
            }
        } else{
            printf("[CAMINHO BLOQUEADO] Não há nada à direita.\n");
        }
    }
    else if(escolha == 's'){
        printf("\nAté logo! Saindo da mansão... Volte Sempre!\n");
    }
    else{
        printf("[OPÇÃO INVÁLIDA] Tente 'e', 'd' ou 's'.\n");
    }
    return raiz;
}


/**
 * @brief Libera a memória alocada para os ponteiros das estruturas
 * @param Estrutura raiz criada para armazenar a árvore
 * usa a função free
 * usa a recursividade para liberar primeiro dos galhos da esquerda e depois da direita
 * após isso, libera o nó principal
 */
void liberarMemoria(struct No *raiz){
    if(raiz != NULL){
        liberarMemoria(raiz->esquerda);
        liberarMemoria(raiz->direita);
        free(raiz);
    }
}






//-------------------------------------------------------
// Desafio Detective Quest
// Tema 4 - Árvores e Tabela Hash
// Este código inicial serve como base para o desenvolvimento das estruturas de navegação, pistas e suspeitos.
// Use as instruções de cada região para desenvolver o sistema completo com árvore binária, árvore de busca e tabela hash.
   
// 🌱 Nível Novato: Mapa da Mansão com Árvore Binária
    //
    // - Crie uma struct Sala com nome, e dois ponteiros: esquerda e direita.
    // - Use funções como criarSala(), conectarSalas() e explorarSalas().
    // - A árvore pode ser fixa: Hall de Entrada, Biblioteca, Cozinha, Sótão etc.
    // - O jogador deve poder explorar indo à esquerda (e) ou à direita (d).
    // - Finalize a exploração com uma opção de saída (s).
    // - Exiba o nome da sala a cada movimento.
    // - Use recursão ou laços para caminhar pela árvore.
    // - Nenhuma inserção dinâmica é necessária neste nível.

    // 🔍 Nível Aventureiro: Armazenamento de Pistas com Árvore de Busca
    //
    // - Crie uma struct Pista com campo texto (string).
    // - Crie uma árvore binária de busca (BST) para inserir as pistas coletadas.
    // - Ao visitar salas específicas, adicione pistas automaticamente com inserirBST().
    // - Implemente uma função para exibir as pistas em ordem alfabética (emOrdem()).
    // - Utilize alocação dinâmica e comparação de strings (strcmp) para organizar.
    // - Não precisa remover ou balancear a árvore.
    // - Use funções para modularizar: inserirPista(), listarPistas().
    // - A árvore de pistas deve ser exibida quando o jogador quiser revisar evidências.

    // 🧠 Nível Mestre: Relacionamento de Pistas com Suspeitos via Hash
    //
    // - Crie uma struct Suspeito contendo nome e lista de pistas associadas.
    // - Crie uma tabela hash (ex: array de ponteiros para listas encadeadas).
    // - A chave pode ser o nome do suspeito ou derivada das pistas.
    // - Implemente uma função inserirHash(pista, suspeito) para registrar relações.
    // - Crie uma função para mostrar todos os suspeitos e suas respectivas pistas.
    // - Adicione um contador para saber qual suspeito foi mais citado.
    // - Exiba ao final o “suspeito mais provável” baseado nas pistas coletadas.
    // - Para hashing simples, pode usar soma dos valores ASCII do nome ou primeira letra.
    // - Em caso de colisão, use lista encadeada para tratar.
    // - Modularize com funções como inicializarHash(), buscarSuspeito(), listarAssociacoes().