#include "syscalls.h"
#include <kernel/thread.h>

/* Pega os processos atuais. */
int getprocess(ps_t *buffer, uint32_t n)
{
	int i, num;
	ps_t  *ps;
	num = 0;
	ps  = buffer;
	/* Começa do 1 para não pega a task IDLE. */
	for (i=1;i<NUM_TASKS;i++){
		if (tasks[i].used==1){
			ps->pid    = tasks[i].pid;
			ps->ppid   = tasks[i].ppid;
			ps->status = tasks[i].status;
			strncpy(ps->cmd, tasks[i].cmd, 16);
			num++;
			ps++;
			if (num>=n){
				break;
			}
		}
	}
	return num;
}

