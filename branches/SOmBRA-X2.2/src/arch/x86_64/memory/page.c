/********************************************************
** Nome: page.c
** Data: 28/05/2011
** Autor: Mauro Joel Schütz
** Descrição: Controle de paginação
********************************************************/
#include <arch.h>
#include <stdio.h>
#include _PAGE_H_
#include _ISR_H_

int page_fault(uregs_t *regs);

int page_map(void *ptr, void *to_ptr, uint16_t tab_prot, uint16_t pg_prot)
{
	uintptr_t p, toptr;
	//frame_t frame;
	uint64_t pml4e, pdpe, pde, pte;
	page_t *pg_pml4, *pg_pdp, *pg_pd, *pg_pt;

	p = (uintptr_t)ptr; toptr = (uintptr_t)to_ptr;
	p &= ~0xFFF;
	toptr = toptr & 0xFFF ? (toptr & ~0xFFF) + 0x1000 : toptr;

	pg_pml4 = PAGE_PML4_TAB_ADDR(KERNEL_PML4E);
	pte = PAGE_ADDR_PTE(p);
	pml4e = pdpe = pde = -1;

	do
	{
		if (!PAGE_ADDR_CHECK_PDE(p, pde))
		{
			if (!PAGE_ADDR_CHECK_PDPE(p, pdpe))
			{
				if (!PAGE_ADDR_CHECK_PML4E(p, pml4e))
				{
					pml4e = PAGE_ADDR_PML4E(ptr);
					pg_pdp = PAGE_PDP_TAB_ADDR(KERNEL_PML4E, pml4e);

					if (!(pg_pml4[pml4e] & 1))
					{
						//if ((frame = frame_get()) == NOFRAMES) return 0;
						pg_pml4[pml4e] = (frame_get() << 0xC) | 1;
						memset(pg_pdp, 0, sizeof(page_t) << 9);
					}
				}

				pdpe = PAGE_ADDR_PDPE(p);
				pg_pd = PAGE_PD_TAB_ADDR(KERNEL_PML4E, pml4e, pdpe);

				if (!(pg_pdp[pdpe] & 1))
				{
					//if ((frame = frame_get()) == NOFRAMES) return 0;
					pg_pdp[pdpe] = (frame_get() << 0xC) | 1;
					memset(pg_pd, 0, sizeof(page_t) << 9);
				}
			}

			pde = PAGE_ADDR_PDE(p);
			pg_pt = PAGE_PT_TAB_ADDR(KERNEL_PML4E, pml4e, pdpe, pde);

			if (!(pg_pd[pde] & 1))
			{
				//if ((frame = frame_get()) == NOFRAMES) return 0;
				pg_pd[pde] = (frame_get() << 0xC) | tab_prot;
				memset(pg_pt, 0, sizeof(page_t) << 9);
			} else
				pg_pd[pde] = pg_pd[pde] | tab_prot;
		}

		if (!(pg_pt[pte] & 1))
		{
			//if ((frame = frame_get()) == NOFRAMES) return 0;
			pg_pt[pte] = (frame_get() << 0xC) | pg_prot;

			if (pg_pt[pte] > 0xFFFFFFFF) panic("Falhou!");
		}
		
		pg_pt[pte] |= 3;

		p += PAGE_PT_SIZE;
		pte = PAGE_ADDR_PTE(p);
	} while (p < toptr);

	return 1;
}

int page_is_clear(page_t *page)
{
	int i;
	int res = 0;

	for (i = 0; i < PAGE_ENTRY_COUNT; i++)
	{
		if (page[i] & 1)
			res++;
	}

	return !res;
}

void page_unmap(void *ptr, void *to_ptr)
{
	uintptr_t p, toptr;
	int pml4e, pdpe, pde, pte;
	page_t *pg_pml4 = 0, *pg_pdp = 0, *pg_pd = 0, *pg_pt = 0;

	p = (uintptr_t)ptr; toptr = (uintptr_t)to_ptr;
	p = p & 0xFFF ? (p & ~0xFFF) + 0x1000 : p;
	toptr = toptr & 0xFFF ? (toptr & ~0xFFF) - 0x1000 : toptr;

	pg_pml4 = PAGE_PML4_TAB_ADDR(KERNEL_PML4E);
	pml4e = pdpe = pde = pte = -1;

	do
	{
		if (!PAGE_ADDR_CHECK_PDE(p, pde))
		{
			pte = PAGE_ADDR_PTE(p);
			pde = PAGE_ADDR_PDE(p);

			if (!PAGE_ADDR_CHECK_PDPE(p, pdpe))
			{
				pdpe = PAGE_ADDR_PDPE(p);

				if (!PAGE_ADDR_CHECK_PML4E(p, pml4e))
				{
					pml4e = PAGE_ADDR_PML4E(p);

					if (!(pg_pml4[pml4e] & 1))
					{
						pml4e = pdpe = pde = -1;
						p &= ~(PAGE_PML4_SIZE-1);
						p += PAGE_PML4_SIZE;
						continue;
					}

					pg_pdp = PAGE_PDP_TAB_ADDR(KERNEL_PML4E, pml4e);
				}

				if (!(pg_pdp[pdpe] & 1))
				{
					pdpe = pde = -1;
					p &= ~(PAGE_PDP_SIZE-1);
					p += PAGE_PDP_SIZE;
					continue;
				}

				pg_pd = PAGE_PD_TAB_ADDR(KERNEL_PML4E, pml4e, pdpe);
			}

			if (!(pg_pd[pde] & 1))
			{
				pde = -1;
				p &= ~(PAGE_PD_SIZE-1);
				p += PAGE_PD_SIZE;
				continue;
			}

			pg_pt = PAGE_PT_TAB_ADDR(KERNEL_PML4E, pml4e, pdpe, pde);
		}

		if (pg_pt[pte] & 1)
		{
			frame_free(pg_pt[pte] >> 0x0C);
			pg_pt[pte] = 0;
			PAGE_REM_ADDR(p);
		}

		if (page_is_clear(pg_pt))
		{
			frame_free(pg_pd[pde] >> 0x0C);
			pg_pd[pde] = 0;
			PAGE_REM_ADDR(pg_pt);
			pde = -1;

			if (page_is_clear(pg_pd))
			{
				frame_free(pg_pdp[pdpe] >> 0x0C);
				pg_pdp[pdpe] = 0;
				PAGE_REM_ADDR(pg_pd);
				pdpe = -1;

				if (page_is_clear(pg_pdp))
				{
					frame_free(pg_pml4[pml4e] >> 0x0C);
					pg_pml4[pml4e] = 0;
					PAGE_REM_ADDR(pg_pdp);
					pml4e = -1;

					p &= ~(PAGE_PML4_SIZE-1);
					p += PAGE_PML4_SIZE;
					continue;
				}

				p &= ~(PAGE_PDP_SIZE-1);
				p += PAGE_PDP_SIZE;
				continue;
			}

			p &= ~(PAGE_PD_SIZE-1);
			p += PAGE_PD_SIZE;
			continue;
		}

		p += PAGE_PT_SIZE;
		pte = PAGE_ADDR_PTE(p);
	} while (p < toptr);
}

