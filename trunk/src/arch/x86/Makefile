include ../../common.mk

INCDIR := ../../$(INCDIR)

MOD := drivers memory syscalls v86
SRC := start.S debug.c interrupts.c isr.S isr_handle.c macros.h mem.S pci.c switch_to.S elf.c syscall.c

-include $(patsubst %, %/Makefile, $(MOD))

OBJ := $(patsubst %.S, %.o, $(filter %.S, $(SRC)))
OBJ += $(patsubst %.c, %.o, $(filter %.c, $(SRC)))

all: x86.o clean

x86.o: $(OBJ)
	@$(LD) $(LDFLAGS) -r $(OBJ) -o ../$@

clean:
	@$(RM) -f $(OBJ)
