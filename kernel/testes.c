#include "testes.h"
#include "memory.h"
#include "uart.h"
#include "task.h"

void heap_estatisticas(){
    uart_print("Heap Total: ");
    uart_print_uint(memory_total());
    uart_print("\n");

    uart_print("Heap Usado: ");
    uart_print_uint(memory_used());
    uart_print("\n");

    uart_print("Heap Livre: ");
    uart_print_uint(memory_free());
    uart_print("\n");

    uart_print("Fragmentacao: ");
    uart_print_uint(memory_fragmentation_percent());
    uart_print("%\n");
}

void testes_memoria(){
    uart_print("========================================\n");
    mapa_heap();

    uart_print("\n----------------------------------------\n");
    uart_print("[TESTE 1] ALOCANDO BLOCOS INICIAIS\n");
    uart_print("-> Pedido: ptr1(1000), ptr2(500), ptr3(2000)\n");
    uart_print("----------------------------------------\n");
    void *ptr1 = kmalloc(1000);
    void *ptr2 = kmalloc(500);
    void *ptr3 = kmalloc(2000);
    
    mapa_heap();

    uart_print("\n----------------------------------------\n");
    uart_print("[TESTE 2] CRIANDO O BURACO\n");
    uart_print("-> Acao: Liberando ptr2 (504 bytes livres)\n");
    uart_print("----------------------------------------\n");
    kfree(ptr2);
    
    mapa_heap();

    uart_print("\n----------------------------------------\n");
    uart_print("[TESTE 3] IGNORAR BURACO CRIADO\n");
    uart_print("-> Acao: Alocando ptr4 (1000 bytes)\n");
    uart_print("----------------------------------------\n");
    void *ptr4 = kmalloc(1000);
    
    mapa_heap();

    uart_print("\n----------------------------------------\n");
    uart_print("[TESTE 4] REUTILIZACAO DO BLOCO\n");
    uart_print("-> Acao: Alocando ptr5 (500 bytes)\n");
    uart_print("----------------------------------------\n");
    void *ptr5 = kmalloc(500);
    
    mapa_heap();

    uart_print("\n----------------------------------------\n");
    uart_print("[TESTE 5] COALESCENCIA\n");
    uart_print("-> Acao: Liberando ponteiros 1 e 5\n");
    uart_print("----------------------------------------\n");
    kfree(ptr1);
    kfree(ptr5); 
    mapa_heap();

    uart_print("\n----------------------------------------\n");
    uart_print("[TESTE 6] COALESCENCIA FINAL\n");
    uart_print("-> Acao: Liberando ponteiros 1 e 5\n");
    uart_print("----------------------------------------\n");
    kfree(ptr3);
    kfree(ptr4);
    mapa_heap();
    
    uart_print("========================================\n\n");
}