#include <kernel/semaphore.h>
#include <kernel/schedule.h>
#include <kernel/mm.h>
#include <stdio.h>

// down
void sem_p(semaphore_t *s)
{
	SPINLOCK_ENTER(&s->lock);

	if(s->counter > 0) 
	{
		s->counter--;
		SPINLOCK_LEAVE(&s->lock);
		return; 
	}

	/*adds current thread t queue tail and marks as blocked */ 
	sleep_on_without_lock(&s->sema_wait, NULL, &s->lock);
	atomic_dec(&s->counter);
}

// up
void sem_v(semaphore_t *s)
{
	SPINLOCK_ENTER(&s->lock);
	if (s->counter <= 0) {
		s->counter = 0;

		wake_up(&s->sema_wait);
	}
	s->counter++;
	SPINLOCK_LEAVE(&s->lock);
}

void sem_init(semaphore_t *s, int i)
{
	s->counter = i;
	s->lock = 0;
}

