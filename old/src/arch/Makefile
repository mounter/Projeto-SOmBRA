include ../common.mk

.PHONY: all clean libudis86.a

all: $(ARCH).o

x86.o: libudis86.a x86-common.o
	@echo Compilando para arquitetura $*
	@$(MAKE) -C x86 all clean
	@$(LD) $(LDFLAGS) -r $@ x86-common.o -o ../arch.o

x86_64.o: libudis86.a x86-common.o
	@$(MAKE) -C x86_64 all clean
	@$(LD) $(LDFLAGS) -r $@ x86-common.o -o ../arch.o

libudis86.a:
	@echo Compilando libudis86
	@$(MAKE) -C libudis86
	
x86-common.o:
	@$(MAKE) -C x86-common

clean:
	@echo Apagando arquivos objetos
	@$(RM) -f *.o
