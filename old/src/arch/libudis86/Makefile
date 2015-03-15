
include ../../common.mk

INCDIR := ../../$(INCDIR) -D__UD_STANDALONE__

SRC := mnemonics.c opcmap.c input.c decode.c syn.c syn-intel.c syn-att.c udis86.c

OBJ := $(patsubst %.c, %.o, $(filter %.c, $(SRC)))

all: libudis86.a clean

libudis86.a: $(OBJ)
	@$(AR) -cru ../$@ $(OBJ)

clean:
	@$(RM) -f $(OBJ)