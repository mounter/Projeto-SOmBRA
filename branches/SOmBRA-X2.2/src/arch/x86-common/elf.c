/*****************************************************************************
**																			**
**	Arquivo: elf.c															**
**	Descrição: Suporte a 'Executable and Linking Format (ELF)'.				**
**	Autor: Leonardo Monteiro Silva (gnomo)									**
**	Email: gnomo86 at gmail.com												**
**	Data: 19/12/2008														**
**	Notas: Este arquivo já existia anteriormente, porém sem					**
**	assinaturas e possuindo apenas a função load_elf_exec.					**
**																			**
** Modificação:																**
**																			**
**	Descrição: Adição a suporte a Elf64 e melhoria de código.				**
**	Autor: Mauro Joel Schütz (Mounter)										**
**	Email: maurojoel at gmail.com											**
**	Data: 16/08/2012														**
**                                                                          **
*****************************************************************************/

#include <x86-common/elf.h>
#include <string.h>
#include <stdio.h>
#include <kernel.h>
#include <kernel/aspace.h>

int elf32_is_valid(void *image)
{
	Elf_Ehdr32 *elf = (Elf_Ehdr32 *)image;

	if(!ELF_MAG_CHECK(elf->e_ident)) /* "ELF" */
		return 0;

	if(!ISELF32(elf) ||
		!ELF_ISVERSION(elf->e_ident) ||	/* 1=current ELF spec */
		elf->e_machine != EM_386 ||
		elf->e_version != EV_CURRENT)
			return 0;

	return 1;
}

int elf32_load_exec(char *image, uintptr_t *entry, aspace_t *as)
{
	Elf_Ehdr32 *elf = (Elf_Ehdr32 *)image;
	Elf_Phdr32 *seg;
	int i, err;

	// Validando executavel ELF32
	if (!elf32_is_valid(image))
		return 1;
		
	if (elf->e_type != ET_EXEC)
		return 1;

/* get entry point */
	(*entry) = elf->e_entry;
	
/* seek to program headers (segments) */
	image += elf->e_phoff;
	for(i = 0; i < elf->e_phnum; i++)
	{
		seg = (Elf_Phdr32 *)(image + elf->e_phentsize * i);
		
/* choke on 2=DYNAMIC and the forbidden 5=SHLIB segments */
		if(seg->p_type == PT_DYNAMIC || seg->p_type == PT_SHLIB)
			return 0;
/* handle 1=LOAD segment */
		else if(seg->p_type == PT_LOAD)
		{
			err = aspace_section_add(as, seg->p_vaddr, seg->p_filesz,
				SF_LOAD | (seg->p_flags & PF_RWX), seg->p_offset);
			if(err)
				return err;
/* if size-in-mem > size-on-disk, this segment contains the BSS */
			if(seg->p_memsz > seg->p_filesz)
			{
				err = aspace_section_add(as,
					seg->p_vaddr + seg->p_filesz,
					seg->p_memsz - seg->p_filesz,
					SF_ZERO | (seg->p_flags & PF_RWX),
					seg->p_offset);
				if(err)
					return err;
			}
		}
/* ignore 0=NULL, 6=PHDR, 3=INTERP, and 4=NOTE segments
		else
			nothing; */
	}
	return 0;
}

int load_elf_exec(char *image, uintptr_t *entry, aspace_t *as)
{
	#ifdef __x86_64__
	return elf64_load_exec(image, entry, as);
	#else
	return elf32_load_exec(image, entry, as);
	#endif
}

// Encontra seção de nome 'sect_name'.
uint8_t	elf32_get_section_by_name(void* image, char* sect_name, Elf_Shdr32** sect)
{
	Elf_Ehdr32	*elf = (Elf_Ehdr32*)image;
	Elf_Shdr32	*sect_table, *str_sect;
	int8_t		*strings;
	uint32_t	i, index;

	// str_sect contem os nomes das seções.
	str_sect = (Elf_Shdr32*)(image + elf->e_shoff + (elf->e_shentsize * elf->e_shstrndx));
	if(str_sect->sh_type != SHT_STRTAB)
		return 0;
		
	strings = (int8_t*)(str_sect->sh_offset + image);
	//printf("%s", strings);

	sect_table = (Elf_Shdr32*)(image + elf->e_shoff);
	for(i = 0; i < elf->e_shnum; i++)
	{
		// Pega o índice do nome da seção atual.
		index = ((Elf_Shdr32*)(image + elf->e_shoff + (elf->e_shentsize * i)))->sh_name;
		if(!strcmp(strings + index , sect_name))
		{
			*sect = (Elf_Shdr32*)(image + elf->e_shoff + (elf->e_shentsize * i));
			return 1;
		}
	}

	return 0;
}

