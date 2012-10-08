#############################################################
# Arquivo: common.mk
# Data: 26/02/2008
# Autor: Mauro Joel Sch�tz
# Descri��o: Arquivo com defini��es comuns para a compila��o (Makefile)
#############################################################
# Extens�es permitidas
.SUFFIXES:
.SUFFIXES: .s .S .c .asm .o

# Diret�rios usados
BINDIR := ../bin
INCDIR := ../include

# Arquitetura de saida
ARCH := x86

# Compiladores usados, se n�o for um que voc� usa mude-os!
CC := i586-pc-sombra-gcc
LD := i586-pc-sombra-ld
AR := i586-pc-sombra-ar
MAKE := make
NASM := nasm
RM := rm -f

# Parametros usados nos programas
CFLAGS = -Wall -nostdinc -nostdlib -fno-exceptions -fno-builtin -I$(INCDIR) $(ECFLAGS)
NASMFLAGS = -felf -I$(INCDIR)/

ifndef WINDIR
	ifdef TERM
		ifeq ($(shell uname -s),Linux)
			ifeq ($(shell uname -m),x86_64)
				LDFLAGS := -melf_i386
				CFLAGS += -m32
			endif
		endif
	else
		$(error Sistema Operacional invalido)
	endif

	CC := gcc
	LD := ld
	AR := ar

	NASMFLAGS = -felf32 -I$(INCDIR)/
endif

# Como as extens�es devem ser tratadas?
# Arquivo assembly .S
.S.o:
	@echo Compilando arquivo $<
	@$(CC) $(CFLAGS) -E $< -o $*.asm
	@$(NASM) $(NASMFLAGS) $*.asm -o $@
	@$(RM) $*.asm

# Arquivos C
.c.o:
	@echo Compilando arquivo $<
	@$(CC) $(CFLAGS) -c $< -o $@
