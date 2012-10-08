#include <thread.h>
#include <task.h>
#include <schedule.h>
#include <mm.h>
#include <stdio.h>
#include <drivers/kbd.h>
#include <drivers/floppy.h>
#include <x86/page.h>
#include <string.h>

/* Contagem inicial do pid. */
uint32_t kpid = 200;

/* Array que irá guardar todas os processos. */
task_t tasks[NUM_TASKS] = {
	/* Inicialização da task #0 - idle. */
	{
		0,				/* kstack (dummy value) */
		0,				/* as (dummy value) */
		0,
		THREAD_MAGIC,	/* magic */
		1,				/* used */
		TS_RUNNABLE,	/* status */
		0,
		-1				/* priority (-1 for idle thread) */
	}
};

/* task_curr é a task que está executando atualmente, inicialmente é a task idle. */
task_t *task_curr = tasks + 0;

/* Fila das tarefas mortas. */
wait_queue_t task_died;


/* Acorda a primeira tarefa da fila 'queue'. */
void wake_up(wait_queue_t *queue){
	task_t *thread, *next;

	/* Verifica se a fila não está vazia. */
	thread = queue->head;
	if(thread == NULL){
		return;
	}

	if (thread->pid==0){
		return;
	}

	/* Acorda a tarefa(basta colocar ele com o status TS_RUNNABLE). */
	thread->status = TS_RUNNABLE;

	/* Remove a tarefa da fila. */
	next = thread->next;
	queue->head = next;
	if(next != NULL){
		next->prev = NULL;
	}
	else{
		queue->tail = NULL;
	}
}


/* Coloca a tarefa atual(task_curr) para dormir. */
int sleep_on_without_lock(wait_queue_t *queue, unsigned *timeout, spinlock_t *lock){
	task_t *prev;

	/* Coloca a tarefa atual para dormir. */
	task_curr->status = TS_BLOCKED;

	/* Coloca a thread dentro da fila de threads dormindo. */
	prev = queue->tail;
	queue->tail = task_curr;
	if(prev == NULL){
		queue->head = task_curr;
		task_curr->prev = NULL;
	}
	else{
		task_curr->prev = prev;
		prev->next = task_curr;
	}

	task_curr->next = NULL;

	/* Seta o timeout, se não houver o timeout é igual a zero. */
	if(timeout != NULL){
		task_curr->timeout = *timeout;
	}
	else{
		task_curr->timeout = 0;
	}

	if (lock != NULL)
		SPINLOCK_LEAVE(lock);

	/* Escalona os processos. */
	schedule();

	/* Atualizamos o timeout. */
	if(timeout != NULL){
		*timeout = task_curr->timeout;
		if(*timeout == 0){
			return -1;
		}
	}


	return 0;
}

/* Coletor de tarefas mortas. */
void dead_task_collector(){
	unsigned i;

	while(1){
		sleep_on(&task_died, NULL); /* sem timeout. */
		/* Verifica se há alguma tarefa morta. */
		for(i = 0; i < NUM_TASKS; i++){
			/* Se a tarefa não estiver morta não é para fazer nada. */
			if(tasks[i].status != TS_ZOMBIE){
				continue;
			}
			/* Caso contrário devemos destruir a tarefa, para poder liberar o slot. */
			/* printf("Tarefa destruida: %d\n", i); */
			task_destroy(tasks + i);
		}
	}
}

/* Cria uma thread, ela retorna um task_t* porque utilizamos
   a mesma estrutura para armazenar threads e tasks. */
task_t *thread_create(unsigned init_eip, int priority, uint32_t ppid, char *name){
	kregs_t *kregs;
	task_t *t;
	unsigned i;

	/* Procura um slot vazio. */
	for(i = 0;i<NUM_TASKS; i++) {
		if(!tasks[i].used) break;
	}

	if(i >= NUM_TASKS){
		return NULL;
	}
	/* Seta o ponteiro para o slot vazio. */
	t = tasks + i;

	/* Aloca memória para a pilha do kernel. */
	t->kstack_mem = malloc(KRNL_STACK_SIZE);
	if(t->kstack_mem == NULL){
		free(t);
		return NULL;
	}

	/* Seta alguns valores importantes sobre a thread. */
	strncpy(t->cmd, name, 16);
	t->wait4pid 	= false;
	t->ppid   		= ppid;
	t->pid	  		= kpid++;
	t->status 		= TS_RUNNABLE;
	t->priority 	= priority;
	t->magic 		= THREAD_MAGIC;
	t->used 		= 1;

	/* Coloca o primeiro contexto(kregs) na pilha para ser restaurando pelo primeiro switch_to(). */
	t->kstack = t->kstack_mem + KRNL_STACK_SIZE - sizeof(kregs_t);
	kregs = (kregs_t *)t->kstack;

	kregs->eip 		= init_eip; /* Seta a entrada da função que será uma thread. */
	kregs->eflags 	= 0x200; 	/* Para threads do kernel, nós habilitamos as interrupções. */

	return t;
}

/* Inicia as threads do sistema. */
void thread_init()
{
	unsigned num_threads;
	task_t *t;

	/* Seta o número inicial de tarefas, lembrando que a task idle é inserida em tempo de compilação. */
	num_threads = 1;
	printf("Iniciando threads: ");

	/* Cria uma tarefa para keyboard_bh(monitor de teclas dos consoles). */
	t = thread_create((unsigned)keyboard_bh, +1, 100, "keyboard");
	if(t == NULL){
		panic("Nao foi possivel criar a thread do keyboardbh.");
	}
	num_threads++;

#if 0
	/* Cria uma tarefa para keyboard_bh(monitor de teclas dos consoles). */
/*	t = create_thread((unsigned)floppy_thread, +1, 100, "disquete");
	if(t == NULL){
		panic("Nao foi possivel criar a thread do disquete.");
	}
	num_threads++;*/
#endif

	/* Cria o dead_task_collector para coletar as tarefas mortas. */
	t = thread_create((unsigned)dead_task_collector, +1, 100, "colletor");
	if(t == NULL){
		printf("Nao foi possivel criar a thread para captura tarefas mortas.\n");
	}

	t->pagedir = cr3_read();

	printf("Iniciando %d threads do kernel.\n", num_threads);
}

/* Destroi uma tarefa. */
void thread_destroy(task_t *t){
	if(t->magic != THREAD_MAGIC){
		panic("Argumento inválido no destroy_thread(%x).", t);
	}
	/* Libera a memória alocada para a tarefa. */
	free(t->kstack_mem);

	/* Ao fazer o memset estamos: zerando o used e o status.*/
	memset(t, 0, sizeof(task_t));

}

/* Finaliza a execução de uma tarefa. */
void taskexit(int code){
	int i;
	task_curr->exit_code = code;
	task_curr->status = TS_ZOMBIE;
	wake_up(&task_died);

	// Acorda o pai que estava esperando o fim da execução do filho.
	for (i=0;i<NUM_TASKS;i++){
		if (tasks[i].pid==task_curr->ppid){
			// Só desbloqueia o processo se o processo está dormindo por causa
			// da syscall wait4pid.
			if (tasks[i].wait4pid){
				tasks[i].wait4pid	= false;
				tasks[i].status		= TS_RUNNABLE;
			}
		}
	}
	/* Escalona os processos. */
	schedule();
}