/*
	Retorna número de seções do 'type' e 'flags' requerido.
	Se 'flags' < 0 ele será ignorado.
*/
int32_t	elf32_get_sections(void* image, uint32_t type, int32_t flags)
{
	Elf_Ehdr32*	elf = (Elf_Ehdr32*)image;
	Elf_Shdr32*	tmp;
	uint32_t	i;
	int32_t		retval = 0;

	for(i = 0; i < elf->e_shnum; i++)
	{
		tmp = (Elf_Shdr32*)(image + elf->e_shoff + (elf->e_shentsize * i));
		if( tmp->sh_type == type)
		{
			if(flags >= 0)
				if(tmp->sh_flags != (uint32_t)(flags))
					return 0;

			retval++;
		}
	}

	return retval;
}

/*
	Atribui à 'sect' a i-ésima ocorrencia de seção do 'type' e 'flags' requerido.
	Se 'flags' < 0 ele será ignorado.
*/
uint8_t	elf32_get_sections_i(void* image, uint32_t index, uint32_t type, int32_t flags, Elf_Shdr32** sect)
{
	Elf_Ehdr32*	elf = (Elf_Ehdr32*)image;
	Elf_Shdr32*	tmp;
	uint32_t	i;

	for(i = 0; i < elf->e_shnum; i++)
	{
		tmp = (Elf_Shdr32*)(image + elf->e_shoff + (elf->e_shentsize * i));
		if( tmp->sh_type == type)
		{
			if(flags >= 0)
			if(tmp->sh_flags != (uint32_t)(flags))
				return 0;

			if(!index)
			{
				*sect = tmp;
				return 1;
			}
			index--;
		}
	}

	return 0;
}

uint8_t	elf32_get_section_i(void* image, uint32_t index, Elf_Shdr32** sect)
{
	Elf_Ehdr32*	elf = (Elf_Ehdr32*)image;
	Elf_Shdr32*	tmp;

	if(elf->e_shnum < index)
		return 0;

	tmp = (Elf_Shdr32*)(image + elf->e_shoff + elf->e_shentsize * index);
	if(sect != NULL)
		*sect = tmp;

	return 1;
}

uint8_t	elf32_get_symbol_i(void* image, uint32_t sym_idx, Elf_Shdr32* symtab, Elf_Sym32** symbol)
{
	if(symtab->sh_type != SHT_SYMTAB)
		return 0;

	if(sym_idx > symtab->sh_size / symtab->sh_entsize)
		return 0;

	*symbol = (Elf_Sym32*)(image + symtab->sh_offset + symtab->sh_entsize * sym_idx);
	return 1;
}

uint8_t	elf32_get_symbol_by_name(void* image, const char* sym_name, int32_t type, Elf_Shdr32* symtab, Elf_Shdr32* strtab, Elf_Sym32** symbol)
{
	Elf_Sym32	*sym;
	uint32_t	i;
	int8_t		*strings;

	/* Verifica se as seções são 'Symbol Table' e 'String Table' */
	if(symtab->sh_type != SHT_SYMTAB || strtab->sh_type != SHT_STRTAB)
		return 0;

	strings = (int8_t*)(image + strtab->sh_offset);

	for(i = 0; i < (symtab->sh_size / symtab->sh_entsize); i++)
	{
		sym = (Elf_Sym32*)(image + symtab->sh_offset + i * symtab->sh_entsize);
		if(!strcmp(strings + sym->st_name, (char*)sym_name))
		{
			if(type > 0 && ELF_ST_TYPE(sym->st_info) != type)
				return 0;

			if(symbol != NULL)
				*symbol = sym;

			return 1;
		}
	}

	return 0;
}

