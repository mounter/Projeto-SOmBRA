#include <fs.h>
#include <kernel/pipe.h>
#include <stdio.h>

static uint32_t pipefs_read(fs_node_t *node, uint8_t *buffer, uint32_t size)
{
	pipe_t *pipe = (pipe_t*)node->inode;

	return pipe_read(buffer, size, pipe);
}

static uint32_t pipefs_write(fs_node_t *node, uint8_t *buffer, uint32_t size)
{
	pipe_t *pipe = (pipe_t *)node->inode;

	return pipe_write(buffer, size, pipe);
}

static void pipefs_close(fs_node_t *node)
{
	pipe_t *pipe = (pipe_t *)node->inode;

	int i = atomic_inc(&pipe->pipe_opened_count);

	fs_destroy_node(node);

	if (i > 1)
		pipe_destroy(pipe);
}

int pipefs_mkpipe(fs_node_t *fs_pipe[])
{
	fs_pipe[0] = fs_create_node(
                    "",           /* Nome do arquivo */
                    FS_PIPE,   	    /* Flags */
                    NULL,           /* Parente */
                    pipefs_read,    /* Funções de leitura */
                    NULL,           /* Função de escrita */
                    NULL,           /* Função de abertura */
                    pipefs_close,   /* Função de fechamento */
                    NULL,	    /* Leitura de nodo */
                    NULL	    /* Procurando um nodo */
                );

	fs_pipe[1] = fs_create_node(
                    "",           /* Nome do arquivo */
                    FS_PIPE,   	    /* Flags */
                    NULL,           /* Parente */
                    NULL,           /* Funções de leitura */
                    pipefs_write,   /* Função de escrita */
                    NULL,           /* Função de abertura */
                    pipefs_close,   /* Função de fechamento */
                    NULL,	    /* Leitura de nodo */
                    NULL	    /* Procurando um nodo */
                );

	pipe_t *pipe = pipe_create();

	fs_node_setINode(fs_pipe[0], (uint32_t)pipe);
	fs_node_setINode(fs_pipe[1], (uint32_t)pipe);

	return 0;
}

