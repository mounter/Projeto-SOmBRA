#include <schedule.h>
#include <kernel.h>
#include <stdio.h>

//extern task_t tasks[NUM_TASKS];
extern void switch_to(task_t *task);

// Escalonador de processos.
void schedule(){
	static unsigned current;
	int i, p = -127;
	task_t *t;

	// Verifica todas as tasks para ver se nenhuma está corrompida.
	for(i = 0; i < NUM_TASKS; i++){
		t = tasks + i;
		if(!t->used){
			continue;
		}
		if(t->magic != THREAD_MAGIC){
			panic("Task corrompida[%x].", t);
		}

		// Procura a maior prioridade das tasks que estão com o status TS_RUNNABLE.
		if(t->status!=TS_RUNNABLE){
			continue;
		}
		if(t->priority > p){
			p = t->priority;
		}

	}

	// Encontra a próxima tarefa a ser executada, ela deve ter prioridade "p".
	for(i=0;i<NUM_TASKS*2;i++){
		current++;
		if(current >= NUM_TASKS){
			current = 0;
		}
		t = tasks + current;
		if((t->status==TS_RUNNABLE)&&(t->priority>=p)){
			break;
		}
	}

	if(i >= NUM_TASKS * 2){
		panic("Loop infinito no schedule(nao há processos).");
	}

	// Muda o contexto para a nova tarefa.
	switch_to(t);
}
