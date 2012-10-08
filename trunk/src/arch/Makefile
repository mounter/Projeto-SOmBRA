include ../common.mk

all: $(ARCH).o

x86.o: libudis86.a
	@echo Compilando para arquitetura $*
	@$(MAKE) -C x86 all clean
	@$(LD) $(LDFLAGS) -r $@ -o ../arch.o

x86_64.o:
	@$(MAKE) -C x86_64

libudis86.a:
	@echo Compilando libudis86
	@$(MAKE) -C libudis86

clean:
	@echo Apagando arquivos objetos
	@$(RM) -f *.o
