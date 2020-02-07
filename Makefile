#CC = gcc
CC = clang
GCC = gcc
CFLAGS = -Wall -Wextra -pedantic -m32 -O0 -std=c99 -finline-functions -fno-stack-protector -nostdinc -ffreestanding -Wno-unused-function -Wno-unused-parameter -fno-pie
LD = ld -m elf_i386
YASM = yasm
ECHO = `which echo` -e
MODULES = $(patsubst %.c,%.o,$(wildcard kernel/core/*.c))
FILESYSTEMS = $(patsubst %.c,%.o,$(wildcard kernel/core/fs/*.c))
EMU = qemu-system-x86_64
GENEXT = genext2fs
DD = dd conv=notrunc

.PHONY: all clean install run

all: mya-kernel mya-initrd

install: mya-initrd mya-kernel
	@${ECHO} -n "\033[34m   --   Installing to /boot...\033[0m"
	@cp mya-kernel /boot/mya-kernel
	@cp mya-initrd /boot/mya-initrd
	@${ECHO} "\r\033[34;1m   --   Kernel and ramdisk installed.\033[0m"

run: mya-initrd mya-kernel
	${EMU} -kernel mya-kernel -initrd mya-initrd -serial stdio

################
#    Kernel    #
################
mya-kernel: kernel/start.o kernel/link.ld kernel/main.o ${MODULES} ${FILESYSTEMS}
	@${ECHO} -n "\033[32m   LD   $<\033[0m"
	@${LD} -T kernel/link.ld -o mya-kernel kernel/*.o kernel/core/*.o kernel/core/fs/*.o
	@${ECHO} "\r\033[32;1m   LD   $<\033[0m"

kernel/start.o: kernel/start.asm
	@${ECHO} -n "\033[32m  yasm  $<\033[0m"
	@${YASM} -f elf -o kernel/start.o kernel/start.asm
	@${ECHO} "\r\033[32;1m  yasm  $<\033[0m"

%.o: %.c
	@${ECHO} -n "\033[32m   CC   $<\033[0m"
	@${CC} ${CFLAGS} -I./kernel/include -c -o $@ $<
	@${ECHO} "\r\033[32;1m   CC   $<\033[0m"

################
#   Ram disk   #
################
mya-initrd: initrd/kernel
	@${ECHO} -n "\033[32m initrd Generating initial RAM disk\033[0m"
	@-rm -f mya-initrd
	@${GENEXT} -d initrd -q -b 249 mya-initrd
	@${DD} if=bootloader/stage1.bin of=mya-initrd 2>/dev/null
	@${ECHO} "\r\033[32;1m initrd Generated initial RAM disk image\033[0m"

### Ram Disk installers...

# Kernel
initrd/kernel: mya-kernel
	@cp mya-kernel initrd/boot/kernel

################
#  Bootloader  #
################

# Stage 1
bootloader/stage1/main.o: bootloader/stage1/main.c
	@${ECHO} -n "\033[32m   CC   $<\033[0m"
	@${GCC} ${CFLAGS} -c -o $@ $<
	@${ECHO} "\r\033[32;1m   CC   $<\033[0m"

bootloader/stage1/start.o: bootloader/stage1/start.s
	@${ECHO} -n "\033[32m  yasm  $<\033[0m"
	@${YASM} -f elf32 -p gas -o $@ $<
	@${ECHO} "\r\033[32;1m  yasm  $<\033[0m"

bootloader/stage1.bin: bootloader/stage1/main.o bootloader/stage1/start.o bootloader/stage1/link.ld
	@${ECHO} -n "\033[32m   ld   $<\033[0m"
	@${LD} -o bootloader/stage1.bin -T bootloader/stage1/link.ld bootloader/stage1/start.o bootloader/stage1/main.o
	@${ECHO} "\r\033[32;1m   ld   $<\033[0m"

# Stage 2
bootloader/stage2/main.o: bootloader/stage2/main.c
	@${ECHO} -n "\033[32m   CC   $<\033[0m"
	@${GCC} ${CFLAGS} -c -o $@ $<
	@${ECHO} "\r\033[32;1m   CC   $<\033[0m"

bootloader/stage2/start.o: bootloader/stage2/start.s
	@${ECHO} -n "\033[32m  yasm  $<\033[0m"
	@${YASM} -f elf32 -p gas -o $@ $<
	@${ECHO} "\r\033[32;1m  yasm  $<\033[0m"

bootloader/stage2.bin: bootloader/stage2/main.o bootloader/stage2/start.o bootloader/stage2/link.ld
	@${ECHO} -n "\033[32m   ld   $<\033[0m"
	@${LD} -o bootloader/stage2.bin -T bootloader/stage2/link.ld bootloader/stage2/start.o bootloader/stage2/main.o
	@${ECHO} "\r\033[32;1m   ld   $<\033[0m"

bootdisk.img: bootloader/stage1.bin bootloader/stage2.bin
	@${ECHO} -n "\033[34m   --   Building bootdisk.img...\033[0m"
	@cat bootloader/stage1.bin bootloader/stage2.bin > bootdisk.img
	@${ECHO} "\r\033[34;1m   --   Bootdisk compiled.  \033[0m"

###############
#    clean    #
###############
clean:
	@${ECHO} -n "\033[31m   RM   Cleaning... \033[0m"
	@-rm -f mya-kernel
	@-rm -f mya-initrd
	@-rm -f kernel/*.o
	@-rm -f kernel/core/*.o
	@-rm -f kernel/core/fs/*.o
	@-rm -f bootloader/stage1.bin
	@-rm -f bootloader/stage1/*.o
	@-rm -f bootloader/stage2.bin
	@-rm -f bootloader/stage2/*.o
	@-rm -f initrd/stage2
	@-rm -f initrd/kernel
	@-rm -f testdisk
	@${ECHO} "\r\033[31;1m   RM   Finished cleaning.\033[0m\033[K"
