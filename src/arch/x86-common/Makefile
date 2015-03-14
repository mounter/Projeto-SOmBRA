include ../../common.mk

INCDIR := ../../$(INCDIR)

.PHONY: all clean

MOD := drivers memory
SRC := interrupts.c elf.c

-include $(patsubst %, %/Makefile, $(MOD))

OBJ := $(patsubst %.S, %.o, $(filter %.S, $(SRC)))
OBJ += $(patsubst %.c, %.o, $(filter %.c, $(SRC)))

all: x86-common.o clean

x86-common.o: $(OBJ)
	@$(LD) $(LDFLAGS) -r $(OBJ) -o ../$@

clean:
	@$(RM) -f $(OBJ)
