CC=gcc
LD=ld -m elf_i386
CFLAGS = -Wall -Wextra -pedantic -m32 -O0 -std=c99 -finline-functions -fno-stack-protector -nostdinc -ffreestanding -Wno-unused-function -Wno-unused-parameter
NASM = nasm -f elf
ECHO = `which echo` -e
MODULES = $(patsubst %.c,%.o,$(wildcard core/*.c))
FILESYSTEMS = $(patsubst %.c,%.o,$(wildcard core/fs/*.c))

.PHONY: kernel initrd

all: kernel initrd

install: kernel initrd
	cp bootdisk.src.img bootdisk.img
	mount bootdisk.img /mnt -o loop
	cp kernel /mnt/kernel
	cp initrd /mnt/initrd
	umount /mnt
	cp kernel /boot/MyaXos-kernel
	cp initrd /boot/MyaXos-initrd

run: bootdisk.img
	qemu-system-x86_64 -fda bootdisk.img

curses: bootdisk.img
	qemu -curses -fda bootdisk.img

kernel: start.o link.ld main.o ${MODULES} ${FILESYSTEMS}
	${LD} -T link.ld -o kernel *.o core/*.o core/fs/*.o

%.o: %.c
	${CC} ${CFLAGS} -I./include -c -o $@ $<

start.o: start.asm
	nasm -f elf -o start.o start.asm

initrd: fs
	-rm -f initrd
	genext2fs -d fs -q -b 249 -v initrd

clean:
	-rm -f *.o kernel
	-rm -f bootdisk.img
	-rm -f initrd
	-rm -f core/*.o
	-rm -f core/fs/*.o
