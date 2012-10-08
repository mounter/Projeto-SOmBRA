#ifndef _KERNEL_TASK_H
#define _KERNEL_TASK_H

#include <kernel.h>

#define USER_BASE	0x400000
#define USER_STACK_SIZE	0x2000

int task_create(char *name, char **argp, char **envp, char *image, uint32_t ppid);
void task_destroy(task_t *t);

#endif
