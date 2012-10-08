#ifndef _X86_ELF_H
#define _X86_ELF_H

#include <kernel.h>
#include <elf/elf.h>

#define	SHN_SOMBRA	0xABCD

#define ELF_MAG_CHECK(ident) (ident[EI_MAG0] == ELFMAG0 && ident[EI_MAG1] == ELFMAG1 && \
				ident[EI_MAG2] == ELFMAG2 && ident[EI_MAG3] == ELFMAG3)
#define ELF_ISVERSION(ident) (ident[EI_VERSION] == EV_CURRENT)

#define ISELF(ehdr, class, dtype) (ehdr->e_ident[EI_CLASS] == class && ehdr->e_ident[EI_DATA] == dtype)
#define ISELF32(ehdr) ISELF(ehdr, ELFCLASS32, ELFDATA2LSB)
#define ISELF64(ehdr) ISELF(ehdr, ELFCLASS64, ELFDATA2LSB)

// status da relocalização (não padrão).
#define	RELOC_OK		0	// Sem erros
#define	RELOC_UNDEF_SYMBOL	1	// Símbolo indefinido. (sugestão: definir e tentar relocalizar novamente)
#define	RELOC_ERROR_SYMTAB	2	// Symbol Table não é válida.
#define	RELOC_ERROR_TARGET	3	// Seção alvo não é válida.
#define	RELOC_ERROR_SYMBOL	4	// Símbolo não existe.
#define	RELOC_ERROR_MEM		5	// Memória insuficiente.
#define	RELOC_ERROR		6	// Erro generico
#define	RELOC_COMMON		7	// COMMON SIMBOL não alocado.

int load_elf_exec(char *image, uintptr_t *entry, aspace_t *as);

// 32-bit

inline int elf32_is_valid(void *image);
inline int elf32_load_exec(char *image, uintptr_t *entry, aspace_t *as);
uint8_t	elf32_get_section_by_name(void* image, char* sect_name, Elf_Shdr32** sect);
int32_t	elf32_get_sections(void* image, uint32_t type, int32_t flags);
uint8_t	elf32_get_sections_i(void* image, uint32_t index, uint32_t type, int32_t flags, Elf_Shdr32** sect);
uint8_t	elf32_get_section_i(void* image, uint32_t index, Elf_Shdr32** sect);
uint8_t	elf32_get_symbol_i(void* image, uint32_t sym_idx, Elf_Shdr32* symtab, Elf_Sym32** symbol);
uint8_t	elf32_get_symbol_by_name(void* image, const char* sym_name, int32_t type, Elf_Shdr32* symtab, Elf_Shdr32* strtab, Elf_Sym32** symbol);
uint8_t	elf32_search_symbol(void* image, const char* name, uint32_t type, Elf_Sym32** symbol);
uint8_t	elf32_get_name(void* image, uint32_t index, Elf_Shdr32* strtab, const char** name);
uint8_t	elf32_reloc_apply(void* image, Elf_Rela32* rel, uint32_t symtab_idx, uint32_t t_sect_idx);

// 64-bit

inline int elf64_is_valid(void *image);
inline int elf64_load_exec(char *image, uintptr_t *entry, aspace_t *as);
uint8_t	elf64_get_section_by_name(void* image, char* sect_name, Elf_Shdr64** sect);
int32_t	elf64_get_sections(void* image, uint32_t type, int32_t flags);
uint8_t	elf64_get_sections_i(void* image, uint32_t index, uint32_t type, int32_t flags, Elf_Shdr64** sect);
uint8_t	elf64_get_section_i(void* image, uint32_t index, Elf_Shdr64** sect);
uint8_t	elf64_get_symbol_i(void* image, uint32_t sym_idx, Elf_Shdr64* symtab, Elf_Sym64** symbol);
uint8_t	elf64_get_symbol_by_name(void* image, const char* sym_name, int32_t type, Elf_Shdr64* symtab, Elf_Shdr64* strtab, Elf_Sym64** symbol);
uint8_t	elf64_search_symbol(void* image, const char* name, uint32_t type, Elf_Sym64** symbol);
uint8_t	elf64_get_name(void* image, uint32_t index, Elf_Shdr64* strtab, const char** name);

#endif
