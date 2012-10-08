ARCH := x86
include ../common.mk

INCDIR := ../../include

OBJ := start.o loader.o elf.o ../klibc/string/memcpy.o ../klibc/string/memset.o

all: loader.elf

loader.elf: $(OBJ)
	$(LD) $(LDFLAGS) -T ../arch/x86/setup.ld -o $@ $(OBJ)


clean:
	@rm -f loader.elf
	@rm -f $(OBJ)