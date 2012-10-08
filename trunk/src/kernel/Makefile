include ../common.mk

INCDIR := ../$(INCDIR)
ECFLAGS := -I $(INCDIR)/kernel

#MOD := v86
SRC := main.c info.c mm.c schedule.c thread.c semaphore.c queue.c pipe.c task.c aspace.c

#-include $(patsubst %, %/Makefile, $(MOD))

OBJ := $(patsubst %.S, %.o, $(filter %.S, $(SRC)))
OBJ += $(patsubst %.c, %.o, $(filter %.c, $(SRC)))

all: $(OBJ)
	@$(LD) $(LDFLAGS) -r $(OBJ) -o ../kernel.o

clean:
	@echo Limpando arquivos objetos
	@$(RM) -f $(OBJ)
