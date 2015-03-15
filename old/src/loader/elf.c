#include <elf.h>

void *memset(void *dest, char val, unsigned count);
void *memcpy(void *dest, const void *src, unsigned count);

void prints(char *s);

int elf64_is_valid(void *image)
{
	Elf_Ehdr64 *elf = (Elf_Ehdr64 *)image;

	if(!ELF_MAG_CHECK(elf->e_ident)) /* "ELF" */
		return 0;

	if(!ISELF64(elf) ||
		!ELF_ISVERSION(elf->e_ident) ||	/* 1=current ELF spec */
		elf->e_machine != EM_X86_64 ||	/* 3=AMD64 */
		elf->e_version != EV_CURRENT)
			return 0;

	return 1;
}

int elf64_load(void *image, Elf_Addr64 *entry)
{
	Elf_Phdr64 *phdr;
	Elf_Ehdr64 *elf = (Elf_Ehdr64 *)image;
	int i;

/* validate ELF headers */
	if (!elf64_is_valid(image))
		return 1;

	if (elf->e_type != ET_EXEC)
		return 1;

/* get entry point */
	 elf->e_entry &= 0x7FFFFFFF;
	*entry = elf->e_entry;

/* seek to program headers (segments) */
	phdr = (Elf_Phdr64 *)((char *)elf + elf->e_phoff);

	/* load all programs in this file */
	for (i = 0; i < elf->e_phnum; i++, phdr++)
	{
		if (phdr->p_type != PT_LOAD)	/* only loadable types */
			continue;

		phdr->p_offset &= 0x7FFFFFFF;
		phdr->p_vaddr &= 0x7FFFFFFF;
		
		if (phdr->p_filesz > 0)
			memcpy((void *)(int)phdr->p_vaddr, (const void *)(int)(image + phdr->p_offset), phdr->p_filesz);

	/* if in-memory size is larger, zero out the difference */
		if (phdr->p_memsz > phdr->p_filesz)
			memset((void *)(int)(phdr->p_vaddr + phdr->p_filesz), 0, phdr->p_memsz - phdr->p_filesz);
	}
	
	return 0;
}