/*
	Procura por um símbolo percorrendo todas as tabelas de símbolos.
	função parecida com elf32_get_symbol_by_name() porém mais genérica.
*/
uint8_t	elf32_search_symbol(void* image, const char* name, uint32_t type, Elf_Sym32** symbol)
{
	Elf_Shdr32	*sect, *strtab;
	uint32_t	i;

	for(i = 0; i < elf32_get_sections(image, SHT_SYMTAB, -1); i++)	// pesquisa em todas tabelas de símbolos.
	{
		if(elf32_get_sections_i(image, i, SHT_SYMTAB, -1, &sect))	// carrega symtab #i
		if(elf32_get_section_i(image, sect->sh_link, &strtab))		// carrega strtab correspondente.
		if(elf32_get_symbol_by_name(image,name,type, sect, strtab, symbol))	// procura pelo símbolo.
			return 1;
	}
	return 0;
}

uint8_t	elf32_get_name(void* image, uint32_t index, Elf_Shdr32* strtab, const char** name)
{
	int8_t*	strings;

	if(strtab->sh_type != SHT_STRTAB)
		return 0;

	if(index > strtab->sh_size)
		return 0;

	strings = (int8_t*)(image + strtab->sh_offset);

	*name = (int8_t*)(strings + index);

	return 1;
}

// Aplica relocalização referenciada por membro de seção SHT_REL ou SHT_RELA.
uint8_t	elf32_reloc_apply(void* image, Elf_Rela32* rel, uint32_t symtab_idx, uint32_t t_sect_idx)
{
	Elf_Shdr32	*symtab, *sect, *sect2;
	Elf_Sym32	*symbol;
	uint32_t	*rel_val, sym_val;

	if (ELF32_R_TYPE(rel->r_info) == 0)
		return RELOC_OK;

	if(!elf32_get_section_i(image, symtab_idx, &symtab))	// carrega 'Symbol Table' associada à 'Relocation Table'.
		return RELOC_ERROR_SYMTAB;

	if(!elf32_get_section_i(image, t_sect_idx, &sect))	// carrega seção alvo da relocalização.
		return RELOC_ERROR_TARGET;

	//printf("symtab.type=%d tsect.type=%d symbol.sect=%d symidx=%d\n", symtab->type & 0xff, sect->type & 0xff, symbol->section & 0xffff, symtab_idx);

	if(ELF32_R_SYM(rel->r_info) == 0)	// Referência: (If the index is STN_UNDEF, the undefined symbol index, the relocation uses 0 as the "symbol value.")
	{
		sym_val = 0;
		goto	ignore_symbol;
	}
	else
	{
		if(!elf32_get_symbol_i(image, ELF32_R_SYM(rel->r_info), symtab, &symbol))	// carrega o símbolo a ser relocalizado.
			return RELOC_ERROR_SYMBOL;

//rel_retry:
		switch(symbol->st_shndx)
		{
			case	SHN_COMMON:
				return	RELOC_COMMON;
			case	SHN_ABS:
			case	SHN_SOMBRA:
				sym_val = (uint32_t)(symbol->st_value);// + image);
				break;

			default:
			if(!elf32_get_section_i(image, symbol->st_shndx, &sect2))
				return RELOC_ERROR_SYMBOL;
			else
				sym_val = (uint32_t)(symbol->st_value);
			break;
		}
	}

	if(symbol->st_shndx != 0)
	{

ignore_symbol:
		rel_val = (uint32_t*)(image + sect->sh_offset + rel->r_offset);	// Endereço da relocalização
		if(symbol->st_shndx == SHN_SOMBRA)
			*rel_val = (uint32_t)sym_val + *rel_val;
		else
			*rel_val = (uint32_t)((uintptr_t)(image + sym_val + sect2->sh_offset) + *rel_val);

		switch(ELF32_R_TYPE(rel->r_info))
		{
			case R_386_32:
				break;
			case R_386_PC32:
				*rel_val -= (uintptr_t)rel_val;
				break;

			default:
				printf("Tipo desconhecido de relocalizacao em 'ELF Relocatable'.\n");
				return RELOC_ERROR;
				break;
		}
		return RELOC_OK;
	}
	else
	{
			return RELOC_UNDEF_SYMBOL;
	}

	return RELOC_OK;
}

