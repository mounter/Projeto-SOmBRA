/*************************************************
** Autor: Mauro Joel Schütz
** DOMÍNIO PÚBLICO: 
**	VOCÊ PODE UTILIZAR ESTES ARQUIVOS DA FORMA QUE BEM QUISER,
**	SOMENTE MANTENHA O NOME DO AUTOR.
*************************************************/
#ifndef _ELFDYN_H_
#define _ELFDYN_H_

// Estrutura que representas as entradas da seção .dynamic
#pragma pack(1)
typedef struct {
	Elf_Word	d_tag;		// Constante que representa o tipo de d_un
	union {
		Elf_Word		d_val;	// Representa valores inteiros com várias interpretações
		Elf_Addr32	d_ptr;	// Representa endereços virtuais
	} d_un;
} Elf_Dyn32;

typedef struct
{
	Elf_Sxword	d_tag;
	union {
		Elf_Xword		d_val;
		Elf_Addr64	d_ptr;
	} d_un;
} Elf_Dyn64;

// Valores para d_tag
#define DT_NULL			0			// Marca o fim do array _DYNAMIC
#define DT_NEEDED			1			// Contem o nome da lib necessária para o programa executar
#define DT_PLTRELSZ		2			// Contem o tamanho de cada entrada da tabela de procedimentos
#define DT_PLTGOT			3			// Endereço para os procedimentos ou para a tabela global
#define DT_HASH			4			// Endereço da tabela de HASHs
#define DT_STRTAB			5			// Endereço da tabela de strings
#define DT_SYMTAB			6			// Endereço da tabela de simbolos
#define DT_RELA			7			// Contem o endereço de uma tabela de relocação
#define DT_RELASZ			8			// Contem o tamanho total de uma DT_RELA
#define DT_RELAENT		9			// Contem o tamanho de uma entrada de DT_RELA
#define DT_STRSZ			10			// Tamanho total da tabela de strings
#define DT_SYMENT			11			// Tamanho em bytes de uma entrada da tabela de simbolos
#define DT_INIT			12			// Contem o endereço da função de inicialização na seção INIT
#define DT_FINI			13			// Contem o endereço da função de finalização na seção FINI
#define DT_SONAME		14			// Contem o indíce na tabela de strings que representa o nome do objeto
#define DT_RPATH			15			// Endereço relativo para localizar as bibliotecas
#define DT_SYMBOLIC		16			// Simboliza uma alteração no modo de procura por dependencias
#define DT_REL			17			// Contem o endereço de uma tabela de relocação
#define DT_RELSZ			18			// Contem o tamanho total de uma DT_REL
#define DT_RELENT			19			// Contem o tamanho de uma entrada de DT_REL
#define DT_PLTREL			20			// Tipo de relocação que uma PLT referesse
#define DT_DEBUG			21			// Informação para Debug, não especificado
#define DT_TEXTREL		22			/* Se este estiver presente, uma ou mais entradas de relocação poderá pedir alteração de um segmento de leitura */
#define DT_JMPREL			23			// Possui entrada para tabela de relocação
#define DT_BIND_NOW		24			// Processar todos as relocações antes do programa executar
#define DT_INIT_ARRAY		25			// d_ptr - Pointer to an array of pointers to initialization functions.
#define DT_FINI_ARRAY		26			// d_ptr - Pointer to an array of pointers to termination functions.
#define DT_INIT_ARRAYSZ	27			// d_val - Size, in bytes, of the array of initialization functions.
#define DT_FINI_ARRAYSZ	28			// d_val - Size, in bytes, of the array of termination functions.
#define DT_GNU_HASH		0x6FFFFEF5	// GNU HASH
#define DT_LOPROC			0x70000000	// Faxa de endereços reservados para processo
#define DT_HIPROC			0x7FFFFFFF	// Limite da Faixa

#endif
