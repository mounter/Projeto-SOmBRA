#ifndef _FS_PIPEFS_H
#define _FS_PIPEFS_H

#include <fs.h>
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

#endif

