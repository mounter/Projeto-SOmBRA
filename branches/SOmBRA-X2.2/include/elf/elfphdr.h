/*************************************************
** Autor: Mauro Joel Sch�tz
** DOM�NIO P�BLICO: 
**	VOC� PODE UTILIZAR ESTES ARQUIVOS DA FORMA QUE BEM QUISER,
**	SOMENTE MANTENHA O NOME DO AUTOR.
*************************************************/
#ifndef _ELFPHDR_H_
#define _ELFPHDR_H_

/** Cabe�alho de programa

Algumas entradas descrevem segmentos; outras nos d�o informa��es
suplementares que n�o contribuem na forma��o da imagem do processo. */

#pragma pack(1)
typedef struct {
	Elf_Word		p_type;		// Como deve ser interpretado os segmentos
	Elf_Off32		p_offset;	/* Offset para do inicio do arquivo que deve ser o primeiro byte que o segmento reside */
	Elf_Addr32	p_vaddr;	// Endere�o virtual do programa
	Elf_Addr32	p_paddr;	// Endere�o f�sico do programa
	Elf_Word		p_filesz;	// Tamanho do arquivo (geralmente 0)
	Elf_Word		p_memsz;	// Tamanho do programa na memoria (geralmente 0)
	Elf_Word		p_flags;	// Flags para o segmento do programa
	Elf_Word		p_align;	/* Como o segmento deve ser alinhado, deve ser divisivel por 2 (se for 0 ou 1 n�o h� alinhamento) */
} Elf_Phdr32;

#pragma pack(1)
typedef struct {
	Elf_Word		p_type;
	Elf_Word		p_flags;
	Elf_Off64		p_offset;
	Elf_Addr64	p_vaddr;
	Elf_Addr64	p_paddr;
	Elf_Xword		p_filesz;
	Elf_Xword		p_memsz;
	Elf_Xword		p_align;
} Elf_Phdr64;

// Valores para p_type
#define PT_NULL		0		// Os segmentos s�o opcionais
#define PT_LOAD		1		// Os segmentos devem ser carregados e o excesso deve ser zerado
#define PT_DYNAMIC	2		// Os segmentos descrevem informa��es para a lincagem dinamica
#define PT_INTERP		3		// O segmento possuem o caminho para o interpretador que deve ser chamado
#define PT_NOTE		4		// O segmento especifica a localiza��o e o tamanho das informa��es auxiliares
#define PT_SHLIB		5		// o segmento � reservado, mas possuem informa��es n�o especificadas
#define PT_PHDR		6		// O segmento possui o cabe�alho que deve proceder o programa
#define PT_LOPROC	0x70000000	// Reservados para o processo/processador
#define PT_HIPROC	0x7FFFFFFF	// Limite

// Valores para p_flags
#define PF_X		0x01		// Segmento para execu��o
#define PF_W		0x02		// Segmento para escrita/dados
#define PF_R		0x04		// Segmento para leitura
#define PF_RWX		(PF_X | PF_W | PF_R)
#define PF_MASKPROC	0xF0000000	// Reservado para processador

// Estrutura que descreve uma nota
#pragma pack(1)
typedef struct {
	Elf_Word		namesz;		// Tamanho em byte de name
	Elf_Word		descsz;		// Tamanho da descri��o
	Elf_Half		type;		// Tipo de desc
	Elf_Addr32	name;		// Nome do campo
	Elf_Addr32	desc;		// definido pelo campo type
} Note_t;

#endif
