#include <x86/page.h>
#include <x86/regs.h>
#include <stdio.h>
#include <string.h>

static unsigned kernel_pg[PG_KERNEL_LIMIT+1];	// Última entrada como sendo o pagedir alocado para o kernel
pagedir_t *pagedir/* __attribute__ ((aligned (4096)))*/;

int page_init(void)
{
    if (frame_init())
        return 1;

    printf("Iniciando a gerencia da paginacao...\n");

    pagetab_t *pagetab;

    isr_vector_t page_vec = { 0x8E, page_fault };

    isr_set_handler(14, &page_vec);

    pagedir = (pagedir_t *)(frame_get() << 12);
    pagetab = (pagetab_t *)(frame_get() << 12);

    memset(kernel_pg, 0, PG_KERNEL_LIMIT+1);

    printf("pagedir[ADDR 0x%08X] | pagetab[ADDR 0x%08X]\n", pagedir, pagetab);

    int i;
    for (i = 0; i < 256; i++)
        (*pagetab)[i] = (i << 12) | PAGE_PRESENT_WRITE;
    for (i = 256; i < HEAP_FRAMES; i++)
        (*pagetab)[i] = (i << 12) | PAGE_PRESENT_WRITE;

    for (i = 0; i < 1024; i++)
        (*pagedir)[i] = 0;

    (*pagedir)[0] = ((unsigned)pagetab) | PAGE_PRESENT_WRITE;
    kernel_pg[0] = (*pagedir)[0];
    kernel_pg[PG_KERNEL_LIMIT] = (unsigned)pagedir;
    (*pagedir)[1023] = ((unsigned)pagedir) | PAGE_PRESENT_WRITE;

    cr3_write((unsigned)pagedir);
    cr0_write(cr0_read() | 0x80000000);

    pagedir = PAGEDIR_PTR;

    return 0;
}

void page_restore_kernel_pg(void)
{
	cr3_write(kernel_pg[PG_KERNEL_LIMIT]);
}


unsigned page_frames_needed(void *addr0, void *addr1)
{
    unsigned short pt_index, pd_index,
                   pt_toindex, pd_toindex;

// Calcula o endereço inicial
    pd_index = PAGE_GET_DIR(addr0);
    pt_index = PAGE_GET_TAB(addr0);

// Calcula o endereço final
    pd_toindex = PAGE_GET_DIR(addr1);
    pt_toindex = PAGE_GET_TAB(addr1);

    int pds = pd_toindex - pd_index;
    int tables = 1024 - pt_index + pt_toindex;

    return (pds-1) * 1024 + tables + pds + 1;
}

int page_map(void *vaddr, void *tovaddr, uint16_t tab_prot, uint16_t pg_prot)
{
#ifdef DEBUG
    printf("PAGEMAP(%X, %X, %X, %X, %X)\n", vaddr, tovaddr, tab_prot, pg_prot);
#endif
    int i, j;
    unsigned short pt_index, pd_index,
                   pt_toindex, pd_toindex;
    pagetab_t *pagetab;

// Calcula o endereço inicial
    pd_index = PAGE_GET_DIR(vaddr);
    pt_index = PAGE_GET_TAB(vaddr);

// Calcula o endereço final
    pd_toindex = PAGE_GET_DIR(tovaddr);
    pt_toindex = PAGE_GET_TAB(tovaddr);

// Arredonda caso seja necessário
    if (((unsigned)tovaddr) & 0xFFF)
    {
        pt_toindex++;

        if (pt_toindex >= 1024)
        {
            pd_toindex++;
            pt_toindex = 0;
        }
    }

// Checa a quantidade máxima de frames necessária, caso não exista frames
// livres, retornamos como falso.
    if (page_frames_needed(vaddr, tovaddr) >= frame_free_count())
        return 0;

// Mapeando as tabelas e as paginas como necessário
    for (i = pd_index; i <= pd_toindex; i++)
    {
	if ((i < PG_KERNEL_LIMIT) && (kernel_pg[i] & PAGE_PRESENT))
	{
		(*pagedir)[i] = kernel_pg[i];
#ifdef DEBUG
		printf("K-CR3[0x%08X]:PAGEDIR[%u]: 0x%08X\n", cr3_read(), i, (*pagedir)[i]);
#endif
	}
	else if (!((*pagedir)[i] & PAGE_PRESENT))
	{
            (*pagedir)[i] = (frame_get() << 12) | tab_prot;
#ifdef DEBUG
		 printf("CR3[0x%08X]:PAGEDIR[%u]: 0x%08X\n", cr3_read(), i, (*pagedir)[i]);
#endif

	    if (i < PG_KERNEL_LIMIT)
		kernel_pg[i] = (*pagedir)[i];
	}

        pagetab = PAGETAB(i);

        for (j = (i == pd_index) ? pt_index : 0; j < ((i >= pd_toindex) ? pt_toindex : 1024); j++)
	{
             if (!((*pagetab)[j] & PAGE_PRESENT))
                (*pagetab)[j] = (frame_get() << 12) | pg_prot;
#ifdef DEBUG
		printf("CR3[0x%08X]:PAGETAB[%u]: 0x%08X\n", cr3_read(), j, (*pagetab)[j]);
#endif
	}
    }

// OK. Memoria expandida
    return 1;
}

