#ifndef _X86_USERMEM_H
#define _X86_USERMEM_H

#include <kernel.h>
#include <kernel/types.h>
#include "../_sizet.h"

#define	USERMEM_STACK_SIZE	0x2000
#define	USERMEM_BASE_ADDR	0x40000000
#define USERMEM_END_ADDR	0xC0000000

#define USERMEM_CODEAREA_PROT	(PAGE_PRESENT | PAGE_USER)
#define USERMEM_DATAAREA_PROT	(PAGE_PRESENT | PAGE_WRITE | PAGE_USER)
#define USERMEM_STACKAREA_PROT	USERMEM_DATA_AREA

bool usermem_map(aspace_t *as);
void usermem_unmap(aspace_t *as);
void usermem_do_process(uregs_t *regs);

#include <x86/page.h>

#endif

