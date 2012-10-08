/*************************************************
** Autor: Mauro Joel Schütz
** DOMÍNIO PÚBLICO: 
**	VOCÊ PODE UTILIZAR ESTES ARQUIVOS DA FORMA QUE BEM QUISER,
**	SOMENTE MANTENHA O NOME DO AUTOR.
*************************************************/
#ifndef _ELFRELS_H_
#define _ELFRELS_H_

// Tipo de relocação Rel
typedef struct {
	Elf_Addr32	r_offset;	// Offset do local que será afetado pela relocação
	Elf_Word		r_info;		/* Contem o indíce para da tabela de simbolos e o tipo de relocação */
} Elf_Rel32;

// Tipo de relocação Rela
typedef struct {
	Elf_Addr32	r_offset;
	Elf_Word		r_info;
	Elf_Sword		r_addend;	/* Numero que deve ser adicionado para calcular o valor a ser guardado no campo de relocação */
} Elf_Rela32;


typedef struct
{
	Elf_Addr64 	r_offset; /* Address of reference */
	Elf_Xword 	r_info; /* Symbol index and type of relocation */
} Elf64_Rel;

typedef struct
{
	Elf_Addr64	r_offset; /* Address of reference */
	Elf_Xword		r_info;  /* Symbol index and type of relocation */
	Elf_Sxword 	r_addend; /* Constant part of expression */
} Elf64_Rela; 


// Macros para r_info
#define ELF32_R_SYM(i)		((i) >> 8)
#define ELF32_R_TYPE(i)		((unsigned char)(i))
#define ELF32_R_INFO(s, t)	(((s) << 8) + (unsigned char)(t))

#define ELF64_R_SYM(i)		((i) >> 32)
#define ELF64_R_TYPE(i)		((i) & 0xFFFFFFFFL)
#define ELF64_R_INFO(s, t)	(((s) << 32) + ((t) & 0xFFFFFFFFL))

/*	Tipos de relocação

 Nome		| Valor	| Campo		| Calculo	|
---------------------------------------------------------
 R_386_NONE	| 0	| none		| none
 R_386_32	| 1	| word32	| S + A
 R_386_PC32	| 2	| word32	| S + A - P
 R_386_GOT32	| 3	| word32	| G + A - P
 R_386_PLT32	| 4	| word32	| L + A - P
 R_386_COPY	| 5	| none		| none
 R_386_GLOB_DAT	| 6	| word32	| S
 R_386_JMP_SLOT	| 7	| word32	| S
 R_386_RELATIVE	| 8	| word32	| B + A
 R_386_GOTOFF	| 9	| word32	| S + A - GOT
 R_386_GOTPC	| 10	| word32	| GOT + A - P

*/

// Definições para ELF32_R_TYPE
#define R_386_NONE	0
#define R_386_32	1
#define R_386_PC32	2
#define R_386_GOT32	3
#define R_386_PLT32	4
#define R_386_COPY	5
#define R_386_GLOB_DAT	6
#define R_386_JMP_SLOT	7
#define R_386_RELATIVE	8
#define R_386_GOTOFF	9
#define R_386_GOTPC	10

// Tipos de relocação para x86-64
#define R_X86_64_NONE		0	// Nenhuma relocação
#define R_X86_64_64		1	// Direct 64 bit 
#define R_X86_64_PC32		2	// PC relative 32 bit signed
#define R_X86_64_GOT32	3	// 32 bit GOT entry
#define R_X86_64_PLT32		4	// 32 bit PLT address
#define R_X86_64_COPY		5	// Copy symbol at runtime
#define R_X86_64_GLOB_DAT	6	// Create GOT entry
#define R_X86_64_JUMP_SLOT	7	// Create PLT entry
#define R_X86_64_RELATIVE	8	// Adjust by program base
#define R_X86_64_GOTPCREL	9	// 32 bit signed pc relative offset to GOT
#define R_X86_64_32		10	// Direct 32 bit zero extended
#define R_X86_64_32S		11	// Direct 32 bit sign extended
#define R_X86_64_16		12	// Direct 16 bit zero extended
#define R_X86_64_PC16		13	// 16 bit sign extended pc relative
#define R_X86_64_8		14	// Direct 8 bit sign extended
#define R_X86_64_PC8		15	// 8 bit sign extended pc relative

#define R_X86_64_NUM		16

#endif
