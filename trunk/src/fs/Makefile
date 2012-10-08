include ../common.mk

INCDIR := ../$(INCDIR)
ECFLAGS := -I $(INCDIR)/fs

#MOD := v86
SRC := dev.c fs.c functions.c lineint.c lineint.h condev.c condev.h ramdisk.c ramdisk.h pipefs.c pipefs.h

#-include $(patsubst %, %/Makefile, $(MOD))

OBJ := $(patsubst %.S, %.o, $(filter %.S, $(SRC)))
OBJ += $(patsubst %.c, %.o, $(filter %.c, $(SRC)))

all: $(OBJ)
	@$(LD) $(LDFLAGS) -r $(OBJ) -o ../fs.o

clean:
	@echo Limpando arquivos objetos
	@$(RM) -f $(OBJ)
