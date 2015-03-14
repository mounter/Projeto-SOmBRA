include ../../common.mk

INCDIR := ../../$(INCDIR)

.PHONY: all clean

MOD := memory x86emu
SRC := start.S isr_handle.c isr.S mem.S debug.c switch_to.S

-include $(patsubst %, %/Makefile, $(MOD))

OBJ := $(patsubst %.S, %.o, $(filter %.S, $(SRC)))
OBJ += $(patsubst %.c, %.o, $(filter %.c, $(SRC)))

all: x86_64.o clean

x86_64.o: $(OBJ)
	@$(LD) $(LDFLAGS) -r $(OBJ) -o ../$@

clean:
	@$(RM) -f $(OBJ)
