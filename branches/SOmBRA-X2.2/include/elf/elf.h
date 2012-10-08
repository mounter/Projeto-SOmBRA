/*************************************************
** Autor: Mauro Joel Schütz
** DOMÍNIO PÚBLICO: 
**	VOCÊ PODE UTILIZAR ESTES ARQUIVOS DA FORMA QUE BEM QUISER,
**	SOMENTE MANTENHA O NOME DO AUTOR.
*************************************************/
#ifndef _ELF_H_
#define _ELF_H_

#include "elftypes.h"

#define EI_NIDENT	16

#pragma pack(1)
typedef struct {
	unsigned char	e_ident[EI_NIDENT];	// Identificação de arquivo
	Elf_Half	e_type;			// Tipo de arquivo (executavel, relocavel, compartilhavel)
	Elf_Half	e_machine;		// Tipo de máquina
	Elf_Word	e_version;		// Versão da especificação ELF
	Elf_Addr32	e_entry;		// Ponto de entrada do executavel (se 0, não a ponto de entrada!)
	Elf_Off32	e_phoff;		// Offset para a tabela de cabeçalho de programa (se 0, não existe)
	Elf_Off32	e_shoff;		// Offset tabela de cabeçalho de seções (se 0, não existe)
	Elf_Word	e_flags;		// Flags de especifico para máquina
	Elf_Half	e_ehsize;		// Tamanho do cabeçalho Elf em bytes
	Elf_Half	e_phentsize;		// Tamanho das entradas do cabeçalho de programa
	Elf_Half	e_phnum;		// Quantidade de entradas no cabeçalho de programa
	Elf_Half	e_shentsize;		// Tamanho de uma entrada no cabeçalho de seções
	Elf_Half	e_shnum;		// Quantidade de entradas no cabeçalho de seções
	Elf_Half	e_shstrndx;		// Indice na tabela de seções associado a seção com a tabela de nomes
} Elf_Ehdr32;

#pragma pack(1)
typedef struct {
	 unsigned char e_ident[EI_NIDENT];
	 Elf_Half e_type;
	 Elf_Half e_machine;
	 Elf_Word e_version;
	 Elf_Addr64 e_entry;
	 Elf_Off64 e_phoff;
	 Elf_Off64 e_shoff;
	 Elf_Word e_flags;
	 Elf_Half e_ehsize;
	 Elf_Half e_phentsize;
	 Elf_Half e_phnum;
	 Elf_Half e_shentsize;
	 Elf_Half e_shnum;
	 Elf_Half e_shstrndx;
} Elf_Ehdr64;

// Numeros mágicos
#define ELFMAG0		0x7f
#define ELFMAG1		'E'
#define ELFMAG2		'L'
#define ELFMAG3		'F'

// Numeros para classes de objetos
#define ELFCLASSNONE	0	// Classe invalida
#define ELFCLASS32	1	// Objetos de 32-bit
#define ELFCLASS64	2	// Objetos de 64-bit

// Numero para codificação de dados
#define ELFDATANONE	0	// Códificação de dados invalida
#define ELFDATA2LSB	1	// Byte menos significante ocupando o menor endereço | Ex. 0x0102 = | 02 | 01 |
#define ELFDATA2MSB	2	// Byte mais significante ocupando o menor endereço  | Ex. 0x0102 = | 01 | 02 |

// Definição de valores para e_ident
#define EI_MAG0		0		// Identificação de arquivo
#define EI_MAG1		1		// Identificação de arquivo
#define EI_MAG2		2		// Identificação de arquivo
#define EI_MAG3		3		// Identificação de arquivo
#define EI_CLASS		4		// Arquivo de classe
#define EI_DATA		5		// Códificação de dados do processador
#define EI_VERSION		6		// Versão de arquivo
#define EI_PAD			7		// Inicio dos bytes sem uso da e_ident
#define EI_NIDENT		16		// Tamanho de e_ident

// Definição de valores para e_type
#define ET_NONE		0		// Sem tipo de arquivo
#define ET_REL		1		// Arquivo Relocavel	
#define ET_EXEC		2		// Arquivo executavel
#define ET_DYN		3		// Arquivo compatilhavel/shared
#define ET_CORE		4		// Arquivo Core
#define ET_LOPROC		0xFF00		// Especifico de processador
#define ET_HIPROC		0xFFFF		// Especifico de processador

// Definição de valores para e_machine
#define EM_NONE		0		// Sem máquina
#define EM_M32		1		// AT&T WE 32100
#define EM_SPARC		2		// Sparc
#define EM_386		3		// Intel 80386
#define EM_68K		4		// Motorola 68000
#define EM_88K		5		// Motorola 88000
#define EM_860		7		// Intel 80860
#define EM_MIPS		8		// Mips RS3000
#define EM_X86_64		62		// AMD64

// Definição para e_version
#define EV_NONE		0		// Versão inválida
#define EV_CURRENT	1		// Versão corrente

// Tamanho máximo de phnum
#define PN_XNUM 0xFFFF

#include "elfsections.h"
#include "elfsymbols.h"
#include "elfrels.h"
#include "elfphdr.h"
#include "elfdyn.h"
#include "elfhash.h"

#endif