void *brk(int sz)
{
    if (!sz)
        return NULL;

    void *nheap = (void *)heap;

    heap += sz;

    if (sz < 0)
    {
        page_unmap((void*)heap, nheap);
        return (void*)heap;
    }

    if (page_map(nheap, (void*)heap, PAGE_PRESENT_WRITE, PAGE_PRESENT_WRITE))
            return nheap;

    heap -= sz;

    return NULL;
}

void page_dir_exchange(uintptr_t *cr3)
{
	uintptr_t pd = cr3_read();

	cr3_write(*cr3);
	*cr3 = pd;
}

page_t *page_new()
{
	page_t *kpg_pml4 = (page_t *)cr3_read();
	page_t *upg_pml4 = (page_t *)(frame_get() << 12);
	
	pagetab_t *vpg = PAGETAB(1022);
	
	kpg_pml4[USER_PML4E] = (uint64_t)upg_pml4 | 1;

	(*pagedir)[1022] = ((unsigned)pg) | PAGE_PRESENT_WRITE;

	memset((*vpg), 0, 4096);

	(*vpg)[0] = (*pagedir)[0];
	(*vpg)[1023] = ((unsigned)pg) | PAGE_PRESENT_WRITE;
	(*pagedir)[1022] = 0;
	

	return upg_pml4;
}

int page_init(void)
{
    if (frame_init())
        return 1;

    printf("Iniciando a gerencia da paginacao...\n");

    isr_vector_t page_vec = { 0x8E, page_fault };
    isr_set_handler(14, &page_vec);

	// Pegando o pml4 atual e direcionando para a ultima entrada!
	page_t *pg_pml4 = (page_t *)cr3_read();
	pg_pml4[KERNEL_PML4E] = (uint64_t)pg_pml4 | 1;
	cr3_write((uint64_t)pg_pml4);

	return 0;
}

int page_fault(uregs_t *regs)
{
    uintptr_t faulting_address = cr2_read();

    // Calcula o endereço inicial
	int pml4e = PAGE_ADDR_PML4E(faulting_address);
	int pdpe = PAGE_ADDR_PDPE(faulting_address);
    int pde = PAGE_ADDR_PDE(faulting_address);
    int pte = PAGE_ADDR_PTE(faulting_address);

	page_t *pg_pml4 = PAGE_PML4_TAB_ADDR(KERNEL_PML4E);
	page_t *pg_pdp = PAGE_PDP_TAB_ADDR(KERNEL_PML4E, pml4e);
	page_t *pg_pd = PAGE_PD_TAB_ADDR(KERNEL_PML4E, pml4e, pdpe);
    page_t *pg_pt = PAGE_PT_TAB_ADDR(KERNEL_PML4E, pml4e, pdpe, pde);

    // The error code gives us details of what happened.
    int present   = !(regs->err_code & 0x1); // Page not present
    int rw = regs->err_code & 0x2;           // Write operation?
    int us = regs->err_code & 0x4;           // Processor was in user-mode?
    int reserved = regs->err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    int id = regs->err_code & 0x10;          // Caused by an instruction fetch?

    // Output an error message.
    printf("Page fault! ( ");
    if (present) {printf("present ");}
    if (rw) {printf("read-only ");}
    if (us) {printf("user-mode ");}
    if (reserved) {printf("reserved ");}
    if (id) printf("instruction fetch ");
    printf(") at 0x%lX on pml4 %lX\n", faulting_address, cr3_read());

/*    if (us) {
		int uvirt_to_kvirt2 = 0;
		if (task_curr->as != NULL)
			uvirt_to_kvirt2 = (unsigned)task_curr->as->user_mem - task_curr->as->virt_adr;
		printf("EIP: 0x%08X => 0x%08X\n", regs->eip, regs->eip + uvirt_to_kvirt2);
		dump_eip(regs->eip + uvirt_to_kvirt2, -11, 10); taskexit(-1); return 0; 
    }*/

    panic("Page fault: pml4[%u]: %lX | pdp[%u]: %lX | pd[%u]: %lX | pt[%u]: %lX",
			pml4e, pg_pml4[pml4e], pdpe, pg_pdp[pdpe], pde, pg_pd[pde], pte, pg_pt[pte]);

    return 0;
}

