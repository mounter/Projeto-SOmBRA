/*****************************************************************************
**                                                                          **
**      Arquivo: main.c                                                     **
**      Descrição: Programa principal do SOmBRA                             **
**      Autor: Mauro Joel Schütz                                            **
**      Data: 17/01/2006                                                    **
**                                                                          **
*****************************************************************************/
#include <kernel/mm.h>
#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/spinlock.h>
#include <kernel/pipe.h>
#include <kernel/info.h>
#include <x86/syscall.h>
#include <x86/pci.h>
#include <drivers/video.h>
#include <drivers/kbd.h>
#include <drivers/timer.h>
#include <drivers/mouse.h>
#include <fs/pipe.h>
#include <fs/fs.h>
#include <stdio.h>


int main()
{
	module_t *mod;

	init_video();
    bem_vindo();

	mm_init();
	init_8253();
	thread_init();
	init_syscall();
//	mouse_install();
	keyboard_init();

	// Escaneia interface PCI
	printf(" %d dispositivos encontrados.\n", pci_discover(1, 0));

	if(!(CHECK_FLAG(multiboot_info, MBF_MODS))){
		printf("Nao há modulos segundo o GRUB.\n");
	}
	else{
	    mod = (module_t *)(multiboot_info->mods_addr);

		// Criando sistema de arquivos!
		fs_root = initialise_initrd((uintptr_t)(mod->mod_start), (uintptr_t)(mod->mod_end));

		// Adicionando /dev
		dev_init_node(fs_root);

		task_create("/init.app", NULL, NULL, (char*)mod[1].mod_start, 100);
	}

	return 0;
}