// Implementação das versões de 64 bit

int elf64_is_valid(void *image)
{
	Elf_Ehdr64 *elf = (Elf_Ehdr64 *)image;

	if(!ELF_MAG_CHECK(elf->e_ident)) /* "ELF" */
		return 0;

	if(!ISELF64(elf) ||
		!ELF_ISVERSION(elf->e_ident) ||	/* 1=current ELF spec */
		elf->e_machine != EM_X86_64 ||
		elf->e_version != EV_CURRENT)
			return 0;

	return 1;
}

int elf64_load_exec(char *image, uintptr_t *entry, aspace_t *as)
{
	Elf_Ehdr64 *elf = (Elf_Ehdr64 *)image;
	Elf_Phdr64 *seg;
	int i, err;

	// Validando executavel ELF32
	if (!elf64_is_valid(image))
		return 1;
		
	if (elf->e_type != ET_EXEC)
		return 1;

/* get entry point */
	(*entry) = elf->e_entry;
	
/* seek to program headers (segments) */
	image += elf->e_phoff;
	for(i = 0; i < elf->e_phnum; i++)
	{
		seg = (Elf_Phdr64 *)(image + elf->e_phentsize * i);
		
/* choke on 2=DYNAMIC and the forbidden 5=SHLIB segments */
		if(seg->p_type == PT_DYNAMIC || seg->p_type == PT_SHLIB)
			return 0;
/* handle 1=LOAD segment */
		else if(seg->p_type == PT_LOAD)
		{
			err = aspace_section_add(as, seg->p_vaddr, seg->p_filesz,
				SF_LOAD | (seg->p_flags & PF_RWX), seg->p_offset);
			if(err)
				return err;
/* if size-in-mem > size-on-disk, this segment contains the BSS */
			if(seg->p_memsz > seg->p_filesz)
			{
				err = aspace_section_add(as,
					seg->p_vaddr + seg->p_filesz,
					seg->p_memsz - seg->p_filesz,
					SF_ZERO | (seg->p_flags & PF_RWX),
					seg->p_offset);
				if(err)
					return err;
			}
		}
/* ignore 0=NULL, 6=PHDR, 3=INTERP, and 4=NOTE segments
		else
			nothing; */
	}
	return 0;
}

uint8_t	elf64_get_section_by_name(void* image, char* sect_name, Elf_Shdr64** sect)
{
	Elf_Ehdr64	*elf = (Elf_Ehdr64*)image;
	Elf_Shdr64	*sect_table, *str_sect;
	int8_t		*strings;
	uint64_t	i, index;

	// str_sect contem os nomes das seções.
	str_sect = (Elf_Shdr64*)(image + elf->e_shoff + (elf->e_shentsize * elf->e_shstrndx));
	if(str_sect->sh_type != SHT_STRTAB)
		return 0;
		
	strings = (int8_t*)(str_sect->sh_offset + image);
	//printf("%s", strings);

	sect_table = (Elf_Shdr64*)(image + elf->e_shoff);
	for(i = 0; i < elf->e_shnum; i++)
	{
		// Pega o índice do nome da seção atual.
		index = ((Elf_Shdr64*)(image + elf->e_shoff + (elf->e_shentsize * i)))->sh_name;
		if(!strcmp(strings + index , sect_name))
		{
			*sect = (Elf_Shdr64*)(image + elf->e_shoff + (elf->e_shentsize * i));
			return 1;
		}
	}

	return 0;
}

/*
	Retorna número de seções do 'type' e 'flags' requerido.
	Se 'flags' < 0 ele será ignorado.
*/
int32_t	elf64_get_sections(void* image, uint32_t type, int32_t flags)
{
	Elf_Ehdr64*	elf = (Elf_Ehdr64*)image;
	Elf_Shdr64*	tmp;
	uint32_t	i;
	int32_t		retval = 0;

	for(i = 0; i < elf->e_shnum; i++)
	{
		tmp = (Elf_Shdr64*)(image + elf->e_shoff + (elf->e_shentsize * i));
		if( tmp->sh_type == type)
		{
			if(flags >= 0)
				if(tmp->sh_flags != (uint32_t)(flags))
					return 0;

			retval++;
		}
	}

	return retval;
}

