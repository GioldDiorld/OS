# commands/Makefile

#ENTRYPOINT = 0x1000
HD = ../100m.img

ASM 	= nasm
DASM 	= objdump
CC 		= gcc
LD 		= ld

ASMFLAGS 	= -I ../include/ -f elf
CFLAGS 		= -m32 -fno-pie -I ../include/ -c -fno-builtin -fno-stack-protector -Wall
LDFLAGS 	= -Ttext 0x1000 -m elf_i386
DASMFLAGS 	= -D

LIB = ../lib/orangescrt.a

SRC := $(wildcard *.c)
BIN := $(SRC:.c=)

# All Phony Targets
.PHONY : everything final clean realclean disasm all install
# Default starting position
everything : $(BIN)

install : all clean
	tar vcf inst.tar $(BIN)
	dd if=inst.tar of=$(HD) seek=$(shell echo "obase=10;ibase=16;(`egrep -e '^ROOT_BASE' ../boot/include/load.inc | sed -e 's/.*0x//g'`+`egrep -e '#define[[:space:]]*INSTALL_START_SECT' ../include/sys/config.h | sed -e 's/.*0x//g'`)*200" | bc) bs=1 count=$(shell ls -l inst.tar | awk -F " " '{print $$5}') conv=notrunc
	rm -f $(BIN) ./*.o
all : realclean everything

clean :
	rm -f ./*.o

realclean :
	rm -f $(BIN) ./*.o

start.o : start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

%: %.c start.o
	$(CC) $(CFLAGS) $< -o $(patsubst %.c, %.o, $<)
	$(LD) $(LDFLAGS) -o $@ $(patsubst %.c, %.o, $^) $(LIB)