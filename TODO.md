**Nesse trabalho serão modificados os arquivos:**

> kernel/memory.c
- Remover a lógica do Bump Allocator;

- Implementar a inicialização da lista livre (Como referência já tem um algoritmo que fora feito em Estrutura de Dados no passado, apenas precisa de adaptações);

- First-Fit (Basicamente só procurar a primeira lista livre com data = NULL);

- kmalloc() > aloca memória no heap;
    - ``void *kmalloc(uint64_t size)`` -> Parâmetros: Tamanho alocado
    - ex: ``buf = kmalloc(1024)``

- kfree() > libera a memória que foi alocada por kmalloc() | Ainda não faz nada
    - ex: ``kfree(buf)``

- Implementar bloco:
    ``
    typedef struct block 
    { 
        uint64_t size; 
        int free; 
        struct block *next; 
    } block_t;
    ``

- Implementar obrigatoriamente:
    - ``void memory_init(void);``
    - inicializar a lista livre; (Tem no código de exemplo)
    - configurar o primeiro bloco;
    - preparar o heap para alocação dinâmica.

- Alocação dinâmica deve:
    - usar política First Fit;
    - percorrer a lista de blocos; (Ir percorrendo os ponteiros *next até chegar no último ponteiro NULL)
    - localizar blocos livres; (usando int free creio eu)
    - dividir blocos quando necessário; (Tamanho do bloco e divide 'size' vezes os bits/caracteres/dado)
    - retornar ponteiro para a área útil.

- Desalocação deve:
    - recuperar o cabeçalho do bloco;
    - marcar o bloco como livre; (int free)
    - implementar coalescência.

- Estatísticas do heap:
    - ``uint64_t memory_used(void);``
    - ``uint64_t memory_free(void);``
    - ``uint64_t memory_total(void);``

- Integração com tasks:
    - criar stacks de tasks com ``kmalloc()``;
    - liberar memória de stacks com ``kfree()``;
    - manter compatibilidade com o sistema de criação de tasks existente.

- Requisitos técnicos:
    - não utilizar bibliotecas externas;
    - não utilizar ``malloc()`` da libc.

- Requisitos de funcionamento:
    - o heap deve reutilizar memória liberada;
    - permitir múltiplas alocações e liberações sem reinicialização.

> include/memory.h
- declarar as funções do heap:
    - ``void memory_init(void);``
    - ``void *kmalloc(uint64_t size);``
    - ``void kfree(void *ptr);``
    - ``uint64_t memory_used(void);``
    - ``uint64_t memory_free(void);``
    - ``uint64_t memory_total(void);``

> kernel/task.c
- atualizar a criação de tasks para alocar stacks com ``kmalloc()``;

- liberar as stacks quando a task terminar ou for descartada com ``kfree()``;

- manter compatibilidade com o sistema de tasks existente.