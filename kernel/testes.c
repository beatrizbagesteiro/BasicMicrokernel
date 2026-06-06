#include "testes.h"
#include "memory.h"
#include "uart.h"
#include "task.h"

static void heap_estatisticas(){
    uart_print("---------------\n");
    uart_print("Heap Total: ");
    uart_print_uint(memory_total());
    uart_print("\n");

    uart_print("Heap Usado: ");
    uart_print_uint(memory_used());
    uart_print("\n");

    uart_print("Heap Livre: ");
    uart_print_uint(memory_free());
    uart_print("\n");
    uart_print("---------------\n");
}

void testes_memoria(){
    heap_estatisticas();

    uart_print("Alocando blocos...\n");
    uart_print("Total de: 3576 bytes\n");
    void *ptr1 = kmalloc(1000);
    void *ptr2 = kmalloc(500);
    void *ptr3 = kmalloc(2000);

    heap_estatisticas();

    uart_print("Liberando memória...\n");
    uart_print("Total de: 2024 bytes\n");
    kfree(ptr3);

    heap_estatisticas();

    uart_print("Reutilizando bloco...\n");
    uart_print("Alocando: 1024 bytes\n");
    void *ptr4 = kmalloc(1000);

    heap_estatisticas();

    uart_print("Teste de coalescencia...\n");
    uart_print("Precisa implementar *mapa da heap* para visualizar esse teste\n");
    kfree(ptr1);
    kfree(ptr4);

    uart_print("Liberando memória restante...\n");
    uart_print("Total de: 528 bytes\n");
    kfree(ptr2);

    heap_estatisticas();
}