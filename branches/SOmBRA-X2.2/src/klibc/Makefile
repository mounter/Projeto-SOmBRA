include ../common.mk

INCDIR := ../../include

#MOD := ctype printf stdio string system setjmp
MOD := ctype printf stdio string system time stdlib
OBJ :=

-include $(patsubst %, %/Makefile, $(MOD))

all: klibc.a

klibc.a: $(OBJ)
	@echo Construindo biblioteca $@
	@$(AR) cru ../$@ $(OBJ)

clean:
	@echo Limpando arquivos objetos
	@rm -f $(OBJ)
