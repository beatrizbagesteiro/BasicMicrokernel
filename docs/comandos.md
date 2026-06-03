> Instalar todas as dependências:

``
sudo apt update

sudo apt install -y gcc-riscv64-unknown-elf binutils-riscv64-unknown-elf qemu-system-misc make gdb-multiarch
``

> Fazer o build:
``make``

> Iniciar o programa:
qemu-system-riscv64 \
    -machine virt \
    -nographic \
    -bios default \
    -kernel kernel.elf

> Comandos adicionados por mim no Makefile:
``make clean``
- Limpa o build para poder ser buildado novamente;
``make run``
- Roda o programa.