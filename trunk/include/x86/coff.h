#ifndef _COFF_H
#define _COFF_H

#include <kernel.h>

// Estrutura para descrever o formato COFF.
typedef struct{
	uint16_t magic;
	uint16_t num_sects;
	uint32_t time_date;
	uint32_t symtab_offset;
	uint32_t num_syms;
	uint16_t aout_hdr_size;
	uint16_t flags;
} coff_file_t; // 20 bytes

// Estrutura para descrever o formato a.out.
typedef struct{
	uint16_t magic;
	uint16_t version;
	uint32_t code_size;
	uint32_t data_size;
	uint32_t bss_size;
	uint32_t entry;
	uint32_t code_offset;
	uint32_t data_offset;
} coff_aout_t; // 28 bytes

// Estrutura para descrever uma seção.
typedef struct{
	char name[8];
	uint32_t phys_adr;
	uint32_t virt_adr;
	uint32_t size;				// Tamanho da seção.
	uint32_t offset;				// Entrada da seção.
	uint32_t relocs_offset;
	uint32_t line_nums_offset;
	uint16_t num_relocs;
	uint16_t num_line_nums;
	uint32_t flags;				// Indica o tipo da seção(Código, Data, etc).
} coff_sect_t; // 40 bytes

int load_coff_exec(char *image, unsigned *entry, aspace_t *as);

#endif
