#########################
# Makefile for Orange'S #
#########################

# Entry point of Orange'S
# It must have the same value with 'KernelEntryPointPhyAddr' in load.inc!
#ENTRYPOINT	= 0x400000
ENTRYPOINT	= 0x1000

FD		= a.img
HD		= 100m.img

# Programs, flags, etc.
AR		= ar
ASM		= nasm
DASM	= objdump
CC		= gcc
LD		= ld
ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -I include/sys/ -f elf
CFLAGS		= -I include/ -I include/sys/ -c -fno-builtin -fno-stack-protector -fpack-struct -Wall
#CFLAGS		= -I include/ -c -fno-builtin -fno-stack-protector -fpack-struct -Wall
LDFLAGS		= -flto -m elf_i386 -Ttext $(ENTRYPOINT) -Map krnl.map
#ASMBFLAGS	= -I boot/include/
#ASMKFLAGS	= -I include/ -I include/sys/ -f elf
#CFLAGS		= -fno-pie -m32 -I include/ -I include/sys/ -c -fno-builtin -fno-stack-protector -fpack-struct -Wall
##CFLAGS		= -I include/ -c -fno-builtin -fno-stack-protector -fpack-struct -Wall
#LDFLAGS		=  -flto -m elf_i386 -Ttext $(ENTRYPOINT) -Map krnl.map
DASMFLAGS	= -D
ARFLAGS		= rcs

# This Program
ORANGESBOOT	= boot/boot.bin boot/hdboot.bin boot/loader.bin boot/hdldr.bin
ORANGESKERNEL	= kernel.bin
SYMKERNEL = kernel.dbg

LIB	= lib/orangescrt.a
LIBSRC := $(wildcard lib/*.c)
LIBOBJS := $(LIBSRC:.c=.o) lib/syscall.o

SRCDIRS = kernel lib fs mm
SRCASM = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.asm))
SRCC = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))

OBJS = $(SRCASM:.asm=.o) $(SRCC:.c=.o)

DASMOUTPUT	= kernel.bin.asm

# All Phony Targets
.PHONY : everything final image clean realclean disasm all buildimg

# Default starting position
nop :
	@echo "why not \`make image' huh? :)"

everything : $(ORANGESBOOT) $(ORANGESKERNEL)


image : realclean everything clean buildimg

clean :
	rm -f $(OBJS) $(LOBJS)

realclean :
	rm -f $(OBJS) $(LOBJS) $(ORANGESBOOT) $(ORANGESKERNEL) $(SYMKERNEL)

disasm :
	$(DASM) $(DASMFLAGS) $(ORANGESKERNEL) > $(DASMOUTPUT)

buildimg :
	dd if=boot/boot.bin of=$(FD) bs=512 count=1 conv=notrunc
	dd if=boot/hdboot.bin of=$(HD) bs=1 count=446 conv=notrunc
	dd if=boot/hdboot.bin of=$(HD) seek=510 skip=510 bs=1 count=2 conv=notrunc
#	dd if=boot/hdboot.bin of=$(HD) seek=`echo "obase=10;ibase=16;\`egrep -e '^ROOT_BASE' boot/include/load.inc | sed -e 's/.*0x//g'\`*200" | bc` bs=1 count=446 conv=notrunc
#	dd if=boot/hdboot.bin of=$(HD) seek=`echo "obase=10;ibase=16;\`egrep -e '^ROOT_BASE' boot/include/load.inc | sed -e 's/.*0x//g'\`*200+1FE" | bc` skip=510 bs=1 count=2 conv=notrunc
	sudo mount -o loop $(FD) /mnt/floppy/
	sudo cp -fv boot/loader.bin /mnt/floppy/
	sudo cp -fv kernel.bin /mnt/floppy
	sudo umount /mnt/floppy

boot/boot.bin : boot/boot.asm boot/include/load.inc boot/include/fat12hdr.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/hdboot.bin : boot/hdboot.asm boot/include/load.inc boot/include/fat12hdr.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/loader.bin : boot/loader.asm boot/include/load.inc boot/include/fat12hdr.inc boot/include/pm.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/hdldr.bin : boot/hdldr.asm boot/include/load.inc boot/include/fat12hdr.inc boot/include/pm.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(ORANGESKERNEL) : $(OBJS) $(LIB)
	$(LD) $(LDFLAGS) -o $(ORANGESKERNEL) $^

$(LIB) : $(OBJS)
	$(AR) $(ARFLAGS) $@ $(LIBOBJS)

$(SRCASM:%.asm=%.o): %.o: %.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

$(SRCC:%.c=%.o): %.o: %.c
	$(CC) $(CFLAGS) -o $@ $<