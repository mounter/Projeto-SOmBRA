#ifndef _KERNEL_SEMAPHORE_H
#define _KERNEL_SEMAPHORE_H

#include <kernel/thread.h>
#include <kernel/spinlock.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int atomic_inc(int * i)
{
  int r = 1;
  asm volatile (
		"lock\n\t"
		"xadd %0, %1\n\t"
		:
		"=r" (r)
		:
		"m" (*i),
		"0" (r)
		:
		"memory"
		);
  return r;
}

static inline int atomic_dec(int * i)
{
  int r = -1;
  asm volatile (
		"lock\n\t"
		"xadd %0, %1\n\t"
		:
		"=r" (r)
		:
		"m" (*i),
		"0" (r)
		:
		"memory"
		);
  return r;
}
  

typedef struct {
  int		counter;
  spinlock_t	lock;
  wait_queue_t	sema_wait;
} semaphore_t;

extern void sem_p(semaphore_t *s);
extern void sem_v(semaphore_t *s);
extern void sem_init(semaphore_t *s, int i);

#ifdef __cplusplus
}
#endif

#endif

