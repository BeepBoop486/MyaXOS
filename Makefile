#CC = gcc
CC = clang
CFLAGS = -Wall -Wextra -pedantic -m32 -O0 -std=c99 -finline-functions -fno-stack-protector -nostdinc -ffreestanding -Wno-unused-function -Wno-unused-parameter
LD = ld -m elf_i386
NASM = nasm
ECHO = `which echo` -e
MODULES = $(patsubst %.c,%.o,$(wildcard kernel/core/*.c))
FILESYSTEMS = $(patsubst %.c,%.o,$(wildcard kernel/core/fs/*.c))
EMU = qemu-system-x86_64
GENEXT = genext2fs
DD = dd conv=notrunc

.PHONY: all clean install run

all: mya-kernel mya-initrd

install: mya-kernel mya-initrd
	@${ECHO} -n "\033[34m   --   Installing to /boot...\033[0m"
	@cp mya-kernel /boot/mya-kernel
	@cp mya-initrd /boot/mya-initrd
	@${ECHO} "\r\033[34;1m   --   Kernel and ramdisk installed.\033[0m"

run: mya-kernel mya-initrd
	${EMU} -kernel mya-kernel -initrd mya-initrd -serial stdio

##########
# KERNEL #
##########
mya-kernel: kernel/start.o kernel/link.ld kernel/main.o ${MODULES} ${FILESYSTEMS}
	@${ECHO} -n "\033[32m   LD   $<\033[0m"
	@${LD} -T kernel/link.ld -o mya-kernel kernel/*.o kernel/core/*.o kernel/core/fs/*.o
	@${ECHO} "\r\033[32;1m   LD   $<\033[0m"

kernel/start.o: kernel/start.asm
	@${ECHO} -n "\033[32m  nasm  kernel/start.asm\033[0m"
	@${NASM} -f elf -o kernel/start.o kernel/start.asm
	@${ECHO} "\r\033[32;1m  nasm  kernel/start.asm\033[0m"

%.o: %.c
	@${ECHO} -n "\033[32m   CC   $<\033[0m"
	@${CC} ${CFLAGS} -I./kernel/include -c -o $@ $<
	@${ECHO} "\r\033[32;1m   CC   $<\033[0m"

############
# Ram disk #
############

mya-initrd: initrd bootloader/stage1
	@${ECHO} -n "\033[32m initrd  Generating initial RAM disk\033[0m"
	@-rm -f mya-initrd
	@${GENEXT} -d initrd -q -b 249 mya-initrd
	@${DD} if=bootloader/stage1 of=mya-initrd 2>/dev/null
	@${ECHO} "\r\033[32;1m initrd  Generated initial RAM disk image\033[0m"

##############
# Bootloader #
##############
bootloader/stage1: bootloader/stage1.s
	@${ECHO} -n "\033[32m nasm bootloader/stage1.s\033[0m"
	@${NASM} -f bin -o bootloader/stage1 bootloader/stage1.s
	@${ECHO} "\r\033[32;1m nasm bootloader/stage1.s\033[0m"

clean:
	@${ECHO} -n "\033[31m   RM   Cleaning...\033[0m"
	@-rm -f mya-kernel
	@-rm -f mya-initrd
	@-rm -f kernel/*.o
	@-rm -f kernel/core/*.o
	@-rm -f kernel/core/fs/*.o
	@${ECHO} "\r\033[31;1m   RM   Finished cleaning.\033[0m"
