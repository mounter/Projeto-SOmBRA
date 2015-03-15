/*************************************************
** Autor: Mauro Joel Sch�tz
** DOM�NIO P�BLICO: 
**	VOC� PODE UTILIZAR ESTES ARQUIVOS DA FORMA QUE BEM QUISER,
**	SOMENTE MANTENHA O NOME DO AUTOR.
*************************************************/
#ifndef _ELF_H_
#define _ELF_H_

#include "elftypes.h"

#define EI_NIDENT	16

#pragma pack(1)
typedef struct {
	unsigned char	e_ident[EI_NIDENT];	// Identifica��o de arquivo
	Elf_Half	e_type;			// Tipo de arquivo (executavel, relocavel, compartilhavel)
	Elf_Half	e_machine;		// Tipo de m�quina
	Elf_Word	e_version;		// Vers�o da especifica��o ELF
	Elf_Addr32	e_entry;		// Ponto de entrada do executavel (se 0, n�o a ponto de entrada!)
	Elf_Off32	e_phoff;		// Offset para a tabela de cabe�alho de programa (se 0, n�o existe)
	Elf_Off32	e_shoff;		// Offset tabela de cabe�alho de se��es (se 0, n�o existe)
	Elf_Word	e_flags;		// Flags de especifico para m�quina
	Elf_Half	e_ehsize;		// Tamanho do cabe�alho Elf em bytes
	Elf_Half	e_phentsize;		// Tamanho das entradas do cabe�alho de programa
	Elf_Half	e_phnum;		// Quantidade de entradas no cabe�alho de programa
	Elf_Half	e_shentsize;		// Tamanho de uma entrada no cabe�alho de se��es
	Elf_Half	e_shnum;		// Quantidade de entradas no cabe�alho de se��es
	Elf_Half	e_shstrndx;		// Indice na tabela de se��es associado a se��o com a tabela de nomes
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

// Numeros m�gicos
#define ELFMAG0		0x7f
#define ELFMAG1		'E'
#define ELFMAG2		'L'
#define ELFMAG3		'F'

// Numeros para classes de objetos
#define ELFCLASSNONE	0	// Classe invalida
#define ELFCLASS32	1	// Objetos de 32-bit
#define ELFCLASS64	2	// Objetos de 64-bit

// Numero para codifica��o de dados
#define ELFDATANONE	0	// C�difica��o de dados invalida
#define ELFDATA2LSB	1	// Byte menos significante ocupando o menor endere�o | Ex. 0x0102 = | 02 | 01 |
#define ELFDATA2MSB	2	// Byte mais significante ocupando o menor endere�o  | Ex. 0x0102 = | 01 | 02 |

// Defini��o de valores para e_ident
#define EI_MAG0		0		// Identifica��o de arquivo
#define EI_MAG1		1		// Identifica��o de arquivo
#define EI_MAG2		2		// Identifica��o de arquivo
#define EI_MAG3		3		// Identifica��o de arquivo
#define EI_CLASS		4		// Arquivo de classe
#define EI_DATA		5		// C�difica��o de dados do processador
#define EI_VERSION		6		// Vers�o de arquivo
#define EI_PAD			7		// Inicio dos bytes sem uso da e_ident
#define EI_NIDENT		16		// Tamanho de e_ident

// Defini��o de valores para e_type
#define ET_NONE		0		// Sem tipo de arquivo
#define ET_REL		1		// Arquivo Relocavel	
#define ET_EXEC		2		// Arquivo executavel
#define ET_DYN		3		// Arquivo compatilhavel/shared
#define ET_CORE		4		// Arquivo Core
#define ET_LOPROC		0xFF00		// Especifico de processador
#define ET_HIPROC		0xFFFF		// Especifico de processador

// Defini��o de valores para e_machine
#define EM_NONE		0		// Sem m�quina
#define EM_M32		1		// AT&T WE 32100
#define EM_SPARC		2		// Sparc
#define EM_386		3		// Intel 80386
#define EM_68K		4		// Motorola 68000
#define EM_88K		5		// Motorola 88000
#define EM_860		7		// Intel 80860
#define EM_MIPS		8		// Mips RS3000
#define EM_X86_64		62		// AMD64

// Defini��o para e_version
#define EV_NONE		0		// Vers�o inv�lida
#define EV_CURRENT	1		// Vers�o corrente

// Tamanho m�ximo de phnum
#define PN_XNUM 0xFFFF

#include "elfsections.h"
#include "elfsymbols.h"
#include "elfrels.h"
#include "elfphdr.h"
#include "elfdyn.h"
#include "elfhash.h"

#endif
