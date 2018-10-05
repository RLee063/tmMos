ENTRYPOINT	= 0x30400
ENTRYOFFSET 	= 0x400

ASM		= nasm
DASM		= ndisasm
CC		= gcc
LD		= ld
ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -f elf
CFLAGS		= -I include/ -c -fno-builtin -m32 -std=c99
LDFLAGS		= -s -Ttext $(ENTRYPOINT) -m elf_i386
DASMFLAGS	= -u -o $(ENTRYPOINT) -e $(ENTRYOFFSET)

# p

BOOT		= boot/boot.bin boot/loader0.com
KERNEL		= kernel.bin
OBJS		= kernel/kernel.o kernel/start.o kernel/interrupt.o lib/lib.o lib/libc.o kernel/clock.o kernel/main.o
DASMOUTPUT	= kernel.bin.asm

# Phony

.PHONY:	everything final image clean realclean disasm all buildimg

# m

everything : $(BOOT) $(KERNEL)
all: realclean everything
final: all clean
image: final buildimg
clean: 
	rm -f $(OBJS)
realclean:
	rm -f $(OBJS) $(BOOT) $(KERNEL)
disasm:
	$(DASM) $(DASMFLAGS) $(KERNEL) > $(DASMOUTPUT)

#to tmMos.img

buildimg:
	dd if=boot/boot.bin of=tmMos.img bs=512 count=1 conv=notrunc
	sudo mount -o loop tmMos.img /mnt/floppy/
	sudo cp -fv boot/loader0.com /mnt/floppy/
	sudo cp -fv kernel.bin /mnt/floppy/
	sudo umount /mnt/floppy

boot/boot.bin: boot/boot.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/loader0.com: boot/loader.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(KERNEL) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJS)

kernel/main.o: kernel/main.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/kernel.o : kernel/kernel.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/interrupt.o: kernel/interrupt.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/clock.o: kernel/clock.c
	$(CC) $(CFLAGS) -o $@ $<
kernel/start.o: kernel/start.c
	$(CC) $(CFLAGS) -o $@ $<

lib/lib.o: lib/lib.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/libc.o: lib/lib.c
	$(CC) $(CFLAGS) -o $@ $<
