#ifndef _KERNEL_H
#define _KERNEL_H

#include <stdint.h>
#include <arch.h>
#include _REGS_H_

#define MAX_CONSOLE	8

/**************************** CONTROLE DE TAREFAS/THREADS ***************************/

#define	NUM_TASKS		32		// N�mero m�ximo de tarefas.
#define	KRNL_STACK_SIZE	8192	// Tamanho da pilha do kernel.

#define	THREAD_MAGIC	0x1F9D	// Deve ser menor que 0x8000
#define	HZ				100		// Frequencia do timer do controlador 8253

// Flags utilizados para descrever os segmentos.
#define	SF_ZERO		0x10	// BSS - zerar antes de usar
#define	SF_LOAD		0x08	// Lido de um arquivo.
#define	SF_READ		0x04	// Leitura
#define	SF_WRITE	0x02	// Escrita
#define	SF_EXEC		0x01	// Execut�vel
#define	SF_BSS		(SF_ZERO | SF_READ | SF_WRITE)

// Estrutura que descreve um setor(segmento).
typedef struct{
	uint_arch_t adr, size, flags;
	uint_arch_t offset;
} sect_t;

// Descreve onde o processo e os seus segmentos est�o armazenados na mem�ria.
typedef struct
{
	char *user_mem;		// Mem�ria alocada para a tarefa (kmalloc).
	uintptr_t	virt_adr;	// Menor valor do endere�o virtual da tarefa.
	uint_arch_t	size;		// Tamanho total da tarefa.

	unsigned magic, num_sects;
	sect_t *sect;
	sect_t heap;
} aspace_t;

// Estrutura que descreve uma tarefa.
// kstack deve ser o primeiro item devido ao start.S
typedef struct _task
{
	char *kstack;				// Valor atual do RSP/ESP do kernel(ring 0).
	aspace_t *as;
	uintptr_t pagedir;

	unsigned short int magic;		// N�mero m�gico para a valida��o.
	char used;				// Flag indicando se a estrutura est� livre ou n�o.

	enum{
		TS_RUNNABLE = 1, TS_BLOCKED = 2, TS_ZOMBIE = 3
	} status;				// Status do processo.

	int priority;				// Prioridade do processo.
	int exit_code;				// C�digo de sa�da do processo.

	unsigned timeout;			// Timeout
	char *kstack_mem;			// Pilha do kernel(kmalloc).
	struct _task *prev, *next;		// Usado na navega��o das tarefas.
	//file_t *files[NUM_FILES];		// Descritores de arquivo.
	//fs_node_t *files;           		// Descritores de arquivo


	bool     wait4pid;			// Indica se o processo est� esperando por Wait4Pid.
	uint32_t pid;				// PID do processo.
	uint32_t ppid;				// PID do pai.
	char     cmd[16];			// Nome do processo.
	int16_t  console;			// Console.
} task_t;


// Estrutura que descreve uma fila
typedef struct{
	struct _task *head, *tail;
} wait_queue_t;

// Estrutura que descreve uma fila circular de tamanho m�ximo "size".
typedef struct {
	unsigned char *data;

	uint16_t size;
	uint16_t id_last;
	uint16_t id_first;
} queue_t;

// Variavel para convers�o de endere�o virtual para f�sico.
extern uintptr_t kvirt_to_phys;

// Task que esta atualmente executando.
extern task_t *task_curr;

// Array com todos os slots e tasks.
extern task_t tasks[NUM_TASKS];

// Suspende(hlt) o processador.
extern void halt(void);

extern void __exit(void);

#endif
