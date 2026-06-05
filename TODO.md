**Nesse trabalho serão modificados os arquivos:**

> kernel/memory.c

- Integração com tasks:
    - criar stacks de tasks com ``kmalloc()``;
    - liberar memória de stacks com ``kfree()``;
    - manter compatibilidade com o sistema de criação de tasks existente.

- Requisitos de funcionamento:
    - o heap deve reutilizar memória liberada;
    - permitir múltiplas alocações e liberações sem reinicialização.
    - Testar

> include/memory.h

> kernel/task.c
- atualizar a criação de tasks para alocar stacks com ``kmalloc()``;

- liberar as stacks quando a task terminar ou for descartada com ``kfree()``;

- manter compatibilidade com o sistema de tasks existente.

> Realizar testes finais!