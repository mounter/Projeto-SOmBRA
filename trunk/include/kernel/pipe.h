#ifndef _PIPE_H
#define _PIPE_H

#include <kernel/thread.h>
#include <kernel/semaphore.h>
#include <kernel/spinlock.h>
#include "../_sizet.h"

#define PIPE_SZ		32
#define PIPE_MAGIC	0x4ABC5CBA

typedef struct {
	unsigned		pipe_magic;
	int			pipe_opened_count;
	semaphore_t		pipe_sema_empty;
	semaphore_t		pipe_sema_full;
	semaphore_t		pipe_sema_mutex;
	spinlock_t		pipe_reader_lock;
	spinlock_t		pipe_writer_lock;
	queue_t			pipe_queue;
} pipe_t;

pipe_t	*pipe_create(void);
size_t	pipe_read(void *ptr, size_t sz, pipe_t *pipe);
size_t	pipe_write(void *ptr, size_t sz, pipe_t *pipe);
int pipe_destroy(pipe_t *pipe);

#endif

