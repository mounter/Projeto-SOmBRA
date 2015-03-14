#include <stdint.h>
#include <elf.h>
#include <kernel/multiboot.h>

int elf64_load(void *image, Elf_Addr64 *entry);

void goto_addr(unsigned addr);
int have_longmode(void);
void enter_compatibility_mode(void);
void install_pae();

char *video_mem = (char*)0xB8000;
int pos = 0;

void printc(char c)
{
	video_mem[pos++] = c;
	video_mem[pos++] = 0x0F;
}

void prints(char *s)
{
	while (*s)
		printc(*(s++));
}

void loader_main(void)
{
	if (have_longmode())
	{
		module_t *mod = (module_t *)(multiboot_info->mods_addr);
		Elf_Addr64 entry;
		
		
		prints("(x86_64) We have long mode! :)");
		install_pae();
		enter_compatibility_mode();
		
		if (elf64_load((void*)mod->mod_start, &entry))
		{
			prints(" erro ao carregar!");
			return;
		}
		
		prints(" here we gooo!");
		goto_addr((unsigned)entry);
	}
	else
		prints("(x86) No long mode, sorry! :(");
}

// PAE Paging


uint64_t page_map_level_4[512] __attribute__((aligned(0x1000)));
uint64_t page_dir_ptr_tab[512] __attribute__((aligned(0x1000))); // must be aligned to (at least)0x20, ...
    // ... turning out that you can put more of them into one page, saving memory

// 512 entries
uint64_t page_dir[512] __attribute__((aligned(0x1000)));  // must be aligned to page boundary
uint64_t page_tab[512] __attribute__((aligned(0x1000)));
uint64_t page_tab2[512] __attribute__((aligned(0x1000)));

void install_pae()
{
	page_map_level_4[0] = (int)&page_dir_ptr_tab | 1;
	page_dir_ptr_tab[0] = (int)&page_dir | 1; // set the page directory into the PDPT and mark it present
	page_dir[0] = (int)&page_tab | 3; //set the page table into the PD and mark it present/writable
	page_dir[1] = (int)&page_tab2 | 3; //set the page table into the PD and mark it present/writable

	unsigned int i, address = 0;
	for(i = 0; i < 512; i++)
	{
		page_tab[i] = address | 3; // map address and mark it present/writable
		address = address + 0x1000;
	}
	
	for (i = 0; i < 512; i++)
	{
		page_tab2[i] = address | 3; // map address and mark it present/writable
		address = address + 0x1000;
	}
	
	/*for (i = 2; i < 512; i++)
		page_dir[i] = 0;
		
	for (i = 1; i < 512; i++)
	{
		page_dir_ptr_tab[i] = 0;
		page_map_level_4[i] = 0;
	}*/
		

	asm volatile ("movl %cr4, %eax; bts $5, %eax; movl %eax, %cr4"); // set bit5 in CR4 to enable PAE		 
	asm volatile ("movl %%eax, %%cr3" :: "a" (&page_map_level_4)); // load PDPT into CR3
	//asm volatile ("movl %cr0, %eax; orl $0x80000000, %eax; movl %eax, %cr0;");
}

