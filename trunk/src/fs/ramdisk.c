#include "ramdisk.h"
#include <string.h>
#include <stdio.h>
#include <kernel/mm.h>

static initrd_header_t      initrd_header;
static fs_node_t            *initrd_root;
fs_node_t                   *fs_initrd_node;
static uintptr_t            initrd_location;
static struct dirent        dirent = { NULL, 0 };

// Protótipos
inline void initrd_make_header(uintptr_t location, uintptr_t size);

void initrd_open(fs_node_t *node, uint32_t mask)
{
    node->offset = 0;
}

void initrd_close(fs_node_t *node)
{
    node->offset = 0;
}

static uint32_t initrd_read(fs_node_t *node, uint8_t *buffer, uint32_t size)
{
    if (node->inode >= initrd_header.node_count)
        return 0;

    initrd_file_t *file = (initrd_file_t*)initrd_header.inodes[node->inode];

    if (node->offset >= file->length)
        return 0;

    if ((node->offset + size) > file->length)
        size = file->length - node->offset;

    memcpy(buffer, (uint8_t*) (initrd_location + file->offset + node->offset), size);

    if ((node->offset + size) < file->length)
        node->offset += size;
    else
        node->offset = file->length;

    return size;
}

static struct dirent *initrd_readdir(fs_node_t *node, uint32_t index)
{
    if (index >= initrd_header.node_count)
        return NULL;

    // Checando o próprio dispositivo
    initrd_file_t *file = (initrd_file_t *)initrd_header.inodes[index];

    if (dirent.name != NULL) free(dirent.name);
    dirent.name = malloc(strlen(file->name)+1);
    strcpy(dirent.name, file->name);
    dirent.name[strlen(file->name)] = 0; // Make sure the string is NULL-terminated.
    dirent.ino = index;

    return &dirent;
}

static fs_node_t *initrd_finddir(fs_node_t *node, char *name)
{
    int i;

    for (i = 0; i < initrd_header.node_count; i++)
        if (!strcmp(name, ((initrd_file_t *)initrd_header.inodes[i])->name))
        {
            if (fs_initrd_node != NULL) fs_destroy_node(fs_initrd_node);

            fs_initrd_node = fs_create_node(
                        ((initrd_file_t *)initrd_header.inodes[i])->name,
                        FS_FILE,
                        NULL,
                        initrd_read,
                        NULL,
                        initrd_open,
                        initrd_close,
                        NULL,
                        NULL
                    );

            fs_node_setINode(fs_initrd_node, i);

            return fs_initrd_node;
        }

    return NULL;
}

#include "lineint.h"

fs_node_t *initialise_initrd(uintptr_t location, uintptr_t end_adr)
{
    initrd_location = location;

// Criando um nodo
    initrd_root = fs_create_node(
                    "initrd",       /* Nome do arquivo */
                    FS_DIRECTORY,   /* Flags */
                    NULL,           /* Parente */
                    NULL,           /* Funções de leitura */
                    NULL,           /* Função de escrita */
                    NULL,           /* Função de abertura */
                    NULL,           /* Função de fechamento */
                    initrd_readdir, /* Leitura de nodo */
                    initrd_finddir  /* Procurando um nodo */
                );

    initrd_make_header(location, end_adr);

    return initrd_root;
}

inline void initrd_make_header(uintptr_t location, uintptr_t end_adr)
{
    initrd_file_t *file;
    unsigned offset = location;

    initrd_header.node_count = 0;

    while (offset < end_adr)
    {
        file = (initrd_file_t *)offset;
        offset += sizeof(initrd_file_t);

        if (file->magic != INITRD_MAGIC)
            panic("INITRD está corrompida!\n");

        initrd_header.inodes[initrd_header.node_count++] = (uintptr_t)file;

        offset += file->length;
    }
}

