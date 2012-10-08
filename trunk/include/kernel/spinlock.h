#ifndef _KERNEL_SPINLOCK_H
#define _KERNEL_SPINLOCK_H

typedef int	spinlock_t;

static inline int xchg(volatile int *addr, int newval)
{   
	int result;

	// The + in "+m" denotes a read-modify-write operand.
	asm volatile("lock; xchgl %0, %1" :
		"+m" (*addr), "=a" (result) :
		"1" (newval) :
		"cc");
	return result;
} 

#define SPINLOCK_ENTER(spinlock)	while(xchg(spinlock, 1) == 1) schedule();
#define SPINLOCK_LEAVE(spinlock)	xchg(spinlock, 0);

#endif
