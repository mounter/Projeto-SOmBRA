#ifndef THREAD_H
#define THREAD_H

#include <kernel.h>
#include <kernel/spinlock.h>

extern task_t *curr_task;

void wake_up(wait_queue_t *queue);
void thread_init();
void thread_destroy(task_t *t);
int sleep_on_without_lock(wait_queue_t *queue, unsigned *timeout, spinlock_t *lock);
#define sleep_on(queue, timeout) sleep_on_without_lock(queue, timeout, (void*)0)
void taskexit(int code);
task_t *thread_create(uintptr_t init_ip, int priority, uint32_t ppid, char *name);

#endif
