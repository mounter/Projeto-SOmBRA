#include <kernel/pipe.h>
#include <kernel/mm.h>
#include <kernel/schedule.h>
#include <kernel/queue.h>
#include <stdio.h>

pipe_t	*pipe_create(void)
{
	pipe_t *pipe = zmalloc(sizeof(pipe_t));

	if (pipe == NULL) return NULL;

	pipe->pipe_magic = PIPE_MAGIC;
	sem_init(&pipe->pipe_sema_empty, PIPE_SZ-1);
	sem_init(&pipe->pipe_sema_full, 0);
	sem_init(&pipe->pipe_sema_mutex, 1);

	if (!queue_init(&pipe->pipe_queue, PIPE_SZ)) {
		pipe_destroy(pipe);
		return NULL;
	}

	return pipe;
}

int pipe_destroy(pipe_t *pipe)
{
	if (pipe == NULL) return 1;

	if (pipe->pipe_magic != PIPE_MAGIC) panic("PIPE_DESTROY - pipe incorreto!");

	queue_destroy(&pipe->pipe_queue);
	free(pipe);

	return 0;
}

size_t	pipe_read(void *ptr, size_t sz, pipe_t *pipe)
{
	int i;
	unsigned char *s = (unsigned char *)ptr;

	if (pipe->pipe_magic != PIPE_MAGIC) panic("PIPE_READ - pipe incorreto!");

	SPINLOCK_ENTER(&pipe->pipe_reader_lock);

	for (i=0; i < sz; i++)
	{
		sem_p(&pipe->pipe_sema_full);
		sem_p(&pipe->pipe_sema_mutex);

		if (!queue_get_data(&pipe->pipe_queue, &s[i])) panic("PIPE - Buffer vazio!");

		sem_v(&pipe->pipe_sema_mutex);
		sem_v(&pipe->pipe_sema_empty);
	}

	SPINLOCK_LEAVE(&pipe->pipe_reader_lock);

	return sz;
}

size_t	pipe_write(void *ptr, size_t sz, pipe_t *pipe)
{
	int i;
	unsigned char *s = (unsigned char *)ptr;

	if (pipe->pipe_magic != PIPE_MAGIC) panic("PIPE_WRITER - pipe incorreto!");

	SPINLOCK_ENTER(&pipe->pipe_writer_lock);

	for (i=0; i < sz; i++)
	{
		sem_p(&pipe->pipe_sema_empty);
		sem_p(&pipe->pipe_sema_mutex);

		if (!queue_insert_data(&pipe->pipe_queue, s[i])) panic("PIPE - Buffer cheio!");

		sem_v(&pipe->pipe_sema_mutex);
		sem_v(&pipe->pipe_sema_full);
	}

	SPINLOCK_LEAVE(&pipe->pipe_writer_lock);

	return sz;
}