/*
	Atribui à 'sect' a i-ésima ocorrencia de seção do 'type' e 'flags' requerido.
	Se 'flags' < 0 ele será ignorado.
*/
uint8_t	elf64_get_sections_i(void* image, uint32_t index, uint32_t type, int32_t flags, Elf_Shdr64** sect)
{
	Elf_Ehdr64*	elf = (Elf_Ehdr64*)image;
	Elf_Shdr64*	tmp;
	uint32_t	i;

	for(i = 0; i < elf->e_shnum; i++)
	{
		tmp = (Elf_Shdr64*)(image + elf->e_shoff + (elf->e_shentsize * i));
		if( tmp->sh_type == type)
		{
			if(flags >= 0)
			if(tmp->sh_flags != (uint32_t)(flags))
				return 0;

			if(!index)
			{
				*sect = tmp;
				return 1;
			}
			index--;
		}
	}

	return 0;
}

uint8_t	elf64_get_section_i(void* image, uint32_t index, Elf_Shdr64** sect)
{
	Elf_Ehdr64*	elf = (Elf_Ehdr64*)image;
	Elf_Shdr64*	tmp;

	if(elf->e_shnum < index)
		return 0;

	tmp = (Elf_Shdr64*)(image + elf->e_shoff + elf->e_shentsize * index);
	if(sect != NULL)
		*sect = tmp;

	return 1;
}

uint8_t	elf64_get_symbol_i(void* image, uint32_t sym_idx, Elf_Shdr64* symtab, Elf_Sym64** symbol)
{
	if(symtab->sh_type != SHT_SYMTAB)
		return 0;

	if(sym_idx > symtab->sh_size / symtab->sh_entsize)
		return 0;

	*symbol = (Elf_Sym64*)(image + symtab->sh_offset + symtab->sh_entsize * sym_idx);
	return 1;
}

uint8_t	elf64_get_symbol_by_name(void* image, const char* sym_name, int32_t type, Elf_Shdr64* symtab, Elf_Shdr64* strtab, Elf_Sym64** symbol)
{
	Elf_Sym64	*sym;
	uint32_t	i;
	int8_t		*strings;

	/* Verifica se as seções são 'Symbol Table' e 'String Table' */
	if(symtab->sh_type != SHT_SYMTAB || strtab->sh_type != SHT_STRTAB)
		return 0;

	strings = (int8_t*)(image + strtab->sh_offset);

	for(i = 0; i < (symtab->sh_size / symtab->sh_entsize); i++)
	{
		sym = (Elf_Sym64*)(image + symtab->sh_offset + i * symtab->sh_entsize);
		if(!strcmp(strings + sym->st_name, (char*)sym_name))
		{
			if(type > 0 && ELF_ST_TYPE(sym->st_info) != type)
				return 0;

			if(symbol != NULL)
				*symbol = sym;

			return 1;
		}
	}

	return 0;
}

/*
	Procura por um símbolo percorrendo todas as tabelas de símbolos.
	função parecida com elf32_get_symbol_by_name() porém mais genérica.
*/
uint8_t	elf64_search_symbol(void* image, const char* name, uint32_t type, Elf_Sym64** symbol)
{
	Elf_Shdr64	*sect, *strtab;
	uint32_t	i;

	for(i = 0; i < elf64_get_sections(image, SHT_SYMTAB, -1); i++)	// pesquisa em todas tabelas de símbolos.
	{
		if(elf64_get_sections_i(image, i, SHT_SYMTAB, -1, &sect))	// carrega symtab #i
		if(elf64_get_section_i(image, sect->sh_link, &strtab))		// carrega strtab correspondente.
		if(elf64_get_symbol_by_name(image,name,type, sect, strtab, symbol))	// procura pelo símbolo.
			return 1;
	}
	return 0;
}

uint8_t	elf64_get_name(void* image, uint32_t index, Elf_Shdr64* strtab, const char** name)
{
	int8_t*	strings;

	if(strtab->sh_type != SHT_STRTAB)
		return 0;

	if(index > strtab->sh_size)
		return 0;

	strings = (int8_t*)(image + strtab->sh_offset);

	*name = (int8_t*)(strings + index);

	return 1;
}