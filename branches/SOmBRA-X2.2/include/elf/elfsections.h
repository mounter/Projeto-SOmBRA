/*************************************************
** Autor: Mauro Joel Sch�tz
** DOM�NIO P�BLICO: 
**	VOC� PODE UTILIZAR ESTES ARQUIVOS DA FORMA QUE BEM QUISER,
**	SOMENTE MANTENHA O NOME DO AUTOR.
*************************************************/
#ifndef _ELFSECTIONS_H_
#define _ELFSECTIONS_H_

// Indices de se��es reservadas
#define SHN_UNDEF	0		// Se��o reservada, perdida, irrelevante ou uma se��o com outro tipo de referencia.
#define SHN_LORESERVE	0xFF00		// Este valor especifica o limite inferior da gama de �ndices reservados.
#define SHN_LOPROC	0xFF00		// Valores de SHN_LOPROC a SHN_HIPROC s�o reservados para fun��es
#define SHN_HIPROC	0xFF1F		// especificas do processador.
#define SHN_ABS		0xFFF1		// Este valor especifica valores absolutos de referencia.
#define SHN_COMMON	0xFFF2		// Simbolos comuns, relativo a vari�veis externas.
#define SHN_HIRESERVE	0xFFFF		// Especifica o topo dos indices reservados.

// Estrutura de uma se��o
#pragma pack(1)
typedef struct {
	Elf_Word		sh_name;	// �ndice na tabela de strings, especificando o nome da se�ao
	Elf_Word		sh_type;	// Categoriza o conteudo das se��es e sem�nticas
	Elf_Word		sh_flags;	// Flags da se��o
	Elf_Addr32		sh_addr;	// Endere�o do primeiro byte da se��o (Se 0, n�o h� endere�o especificado)
	Elf_Off32		sh_offset;	// Offset no arquivo para o primeiro byte da se��o
	Elf_Word		sh_size;	// Tamanho da se��o em bytes
	Elf_Word		sh_link;	// �ndice na tabela de se��es. Este valor depende do tipo de se��o.
	Elf_Word		sh_info;	// Informa��o extra, depende do tipo da se��o
	Elf_Word		sh_addralign;	// N�mero de alinhamento de se��o
	Elf_Word		sh_entsize;	/* Algumas se��es possuem entradas de tamanho fixo, este � o numero de bytes
					 que cada se��o possui. */
} Elf_Shdr32;

#pragma pack(1)
typedef struct {
	Elf_Word		sh_name;
	Elf_Word		sh_type;
	Elf_Xword		sh_flags;
	Elf_Addr64	sh_addr;
	Elf_Off64		sh_offset;
	Elf_Xword		sh_size;
	Elf_Word		sh_link;
	Elf_Word		sh_info;
	Elf_Xword		sh_addralign;
	Elf_Xword		sh_entsize;
} Elf_Shdr64;


// Constantes para sh_type
#define SHT_NULL		0		// Se��o inativa
#define SHT_PROGBITS	1		// Informa��o definida pelo programa
#define SHT_SYMTAB	2		// Se��o contendo uma tabela de s�mbolos
#define SHT_STRTAB	3		// Se��o contendo uma tabela de strings
#define SHT_RELA		4		// Se��o de reloca��o, contendo o tipo Elf32_Rela
#define SHT_HASH		5		// Se��o contendo uma tabela de hashs
#define SHT_DYNAMIC	6		// Se��o contendo informa��es para a reloca��o dinamica
#define SHT_NOTE		7		// Se��o que marca o arquivo de algum modo
#define SHT_NOBITS	8		// Se��o que n�o contem dados no arquivo 
#define SHT_REL		9		// Se��o de reloca��o, contendo o tipo Elf32_Rel
#define SHT_SHLIB		10		// Tipo reservado
#define SHT_DYNSYM	11		// Se��o contendo uma tabela de s�mbolos
#define SHT_LOPROC	0x70000000	// Se��o reservada para sem�nticas para o processador
#define SHT_X86_64_UNWIND 0x70000001	// 
#define SHT_HIPROC	0x7FFFFFFF	// Limite da se��o para sem�nticas
#define SHT_LOUSER	0x80000000	// Se��o reservada para a aplica��o - Usar para identificar o processo?
#define SHT_HIUSER	0xFFFFFFFF	// Limite da se��o da aplica��o

// Constantes para sh_flags
#define SHF_WRITE	0x01		// Se��o contendo dados de escrita
#define SHF_ALLOC	0x02		// A se��o ocupa espa�o durante a execu��o do processo
#define SHF_EXECINSTR	0x04		// Se��o contendo c�digo de m�quina
#define SHF_X86_64_LARGE 0x10000000	// Se��o maior que 2GB
#define SHF_MASKPROC	0xF0000000	// Todos os bits incluidos neste Mask s�o especificos para processadores

/* Para sh_link e sh_info quando as se��es forem do tipo SHT_REL, SHT_RELA, SHT_SYMTAB e SHT_DYNSYM, seus valores 
 s�o definidos pela se��o, caso contrario s�o zerados! */

/*

Nomes de se��es Especiais

Nome		|	Tipo	|	Atributos	|
---------------------------------------------------------
.bss		| SHT_NOBITS	| SHF_ALLOC + SHF_WRITE
.comment	| SHT_PROGBITS	| none
.data		| SHT_PROGBITS	| SHF_ALLOC + SHF_WRITE
.data1		| SHT_PROGBITS	| SHF_ALLOC + SHF_WRITE
.debug		| SHT_PROGBITS	| none
.dynamic	| SHT_DYNAMIC	| Depende da se��o
.dynstr		| SHT_STRTAB	| SHF_ALLOC
.dynsym		| SHT_DYNSYM	| SHF_ALLOC
.fini		| SHT_PROGBITS	| SHF_ALLOC + SHF_EXECINSTR
.got		| SHT_PROGBITS	| Depende da se�ao
.hash		| SHT_HASH	| SHF_ALLOC
.init		| SHT_PROGBITS	| SHF_ALLOC + SHF_EXECINSTR
.interp		| SHT_PROGBITS	| Depende da se��o
.line		| SHT_PROGBITS	| none
.note		| SHT_NOTE	| none
.plt		| SHT_PROGBITS	| Depende da se��o
.relNAME	| SHT_REL	| Depende da se��o
.relaNAME	| SHT_RELA	| Depende da se��o
.rodata		| SHT_PROGBITS	| SHF_ALLOC
.rodata1	| SHT_PROGBITS	| SHF_ALLOC
.shstrtab	| SHT_STRTAB	| none
.strtab		| SHT_STRTAB	| Depende da se��o
.symtab		| SHT_SYMTAB	| Depende da se��o
.text		| SHT_PROGBITS	| SHF_ALLOC + SHF_EXECINSTR 

*/

/*
	AMD64 - X86_64
Nome		|	Tipo		|	Atributos	|
-----------------------------------------------------------------
.got		| SHT_PROGBITS		| SHF_ALLOC+SHF_WRITE
.plt		| SHT_PROGBITS		| SHF_ALLOC+SHF_EXECINSTR
.eh_frame	| SHT_X86_64_UNWIND	| SHF_ALLOC

*/

#endif
