include common.mk

MAKEDIRS := arch kernel

.PHONY: all clean klibc.a $(MAKEDIRS)

OBJ := $(foreach FILE, $(MAKEDIRS), $(FILE).o)

all: kernel.elf clean

kernel.elf: $(MAKEDIRS) klibc.a
	@echo Criando arquivo $@
	@$(LD) $(LDFLAGS) -T arch/$(ARCH)/setup.ld -o ../bin/$@ $(OBJ) arch/libudis86.a klibc.a
	@echo Compilacao concluida

$(MAKEDIRS):
	@$(MAKE) -C $@ all clean

klibc.a:
	@$(MAKE) -C klibc all clean

clean:
	@$(RM) $(OBJ)
