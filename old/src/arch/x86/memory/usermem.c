#include <x86/usermem.h>
#include <x86/regs.h>
#include <kernel/thread.h>
#include <kernel/aspace.h>
#include <string.h>
#include <stdio.h>

extern unsigned *pagedir;

bool usermem_map(aspace_t *as)
{
	sect_t *sect;
	unsigned i, addr, uvirt_to_kvirt;
	unsigned short prot;

	uvirt_to_kvirt = (unsigned)as->user_mem - as->virt_adr;

	aspace_section_add(as, (USERMEM_END_ADDR - USERMEM_STACK_SIZE) - uvirt_to_kvirt, USERMEM_STACK_SIZE-1, SF_BSS, 0);

	/* Carrega as seções, no caso do BSS(SF_ZERO) zera o conteúdo. */
	for(i = 0; i < as->num_sects; i++){
		sect = as->sect + i;
		if(sect->flags & (SF_LOAD | SF_ZERO)){
			addr = sect->adr + uvirt_to_kvirt;

			if (sect->flags & (SF_EXEC))
				prot = USERMEM_CODEAREA_PROT;
			else	prot = USERMEM_DATAAREA_PROT;

			/* Para Code e Data. */
			printf("PAGEMAP(%X, %X, %X) on %X\n", addr, addr+sect->size, prot, pagedir[1023]);
			page_map((void*)addr, (void*)(addr+sect->size), PAGE_PRESENT_WRITE_USER, prot);
		}
	}

	return true;
}

void usermem_unmap(aspace_t *as)
{
	sect_t *sect;
	unsigned i, addr, uvirt_to_kvirt;

	uvirt_to_kvirt = (unsigned)as->user_mem - as->virt_adr;

	/* Carrega as seções, no caso do BSS(SF_ZERO) zera o conteúdo. */
	for(i = 0; i < as->num_sects; i++){
		sect = as->sect + i;
		if(sect->flags & (SF_LOAD | SF_ZERO)){
			addr = sect->adr + uvirt_to_kvirt;

			page_unmap((void*)addr, (void*)(addr+sect->size));
		}
	}
}

