#ifndef _X86_PAGE_H
#define _X86_PAGE_H

#include <x86-common/_heap.h>
#include <x86-common/frame.h>
#include <kernel.h>
#include <arch.h>
#include _ISR_H_

// Macros para manipular as paginas
#define PAGE_AUX_VIRT_ADDR	0xFF800000
#define PAGE_VIRT_ADDR  	0xFFC00000
#define PAGETAB(pg)     	(void *)(PAGE_VIRT_ADDR + (0x1000 * (pg)))
#define PAGEDIR_PTR     	PAGETAB(1023)
#define PAGETAB_AUX(pg)		(void *)(PAGE_AUX_VIRT_ADDR + (0x1000 * (pg)))
#define PAGEDIR_AUX_PTR		PAGETAB(1022)

// Macros para conversão de endereços
#define PAGE_GET_DIR(addr)  ((unsigned)(addr) >> 22)
#define PAGE_GET_TAB(addr)  (((unsigned)(addr) >> 12) & 0x3FF)

// Definições dos campos das paginas
#define PAGE_PRESENT            0x001
#define PAGE_WRITE              0x002
#define PAGE_USER               0x004
#define PAGE_NOCACHE            0x010
#define PAGE_ACCESSED           0x020
#define PAGE_DIRTY              0x040
#define PAGE_GLOBAL             0x100
#define PAGE_AVAIL              0xE00
#define PAGE_PRESENT_WRITE      (PAGE_PRESENT | PAGE_WRITE)
#define PAGE_PRESENT_WRITE_USER (PAGE_PRESENT | PAGE_WRITE | PAGE_USER)
#define PAGE_KERNEL_PGPROT	(PAGE_PRESENT_WRITE)

#define PG_KERNEL_LIMIT		0x100

#define	USERMEM_STACK_SIZE	0x2000
#define	USERMEM_BASE_ADDR	0x40000000
#define USERMEM_END_ADDR	0xC0000000

#define USERMEM_CODEAREA_PROT	(PAGE_PRESENT | PAGE_USER)
#define USERMEM_DATAAREA_PROT	(PAGE_PRESENT | PAGE_WRITE | PAGE_USER)
#define USERMEM_STACKAREA_PROT	USERMEM_DATA_AREA

// Tipos de Estruturas
typedef page_t  pagedir_t[PAGE_ENTRY_COUNT];
typedef page_t  pagetab_t[PAGE_ENTRY_COUNT];

// Funções para alterar registradores do PC
unsigned cr0_read(void);
void cr0_write(unsigned cr0);
unsigned cr2_read(void);
unsigned cr3_read(void);
void cr3_write(unsigned cr3);

// Funções da paginação
int page_init(void);
int page_map(void *vaddr, void *tovaddr, uint16_t tab_prot, uint16_t pg_prot);
void page_unmap(void *vaddr, void *tovaddr);
void page_restore_kernel_pg(void);
void page_dir_exchange(unsigned *cr3);
void *brk(int sz);
int page_fault(uregs_t *regs);
pagedir_t *page_new();

#define page_free(pd) frame_free((uintptr_t)pd >> 12);

#endif
