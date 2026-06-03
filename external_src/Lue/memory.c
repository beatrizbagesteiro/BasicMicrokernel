#include "memory.h"
#include <stddef.h>

#define HEAP_START 0x80400000UL
#define HEAP_SIZE (8 * 1024 * 1024) // 8 MB
#define ALIGN 8 // Alinhamento de 8 bytes

static uint8_t heap_area[HEAP_SIZE];

// Modelo do bloco
typedef struct block{
    uint64_t size; // Área útil - cabeçalho
    int free; // 1 = livre / 0 = ocupado (não posso usar biblioteca de boolean)
    struct block *next; // Próximo bloco na lista
} block_t;

#define BLOCK_HEADER_SIZE (sizeof(block_t)) // Tamanho da própria struct

static uint8_t *heap_base = (uint8_t *)HEAP_START;
static uint8_t *heap_end = (uint8_t *)(HEAP_START + HEAP_SIZE);
static block_t *heap_start = NULL; // início da lista de blocos


void memory_init(void) { // Cria o bloco inicial que reserva todo o heap
    heap_start = (block_t *)heap_area;
    heap_start->size = HEAP_SIZE - BLOCK_HEADER_SIZE;
    heap_start->free = 1;
    heap_start->next = NULL;
}


static block_t *pesquisar (uint64_t r_size){ // Será usado para pesquisar por um bloco livre
    block_t *aux = heap_start;
    while(aux != NULL){
        if(aux->free == 1 && aux->size >= r_size){ // Aloca no primeiro que encontrar e se estiver dentro do tamanho alocado
            return aux;
        }
        aux = aux->next;
    }
    return NULL; // Não achou
}


static uint64_t align_up(uint64_t s){
    return (s + ALIGN - 1) & ~(uint64_t)(ALIGN - 1);
}


// Começará com um blocão gigante e depois vai dividindo ele a cada dividir_bloco, isso torna eficiente.
// Ex:
// Aloquei 1000 bytes
// Quero alugar 150 bytes + cabeçalho (24 bytes, 3 variáveis com sizeof = 8 cada)
// 1000 - 150 - 24 = 826. Se eu não dividir, estarei desperdiçando um heap inteiro ali de 876 bytes, então o que eu faço:
// Divido em um Bloco A de 174 bytes e ele aponta (*next) para o novo bloco após a divisão, o Bloco B com 826 bytes
// Bloco A: size = 150 | free = 0 | next = -> Bloco B
// Bloco B: size = 826 | free = 1 | next = NULL
// E assim toda vez que alocar mais do bloco, divido ele novamente da mesma forma, e isso torna o programa muito eficiente para espaço
// O First-Fit sempre irá procurar onde está esse free = 1, mas o defeito é que ele n permite o programa crescer muito
// Mas agora para estudo está bom e é complexo o suficiente.
// Os bits não são salvos de forma estruturada.
// Só um extra é que será alocado em múltiplos de 8 por estar alinhado com a função align_up.

static void dividir_bloco(block_t *bloco, uint64_t size){
    // Calcula quanto sobra depois de reservar 'size' bytes no bloco atual
    uint64_t espaco_restante = bloco->size - size;

    // Apenas dividirá se o restante couber em: cabeçalho + ALIGN bits úteis. Caso contrário, quem chamou fica com o bloco inteiro
    if (espaco_restante <= BLOCK_HEADER_SIZE + ALIGN)
        return; // Não pode dividir
    
    // Novo bloco, o bloco B citado anteriormente. Posiciona logo após o fim da área útil do bloco A
    // bloco = posição base do bloco A + cabeçalho do bloco + área ocupada do A 
    block_t *novo = (block_t *)((uint8_t *)bloco + BLOCK_HEADER_SIZE + size);

    novo->size = espaco_restante - BLOCK_HEADER_SIZE; // B recebe o espaço restante
    novo->free = 1; // Livre
    novo->next = bloco->next; // Bloco B herda o next que o bloco A tinha antes (N necessariamente é NULL, pois pode estar no meio)
 
    // Bloco A fica com oq foi pedido e aponta para o novo.
    bloco->size = size;
    bloco->next = novo;
}


void *kmalloc(uint64_t size){
    if (size == 0)
        return NULL; // Não posso alocar em espaço 0
 
    size = align_up(size); // Alinhando para tomar espaço de tipos de dados reais, de forma uniforme de 64 bits (8 bytes).
 
    block_t *bloco = pesquisar(size); // Procurando espaço livre linearmente.
    if (bloco == NULL)
        return NULL; // Sem memória

    dividir_bloco(bloco, size);
    bloco->free = 0; // Ocupa o bloco
 
    // Retorna ponteiro para a área útil (logo após o cabeçalho), que seria o início do bloco novo.
    return (void *)((uint8_t *)bloco + BLOCK_HEADER_SIZE);
}


// Estatísticas da memória:
uint64_t memory_total(void){
    return (uint64_t)HEAP_SIZE;
}


uint64_t memory_used(void){
    uint64_t used = 0;
    block_t *aux = heap_start;
    while (aux != NULL){
        if (aux->free == 0)
            used += BLOCK_HEADER_SIZE + aux->size;
        aux = aux->next;
    }
    return used;
}


uint64_t memory_free(void){
    uint64_t free = 0;
    block_t *aux = heap_start;
    while (aux != NULL){
        if (aux->free == 1)
            free += aux->size;
        aux = aux->next;
    }
    return free;
}