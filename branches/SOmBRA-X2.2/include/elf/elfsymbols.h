/*************************************************
** Autor: Mauro Joel Sch�tz
** DOM�NIO P�BLICO: 
**	VOC� PODE UTILIZAR ESTES ARQUIVOS DA FORMA QUE BEM QUISER,
**	SOMENTE MANTENHA O NOME DO AUTOR.
*************************************************/
#ifndef _ELFSYMBOLS_H_
#define _ELFSYMBOLS_H_

// Estrutura definindo um simbolo
#pragma pack(1)
typedef struct {
	Elf_Word			st_name;	// Ind�ce para o nome que est� contido na se��o .dynstr
	Elf_Addr32		st_value;	// Valor do simbolo, pode ser uma constante, um endere�o ou qualquer valor...
	Elf_Word			st_size;	// Tamanho do simbolo
	unsigned char	st_info;	// Especifica o tipo do simbolo e valores de bind (traduzir!)
	unsigned char	st_other;	// Valor zerado e sem significado definido
	Elf_Half			st_shndx;	// De que se��o � o simbolo!
} Elf_Sym32;

#pragma pack(1)
typedef struct {
	 Elf_Word			st_name;
	 unsigned char	st_info;
	 unsigned char	st_other;
	 Elf_Half			st_shndx;
	 Elf_Addr64		st_value;
	 Elf_Xword		st_size;
} Elf_Sym64;

#define STN_UNDEF	0	// Entrada nula ou indefinida

// Macros para manipular valores de st_info
#define ELF_ST_BIND(i)		((i) >> 4)
#define ELF_ST_TYPE(i)		((i) & 0x0F)
#define ELF_ST_INFO(b, t)	(((b) << 4) + ((t) & 0x0F))

// Valores para ELF32_ST_BIND
#define STB_LOCAL		0	// S�mbolo local
#define STB_GLOBAL	1	// S�mbolo global
#define STB_WEAK		2	/* S�mbolo global de baixa referencia (ie. se a um simbolo STB_GLOBAL com mesmo nome, ele substitui o simbolo com STB_WEAK) */
#define STB_LOPROC	13	// Simbolos de LOPROC a HIPROC s�o reservados para processador
#define STB_HIPROC	15

// Valores para ELF32_ST_TYPE
#define STT_NOTYPE	0	// Sem tipo definido
#define STT_OBJECT		1	// Pode ser uma variavel, array...
#define STT_FUNC		2	// Fun��o ou c�digo executavel
#define STT_SECTION	3	// Simbolo associado a uma se��o, geralmente tem o BIND como STB_LOCAL
#define STT_FILE		4	/* Por conven��o, o nome do simbolo � um arquivo associado ao arquivo objeto, seu BIND � STB_LOCAL e st_shndx � igual a SHN_ABS */
#define STT_LOPROC	13	// Simbolos de LOPROC a HIPROC s�o reservados para processador
#define STT_HIPROC		15

#endif
