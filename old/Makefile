include src/common.mk

.PHONY: all clean SOmBRA.iso run

all:
	@$(MAKE) -C src all

clean:
	@$(MAKE) -C src clean

SOmBRA.iso:
	@echo Criando imagem $@
	@$(RM) ./bin/$@
	@mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o ./bin/$@ ./bin

run:
	@echo Executando bochs...
	@cd bin;bochs -f bochsrc.bxrc
