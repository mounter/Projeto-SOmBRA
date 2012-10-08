/*************************************************
** Autor: Mauro Joel Schütz
** DOMÍNIO PÚBLICO: 
**	VOCÊ PODE UTILIZAR ESTES ARQUIVOS DA FORMA QUE BEM QUISER,
**	SOMENTE MANTENHA O NOME DO AUTOR.
*************************************************/
#ifndef _ELFSECTIONS_H_
#define _ELFSECTIONS_H_

// Indices de seções reservadas
#define SHN_UNDEF	0		// Seção reservada, perdida, irrelevante ou uma seção com outro tipo de referencia.
#define SHN_LORESERVE	0xFF00		// Este valor especifica o limite inferior da gama de índices reservados.
#define SHN_LOPROC	0xFF00		// Valores de SHN_LOPROC a SHN_HIPROC são reservados para funções
#define SHN_HIPROC	0xFF1F		// especificas do processador.
#define SHN_ABS		0xFFF1		// Este valor especifica valores absolutos de referencia.
#define SHN_COMMON	0xFFF2		// Simbolos comuns, relativo a variáveis externas.
#define SHN_HIRESERVE	0xFFFF		// Especifica o topo dos indices reservados.

// Estrutura de uma seção
#pragma pack(1)
typedef struct {
	Elf_Word		sh_name;	// Índice na tabela de strings, especificando o nome da seçao
	Elf_Word		sh_type;	// Categoriza o conteudo das seções e semânticas
	Elf_Word		sh_flags;	// Flags da seção
	Elf_Addr32		sh_addr;	// Endereço do primeiro byte da seção (Se 0, não há endereço especificado)
	Elf_Off32		sh_offset;	// Offset no arquivo para o primeiro byte da seção
	Elf_Word		sh_size;	// Tamanho da seção em bytes
	Elf_Word		sh_link;	// índice na tabela de seções. Este valor depende do tipo de seção.
	Elf_Word		sh_info;	// Informação extra, depende do tipo da seção
	Elf_Word		sh_addralign;	// Número de alinhamento de seção
	Elf_Word		sh_entsize;	/* Algumas seções possuem entradas de tamanho fixo, este é o numero de bytes
					 que cada seção possui. */
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
#define SHT_NULL		0		// Seção inativa
#define SHT_PROGBITS	1		// Informação definida pelo programa
#define SHT_SYMTAB	2		// Seção contendo uma tabela de símbolos
#define SHT_STRTAB	3		// Seção contendo uma tabela de strings
#define SHT_RELA		4		// Seção de relocação, contendo o tipo Elf32_Rela
#define SHT_HASH		5		// Seção contendo uma tabela de hashs
#define SHT_DYNAMIC	6		// Seção contendo informações para a relocação dinamica
#define SHT_NOTE		7		// Seção que marca o arquivo de algum modo
#define SHT_NOBITS	8		// Seção que não contem dados no arquivo 
#define SHT_REL		9		// Seção de relocação, contendo o tipo Elf32_Rel
#define SHT_SHLIB		10		// Tipo reservado
#define SHT_DYNSYM	11		// Seção contendo uma tabela de símbolos
#define SHT_LOPROC	0x70000000	// Seção reservada para semânticas para o processador
#define SHT_X86_64_UNWIND 0x70000001	// 
#define SHT_HIPROC	0x7FFFFFFF	// Limite da seção para semânticas
#define SHT_LOUSER	0x80000000	// Seção reservada para a aplicação - Usar para identificar o processo?
#define SHT_HIUSER	0xFFFFFFFF	// Limite da seção da aplicação

// Constantes para sh_flags
#define SHF_WRITE	0x01		// Seção contendo dados de escrita
#define SHF_ALLOC	0x02		// A seção ocupa espaço durante a execução do processo
#define SHF_EXECINSTR	0x04		// Seção contendo código de máquina
#define SHF_X86_64_LARGE 0x10000000	// Seção maior que 2GB
#define SHF_MASKPROC	0xF0000000	// Todos os bits incluidos neste Mask são especificos para processadores

/* Para sh_link e sh_info quando as seções forem do tipo SHT_REL, SHT_RELA, SHT_SYMTAB e SHT_DYNSYM, seus valores 
 são definidos pela seção, caso contrario são zerados! */

/*

Nomes de seções Especiais

Nome		|	Tipo	|	Atributos	|
---------------------------------------------------------
.bss		| SHT_NOBITS	| SHF_ALLOC + SHF_WRITE
.comment	| SHT_PROGBITS	| none
.data		| SHT_PROGBITS	| SHF_ALLOC + SHF_WRITE
.data1		| SHT_PROGBITS	| SHF_ALLOC + SHF_WRITE
.debug		| SHT_PROGBITS	| none
.dynamic	| SHT_DYNAMIC	| Depende da seção
.dynstr		| SHT_STRTAB	| SHF_ALLOC
.dynsym		| SHT_DYNSYM	| SHF_ALLOC
.fini		| SHT_PROGBITS	| SHF_ALLOC + SHF_EXECINSTR
.got		| SHT_PROGBITS	| Depende da seçao
.hash		| SHT_HASH	| SHF_ALLOC
.init		| SHT_PROGBITS	| SHF_ALLOC + SHF_EXECINSTR
.interp		| SHT_PROGBITS	| Depende da seção
.line		| SHT_PROGBITS	| none
.note		| SHT_NOTE	| none
.plt		| SHT_PROGBITS	| Depende da seção
.relNAME	| SHT_REL	| Depende da seção
.relaNAME	| SHT_RELA	| Depende da seção
.rodata		| SHT_PROGBITS	| SHF_ALLOC
.rodata1	| SHT_PROGBITS	| SHF_ALLOC
.shstrtab	| SHT_STRTAB	| none
.strtab		| SHT_STRTAB	| Depende da seção
.symtab		| SHT_SYMTAB	| Depende da seção
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
