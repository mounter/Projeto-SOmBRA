#############################################################
# Arquivo: common.mk
# Data: 26/02/2008
# 		18/05/2011 - Editado para suporte a ARCH=X86_64
# Autor: Mauro Joel Schütz
# Descrição: Arquivo com definições comuns para a compilação (Makefile)
#############################################################
# Extensões permitidas
.SUFFIXES:
.SUFFIXES: .s .S .c .asm .o

# Diretórios usados
BINDIR := ../bin
INCDIR := ../include

# Arquitetura de saida
ifndef ARCH
ARCH := x86_64
endif

# Parametros usados nos programas
CFLAGS = -Wall  -I$(INCDIR) $(ECFLAGS)
LDFLAGS = -nostdlib -nodefaultlibs
NASMFLAGS = -I$(INCDIR)/

# Como usamos um compiladores de 64-bits, iremos somente adapta-lo para várias plataformas
# mantendo a compatibilidade de compilação!
COMPILER_PREFIX := x86_64-pc-elf-

ifeq ($(ARCH),x86)
CFLAGS += -m32 -nostdinc -nostdlib -fno-exceptions -fno-builtin
LDFLAGS += -melf_i386
NASMFLAGS += -felf32
else
CFLAGS += -m64 -ffreestanding -nostdlib -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow
NASMFLAGS += -felf64
endif

# Compiladores usados
CC := $(COMPILER_PREFIX)gcc
LD := $(COMPILER_PREFIX)ld
AR := $(COMPILER_PREFIX)ar
MAKE := make
NASM := nasm
RM := rm -f

# Como as extensões devem ser tratadas?
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
