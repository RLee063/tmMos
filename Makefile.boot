#Makefile for boot

#ASM

ASM = nasm
ASMFLAGS = -I boot/include/

#This Program

BOOT = boot/boot.bin boot/loader0.com

# ALL Phony

.PHONY : everything clean all

# Default starting position

everything : $(BOOT)

clean: 
	rm -f $(BOOT)

all: clean everything

boot/boot.bin: boot/boot.asm boot/include/FAT12.inc boot/include/readSector.inc boot/include/showMessage.inc boot/include/readFile.inc
	$(ASM) $(ASMFLAGS) -o $@ $<

boot/loader0.com: boot/loader.asm boot/include/protectMode.inc boot/include/FAT12.inc boot/include/loadConvention.inc boot/include/readFile.inc boot/include/showMessage.inc boot/include/lib.inc
	$(ASM) $(ASMFLAGS) -o $@ $<
	
