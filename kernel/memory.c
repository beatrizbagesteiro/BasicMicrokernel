#include "memory.h"
#include "uart.h"
#include <stddef.h>

#define HEAP_SIZE 0x10000
#define ALIGN 8 // Alinhamento de 8 bytes

// Modelo do bloco
typedef struct block{
    uint64_t size; // Área útil - cabeçalho
    int free; // 1 = livre / 0 = ocupado (não posso usar biblioteca de boolean)
    struct block *next; // Próximo bloco na lista
} block_t;

static uint8_t heap[HEAP_SIZE];
static block_t *free_list = NULL;

#define BLOCK_HEADER_SIZE (sizeof(block_t)) // Tamanho da própria struct

void memory_init(void){
    free_list = (block_t*) heap;
    free_list->size = HEAP_SIZE - sizeof(block_t);
    free_list->free = 1;
    free_list->next = 0;
}

static block_t *pesquisar (uint64_t r_size){ // Será usado para pesquisar por um bloco livre
    block_t *aux = free_list;
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


// Une dois blocos livres vizinhos para evitar fragmentação.
// Isso é feito pois após vários kmallocs e kfrees o heap pode acabar ficando fragmentado e com espaços livres vizinhos.
// Ex:
// [Bloco A: 150B | livre] -> [Bloco B: 300B | livre] -> [Bloco C: 200B | ocupado]
// A e B estão livres e são vizinhos, somados possuem um espaço de 450B
// Mas caso aparecesse um bloco que precisasse de 400B, nenhum dos dois poderia alocar o espaço, pois está dividido em dois
// Porém, ao unir (fazer o coalesce), juntamos os 150B + 24B de cabeçalho do bloco B + 300B, teríamos 474B
// Como resultado [Bloco B: 474B | livre] -> [Bloco C: 200B | ocupado], e agora poderia alocar os anteriores 400B
// B é absorvido pelo A, chamado a cada kfree()
static void coalesce(void){
    block_t *atual = free_list;
    while (atual != NULL && atual->next != NULL){
        // Se o atual e o seu vizinho (next) estiverem livre (free = 1), eu uno os dois
        if (atual->free == 1 && atual->next->free == 1){
            // Absorve o cabeçalho + área útil do próximo
            atual->size += BLOCK_HEADER_SIZE + atual->next->size;

            // Pula o vizinho da lista, já que foi absorvido pelo atual.
            // Mas agora testamos se o vizinho do vizinho também está livre, então testamos novamente na próxima iteração.
            // Isso é importante, como estamos implementando uma lista unicamente encadeada, só temos noção se o próximo da lista está livre ou não, não os passados
            atual->next  = atual->next->next;
        } else{
            atual = atual->next; // Vizinho ocupado, não dá para unir, avança para o próximo
        }
    }
}


void kfree(void *ptr){
    if (ptr == NULL)
        return;
 
    // Recupera o cabeçalho (está imediatamente antes da área útil)
    block_t *bloco = (block_t *)((uint8_t *)ptr - BLOCK_HEADER_SIZE);
    bloco->free = 1;
 
    coalesce(); // Junta blocos vizinhos livres
}


// Estatísticas da memória:
uint64_t memory_total(void){
    return (uint64_t)HEAP_SIZE;
}


uint64_t memory_used(void){
    uint64_t used = 0;
    block_t *aux = free_list;
    while (aux != NULL){
        if (aux->free == 0)
            used += BLOCK_HEADER_SIZE + aux->size;
        aux = aux->next;
    }
    return used;
}

uint64_t memory_free(void){
    uint64_t free = 0;
    block_t *aux = free_list;
    while (aux != NULL){
        if (aux->free == 1)
            free += aux->size;
        aux = aux->next;
    }
    return free;
}

void mapa_heap(void){
    // Pega o primerio endereço da heap
    block_t *bloco_atual = (block_t *)heap;
    // Para descobrir o endereço final, primeiro se converte o endereço para int e depois faz o calculo
    uint64_t endereco_final = (uint64_t)heap + HEAP_SIZE; 

    while ((uint64_t)bloco_atual < endereco_final) {
        
        uart_print("[");
        uart_print_uint((uint64_t)bloco_atual); 
        uart_print("] | Tamanho: ");
        uart_print_uint(bloco_atual->size);
        uart_print(" bytes | Status: ");
        
        if (bloco_atual->free) {
            uart_print("LIVRE\n");
        } else {
            uart_print("OCUPADO\n");
        }

        uint64_t proximo_endereco = (uint64_t)bloco_atual + BLOCK_HEADER_SIZE + bloco_atual->size;
        
        bloco_atual = (block_t *)proximo_endereco;
    }

}