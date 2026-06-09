#include "task.h"
#include "scheduler.h"
#include "memory.h"
#include "testes.h"
#include "uart.h" // Não estava incluso
#include "utils.h" // Adicionando para importar o delay

// extern void uart_print(const char*);

// Tempos de delay
#define DESATIVADO  0UL
#define RAPIDO 10000000UL
#define NORMAL 100000000UL
#define LENTO 250000000UL

#define DELAY LENTO // Definindo o DELAY global

#define TESTE 1  // 1 = teste ativo / 2 = teste desativado
#define TESTE_FRAGMENTACAO 1

/*   Tasks   */

void task1()
{
    while (1)
    {
        uart_print("Task 1 running\n");

        uart_print("Memory used: ");
        uart_print_uint(memory_used());
        uart_print(" bytes\n");

        uart_print("Memory free: ");
        uart_print_uint(memory_free());
        uart_print(" bytes\n\n");

        delay(DELAY);

        yield();
    }
}

void task2()
{
#if !TESTE
    // Modo normal
    while (1)
    {
        uart_print("Task 2 running\n");

        uart_print("Memory used: ");
        uart_print_uint(memory_used());
        uart_print(" bytes\n");

        uart_print("Memory free: ");
        uart_print_uint(memory_free());
        uart_print(" bytes\n\n");

        delay(DELAY);

        yield();
    }
#else
    // Modo teste
    int impressoes = 0; // Criamos um contador

    while (1)
    {
        if (impressoes < 1) // Só entra no if e imprime se ainda não tiver impresso
        {
            uart_print("Task 2 running\n");

            uart_print("Memory used: ");
            uart_print_uint(memory_used());
            uart_print(" bytes\n");

            uart_print("Memory free: ");
            uart_print_uint(memory_free());
            uart_print(" bytes\n\n");

            impressoes++; // Aumenta o contador para nunca mais entrar aqui
        }

        delay(DELAY);

        yield(); // A task continua viva e rodando silenciosamente
    }
#endif
}

/*   Kernel   */

void kernel_main()
{
    memory_init(); // OBRIGATÓRIO

#if TESTE
    testes_memoria();
#endif

    uart_print("\n=== Kernel ===\n");

    xTaskCreate(task1, 2048, 1);
    xTaskCreate(task2, 2048, 1);
    uart_print("Task 1 criada\n");
    uart_print("Task 2 criada\n");

    uart_print("----------------------------------------\n");
    mapa_heap();
    uart_print("\n");
    heap_estatisticas();
    uart_print("\n----------------------------------------\n");

    vTaskDelete(task1);
    uart_print("Task 1 deletada\n");

    uart_print("----------------------------------------\n");
    mapa_heap();
    uart_print("\n");
    heap_estatisticas();
    uart_print("\n----------------------------------------\n");

#if TESTE_FRAGMENTACAO
    void *a = kmalloc(8192);
    void *b = kmalloc(8192);
    void *c = kmalloc(8192);
    void *d = kmalloc(8192);
    void *e = kmalloc(8192);
    void *f = kmalloc(8192);

    kfree(b);
    kfree(d);
    kfree(f);
    
    uart_print("=== TESTE DE FRAGMENTACAO ===\n");

    mapa_heap();
    uart_print("\n");

    heap_estatisticas();

    uart_print("============================\n");

    // === TESTE DE FRAGMENTACAO ===
    // [2149599368] | Tamanho: 2048 bytes | Status: LIVRE
    // [2149601440] | Tamanho: 2048 bytes | Status: OCUPADO
    // [2149603512] | Tamanho: 8192 bytes | Status: OCUPADO
    // [2149611728] | Tamanho: 8192 bytes | Status: LIVRE
    // [2149619944] | Tamanho: 8192 bytes | Status: OCUPADO
    // [2149628160] | Tamanho: 8192 bytes | Status: LIVRE
    // [2149636376] | Tamanho: 8192 bytes | Status: OCUPADO
    // [2149644592] | Tamanho: 20288 bytes | Status: LIVRE

    // Heap Total: 65536
    // Heap Usado: 26720
    // Heap Livre: 38720
    // Fragmentacao: 47%
    // Com 38720B livres e apenas 20288 no maior bloco livre contínuo indica que quase metade da memória livre está espalhada em buracos menores

#endif

#if !TESTE
    xTaskCreate(task1, 2048, 1);
    uart_print("Task 1 criada\n");

    uart_print("----------------------------------------\n");
    mapa_heap();
    uart_print("\n");
    heap_estatisticas();
    uart_print("\n----------------------------------------\n");
#endif

    scheduler_start();

    while (1);
}