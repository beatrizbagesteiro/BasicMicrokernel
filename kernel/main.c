#include "task.h"
#include "scheduler.h"
#include "memory.h"
#include "testes.h"
#include "uart.h" // Não estava incluso

// extern void uart_print(const char*);

#define TESTE 1  // 1 = teste ativo / 2 = teste desativado

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