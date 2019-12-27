include Makefile.inc

DIRS = core

.PHONY: all clean install core run curses

all: kernel

install: kernel
	sudo mount bootdisk.img /mnt -o loop
	sudo cp kernel /mnt/kernel
	sudo cp initrd /mnt/initrd
	sudo umount /mnt
	sudo cp kernel /boot/myaxos-kernel

kernel: start.o link.ld main.o core
	${LD} -T link.ld -o kernel *.o core/*.o

%.o: %.c
	${CC} ${CFLAGS} -I./include -c -o $@ $<

core:
	cd core; ${MAKE} ${MFLAGS}

start.o: start.asm
	nasm -f elf -o start.o start.asm

run: bootdisk.img
	qemu-system-x86_64 -fda bootdisk.img

curses: bootdisk.img
	qemu-system-x86_64 -curses -fda bootdisk.img

clean:
	-rm -f *.o kernel
	-for d in ${DIRS}; do (cd $$d; ${MAKE} clean); done
