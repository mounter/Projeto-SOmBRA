/*****************************************************************************
**                                                                          **
**	Arquivo: kernel.h                                                       **
**	Descrição: Variaveis, constantes e tipos do kernel                      **
**	Autor: Mauro Joel Schütz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/

#ifndef KERNEL_H
#define KERNEL_H

#include <kernel/types.h>
#include <fs/fs.h>

/***************************** CONTROLE DE INTERRUPÇÕES *****************************/

#include "x86/x86.h"

// Estrutura para armazenar um vetor.
typedef struct
{
	unsigned access_byte, eip;
} vector_t;

#include REGS_H
#include <kernel/mm.h>

/**************************** CONTROLE DE TAREFAS/THREADS ***************************/

#define	NUM_FILES		20		// Número máximo de descritores de arquivos por tarefa.
#define	NUM_TASKS		32		// Número máximo de tarefas.

#define NUM_DESCRIPTOR  16		// Número de descritores.

#define	THREAD_MAGIC	0x1F9D	// Deve ser menor que 0x8000
#define	HZ				100		// Frequencia do timer do controlador 8253

#define	KBD_BUF_SIZE	32		// Tamanho do buffer do teclado.
#define	KRNL_STACK_SIZE	8192	// Tamanho da pilha do kernel.


// Flags utilizados para descrever os segmentos.
#define	SF_ZERO		0x10	// BSS - zerar antes de usar
#define	SF_LOAD		0x08	// Lido de um arquivo.
#define	SF_READ		0x04	// Leitura
#define	SF_WRITE	0x02	// Escrita
#define	SF_EXEC		0x01	// Executável
#define	SF_BSS		(SF_ZERO | SF_READ | SF_WRITE)

// Estrutura que descreve um setor(segmento).
typedef struct{
	unsigned adr, size, flags;
	unsigned long offset;
} sect_t;

// Descreve onde o processo e os seus segmentos estão armazenados na memória.
typedef struct{
	char *user_mem;		// Memória alocada para a tarefa (kmalloc).
	unsigned virt_adr;	// Menor valor do endereço virtual da tarefa.
	unsigned size;		// Tamanho total da tarefa.

	unsigned magic, num_sects;
	sect_t *sect;
	sect_t heap;
} aspace_t;


// Estrutura que descreve uma tarefa.
// kstack deve ser o primeiro item devido ao kstart.s
typedef struct _task{
	char *kstack;				// Valor atual do ESP do kernel(ring 0).
	aspace_t *as;
	unsigned pagedir;

	unsigned short int magic;		// Número mágico para a validação.
	char used;				// Flag indicando se a estrutura está livre ou não.

	enum{
		TS_RUNNABLE = 1, TS_BLOCKED = 2, TS_ZOMBIE = 3
	} status;				// Status do processo.

	int priority;				// Prioridade do processo.
	int exit_code;				// Código de saída do processo.

	unsigned timeout;			// Timeout
	char *kstack_mem;			// Pilha do kernel(kmalloc).
	struct _task *prev, *next;		// Usado na navegação das tarefas.
	//file_t *files[NUM_FILES];		// Descritores de arquivo.
	fs_node_t *files;           		// Descritores de arquivo


	bool     wait4pid;			// Indica se o processo está esperando por Wait4Pid.
	uint32_t pid;				// PID do processo.
	uint32_t ppid;				// PID do pai.
	char     cmd[16];			// Nome do processo.
	int16_t  console;			// Console.
} task_t;


// Estrutura que descreve uma fila
typedef struct{
	struct _task *head, *tail;
} wait_queue_t;


// Estrutura que descreve uma fila circular de tamanho máximo "size".
typedef struct {
	unsigned char *data;

	uint16_t size;
	uint16_t id_last;
	uint16_t id_first;
} queue_t;

/******************************** CONSOLES VIRTUAIS *********************************/

#define MAX_CONSOLE	8

/*typedef struct{
	uint16_t	col, row;
	uint16_t	color;
	uint16_t	*src;

	queue_t keys;

	wait_queue_t wait;
} console_t;*/

/***************************** ************************ *****************************/

// Variavel para conversão de endereço virtual para físico.
extern uint32_t kvirt_to_phys;

// Task que esta atualmente executando.
extern task_t *task_curr;

// Array com todos os slots e tasks.
extern task_t tasks[NUM_TASKS];

// Suspende(hlt) o processador.
extern void halt(void);

extern void __exit(void);

#endif