void page_unmap(void *vaddr, void *tovaddr)
{
    int i, j;
    unsigned short pt_index, pd_index,
                   pt_toindex, pd_toindex;
    pagetab_t   *pagetab;

// Calcula o endereço inicial
    pd_index = PAGE_GET_DIR(vaddr);
    pt_index = PAGE_GET_TAB(vaddr);

// Calcula o endereço final
    pd_toindex = PAGE_GET_DIR(tovaddr);
    pt_toindex = PAGE_GET_TAB(tovaddr);

// Arredonda caso seja necessário
    if ((unsigned)vaddr & 0xFFF)
    {
        pt_index++;

        if (pt_index >= 1024)
        {
            pd_index++;
            pt_index = 0;
        }
    }

    // Arredonda caso seja necessário
    if ((unsigned)tovaddr & 0xFFF)
    {
        pt_toindex++;

        if (pt_toindex >= 1024)
        {
            pd_toindex++;
            pt_toindex = 0;
        }
    }

    // Mapeando as tabelas e as paginas como necessário
    for (i = pd_index; i <= pd_toindex; i++)
    {
        if ((*pagedir)[i] & PAGE_PRESENT)
        {
            pagetab = PAGETAB(i);

            for (j = (i == pd_index) ? pt_index : 0; j < ((i >= pd_toindex) ? pt_toindex : 1024); j++)
            {
                if ((*pagetab)[j] & PAGE_PRESENT)
                {
#ifdef DEBUG
                    printf("FREE[t:%u:%u]: 0x%08X\n", i, j, (*pagetab)[j]);
#endif
                    frame_free((*pagetab)[j] >> 12);
                    (*pagetab)[j] = 0;

                    // Tirando a página do buffer...
                    __asm__ volatile("invlpg %0"::"m" (*(char *)((i << 22) + (j << 12))));
                }
            }

            if ((i == pd_index) && (pd_index == PAGE_GET_DIR(vaddr)))
                continue;
#ifdef DEBUG
            printf("FREE[d:%u]: 0x%08X\n", i, (*pagedir)[i]);
#endif
            frame_free((*pagedir)[i] >> 12);
            (*pagedir)[i] = 0;

	    if (i < PG_KERNEL_LIMIT)
		kernel_pg[i] = 0;
        }
    }
}

void page_dir_exchange(unsigned *cr3)
{
	unsigned pd = cr3_read();

	cr3_write(*cr3);
	*cr3 = pd;
}

pagedir_t *page_new()
{
	pagedir_t *pg = (pagedir_t *)(frame_get() << 12);
	pagetab_t *vpg = PAGETAB(1022);

	(*pagedir)[1022] = ((unsigned)pg) | PAGE_PRESENT_WRITE;

	memset((*vpg), 0, 4096);

	(*vpg)[0] = (*pagedir)[0];
	(*vpg)[1023] = ((unsigned)pg) | PAGE_PRESENT_WRITE;
	(*pagedir)[1022] = 0;

	return pg;
}

void *brk(int sz)
{
    if (!sz)
        return NULL;

    void *nheap = (void *)heap;

    heap += sz;

#ifdef DEBUG
    printf("HEAP: %X | NHEAP: %X\n", heap, nheap);
#endif
    if (sz < 0)
    {
        page_unmap((void*)heap, nheap);
        return (void*)heap;
    }

    if (page_map(nheap, (void*)heap, PAGE_PRESENT_WRITE, PAGE_KERNEL_PGPROT))
            return nheap;

    heap -= sz;

    return NULL;
}

int page_fault(uregs_t *regs)
{
    unsigned faulting_address;
    faulting_address = cr2_read();

        // Calcula o endereço inicial
    unsigned pd_index = PAGE_GET_DIR(faulting_address);
    unsigned pt_index = PAGE_GET_TAB(faulting_address);

    pagetab_t   *pagetab = PAGETAB(pd_index);

    // The error code gives us details of what happened.
    int present   = !(regs->err_code & 0x1); // Page not present
    int rw = regs->err_code & 0x2;           // Write operation?
    int us = regs->err_code & 0x4;           // Processor was in user-mode?
    int reserved = regs->err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    int id = regs->err_code & 0x10;          // Caused by an instruction fetch?

    // Checa se há uma pagetab do kernel armazenada
    if (!us && (pd_index < PG_KERNEL_LIMIT) && (kernel_pg[pd_index] & PAGE_PRESENT))
    {
	(*pagedir)[pd_index] = kernel_pg[pd_index];
	return 0;
    }

    // Output an error message.
    printf("Page fault! ( ");
    if (present) {printf("present ");}
    if (rw) {printf("read-only ");}
    if (us) {printf("user-mode ");}
    if (reserved) {printf("reserved ");}
    if (id) printf("instruction fetch ");
    printf(") at 0x%X on page dir %X\n", faulting_address, cr3_read());

    panic("Page fault: pagedir[%u]: %X | pagetab[%u]: %X", pd_index, (*pagedir)[pd_index], pt_index, (*pagetab)[pt_index]);

    return 0;
}
