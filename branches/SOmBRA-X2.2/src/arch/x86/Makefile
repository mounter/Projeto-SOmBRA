include ../../common.mk

INCDIR := ../../$(INCDIR)

.PHONY: all clean

MOD := memory
SRC := start.S isr_handle.c isr.S debug.c mem.S

-include $(patsubst %, %/Makefile, $(MOD))

OBJ := $(patsubst %.S, %.o, $(filter %.S, $(SRC)))
OBJ += $(patsubst %.c, %.o, $(filter %.c, $(SRC)))

all: x86.o clean

x86.o: $(OBJ)
	@$(LD) $(LDFLAGS) -r $(OBJ) -o ../$@

clean:
	@$(RM) -f $(OBJ)
