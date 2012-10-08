/*****************************************************************************
**
**	Arquivo: x86_64/page.h
**	Descrição: Variaveis, constantes e tipos de paginação
**	Autor: Mauro Joel Schütz
**	Data: 14/05/2011
**
*****************************************************************************/
#ifndef _X86_64_PAGE_H
#define _X86_64_PAGE_H

// Verificar calculos

#include <x86-common/_heap.h>
#include <x86-common/frame.h>

// Macros para converter endereço para entradas da paginação
#define PAGE_ADDR_PML4E(addr)	(uint64_t)(((uint64_t)addr >> 39) & 0x1FF)
#define PAGE_ADDR_PDPE(addr)	(uint64_t)(((uint64_t)addr >> 30) & 0x1FF)
#define PAGE_ADDR_PDE(addr)		(uint64_t)(((uint64_t)addr >> 21) & 0x1FF)
#define PAGE_ADDR_PTE(addr)		(uint64_t)(((uint64_t)addr >> 12) & 0x1FF)

// Macros para checar mudança no endereço
#define PAGE_ADDR_CHECK_PML4E(addr, pml4e)	(int)(PAGE_ADDR_PML4E(addr) == pml4e)
#define PAGE_ADDR_CHECK_PDPE(addr, pdpe)	(int)(PAGE_ADDR_PDPE(addr) == pdpe)
#define PAGE_ADDR_CHECK_PDE(addr, pde)		(int)(PAGE_ADDR_PDE(addr) == pde)
#define PAGE_ADDR_CHECK_PTE(addr, pte)		(int)(PAGE_ADDR_PTE(addr) == pte)

// Para lidar com endereços canonicos
#define PAGE_CANONICAL_BIT				(uint64_t)((uint64_t)1 << (uint64_t)47)
#define PAGE_ADDR_IS_CANONICAL(addr)	(uint64_t)((uint64_t)(addr) & PAGE_CANONICAL_BIT)
#define PAGE_MK_ADDR_CANONICAL(addr)	(uint64_t)(PAGE_ADDR_IS_CANONICAL(addr) ? (addr) | 0xFFFF000000000000 : (addr))

// Tamanho de cada tabela
#define PAGE_PML4_SIZE 0x8000000000
#define PAGE_PDP_SIZE	0x40000000
#define PAGE_PD_SIZE	0x200000
#define PAGE_PT_SIZE	0x1000

// Calcula endereço de uma entrada na PML4E
#define PAGE_PML4E_ADDR(pml4e)					PAGE_MK_ADDR_CANONICAL((uint64_t)pml4e << (uint64_t)39)
#define PAGE_PDPE_ADDR(pml4e, pdpe)				(PAGE_PML4E_ADDR(pml4e) | ((uint64_t)pdpe << (uint64_t)30))
#define PAGE_PDE_ADDR(pml4e, pdpe, pde) 		(PAGE_PDPE_ADDR(pml4e, pdpe) | ((uint64_t)pde << (uint64_t)21))
#define PAGE_PTE_ADDR(pml4e, pdpe, pde, pte)	(PAGE_PDE_ADDR(pml4e, pdpe, pde) | ((uint64_t)pte << (uint64_t)12))

// Entradas reservadas da PML4
#define KERNEL_PML4E	0x1FF	// Entrada do gerenciamento de páginas do kernel
#define USER_PML4E		0x1FE	//Entrada do gerenciamento de páginas do usuário

// Calcula os endereços das estruturas
#define PAGE_PML4_TAB_ADDR(pml4b)					(page_t*)PAGE_PTE_ADDR(pml4b, 0x1FF, 0x1FF, 0x1FF)
#define PAGE_PDP_TAB_ADDR(pml4b, pml4e)				(page_t*)PAGE_PTE_ADDR(pml4b, 0x1FF, 0x1FF, pml4e)
#define PAGE_PD_TAB_ADDR(pml4b, pml4e, pdpe)		(page_t*)PAGE_PTE_ADDR(pml4b, 0x1FF, pml4e, pdpe)
#define PAGE_PT_TAB_ADDR(pml4b, pml4e, pdpe, pde)	(page_t*)PAGE_PTE_ADDR(pml4b, pml4e, pdpe, pde)

// Usado para remover endereços da TLB
#define PAGE_REM_ADDR(addr) __asm__ volatile("invlpg %0"::"m" (*(char *)(addr)))

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
#define PAGE_KERNEL_PGPROT		(PAGE_PRESENT_WRITE)

#define	USERMEM_STACK_SIZE	0x2000
#define	USERMEM_BASE_ADDR	0x40000000
#define USERMEM_END_ADDR	0xC0000000

#define USERMEM_CODEAREA_PROT	(PAGE_PRESENT | PAGE_USER)
#define USERMEM_DATAAREA_PROT	(PAGE_PRESENT | PAGE_WRITE | PAGE_USER)
#define USERMEM_STACKAREA_PROT	USERMEM_DATA_AREA

// Funções para alterar registradores do PC
uint64_t cr0_read(void);
void cr0_write(uint64_t cr0);
uint64_t cr2_read(void);
uint64_t cr3_read(void);
void cr3_write(uint64_t cr3);

// Funções da paginação
int page_init(void);
int page_map(void *vaddr, void *tovaddr, uint16_t tab_prot, uint16_t pg_prot);
void page_unmap(void *vaddr, void *tovaddr);
void page_restore_kernel_pg(void);
void page_dir_exchange(uintptr_t *cr3);
void *brk(int sz);
page_t *page_new();

#define page_free(pd) frame_free((uint64_t)pd >> 12);

#endif
