CROSS = riscv64-unknown-elf-

CFLAGS = -march=rv64gc -mabi=lp64 \
         -mcmodel=medany \
         -msmall-data-limit=0 \
         -ffreestanding \
         -nostdlib -nostartfiles \
         -fno-stack-protector \
         -Wall -Iinclude

OBJS = start.o trap_entry.o context.o \
       main.o task.o scheduler.o uart.o string.o memory.o

all:
	$(CROSS)gcc $(CFLAGS) -c boot/start.S
	$(CROSS)gcc $(CFLAGS) -c boot/trap_entry.S
	$(CROSS)gcc $(CFLAGS) -c kernel/context.S

	$(CROSS)gcc $(CFLAGS) -c kernel/main.c
	$(CROSS)gcc $(CFLAGS) -c kernel/task.c
	$(CROSS)gcc $(CFLAGS) -c kernel/scheduler.c
	$(CROSS)gcc $(CFLAGS) -c kernel/uart.c
	$(CROSS)gcc $(CFLAGS) -c kernel/string.c
	$(CROSS)gcc $(CFLAGS) -c kernel/memory.c

	$(CROSS)ld -T linker.ld $(OBJS) -o kernel.elf

run-make: all
	qemu-system-riscv64 \
		-machine virt \
		-nographic \
		-bios default \
		-kernel kernel.elf
		# \ 2>&1 | tee qemu.log
		# Isso é para salvar o log externamente. Remova os '#' no Makefile para funcionar (Isso salvará um qemu.log na pasta principal)
clean:
	rm -f *.o kernel.elf

run:
	qemu-system-riscv64 \
		-machine virt \
		-nographic \
		-bios default \
		-kernel kernel.elf