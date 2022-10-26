include Makefile.inc

KERNEL=kernel.bin
SOURCES=$(wildcard *.c ./interruptions/*.c ./drivers/*.c ./lib/*.c ./scheduler/*.c ./semaphores/*.c ./memory_manager/*.c ./pipes/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

MM=-D STANDARD

LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld -o kernel.elf  --oformat=elf64-x86-64 $^
	$(LD) $(LDFLAGS) -T kernel.ld -o  $@ $^

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include $(MM) -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o *.bin */*.o

.PHONY: all clean
