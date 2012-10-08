#include "ar.h"
#include <string.h>
#include <stdio.h>
#include <kernel/mm.h>

static ar_header_t          ar_header;
static fs_node_t            *ar_root;
fs_node_t                   *fs_ar_node;
static uintptr_t            ar_location;
static struct dirent        dirent = { NULL, 0 };

// Protótipos
inline void ar_make_header(uintptr_t location, uintptr_t size);

void ar_open(fs_node_t *node, uint32_t mask)
{
    node->offset = 0;
}

void ar_close(fs_node_t *node)
{
    node->offset = 0;
}

static uint32_t ar_read(fs_node_t *node, uint8_t *buffer, uint32_t size)
{
    if (node->inode >= ar_header.node_count)
        return 0;

    ar_file_t *file = (ar_file_t*)ar_header.inodes[node->inode];

    if (node->offset >= file->length)
        return 0;

    if ((node->offset + size) > file->length)
        size = file->length - node->offset;

    memcpy(buffer, (uint8_t*) (ar_location + file->offset + node->offset), size);

    if ((node->offset + size) < file->length)
        node->offset += size;
    else
        node->offset = file->length;

    return size;
}

static struct dirent *ar_readdir(fs_node_t *node, uint32_t index)
{
    if (index >= ar_header.node_count)
        return NULL;

    // Checando o próprio dispositivo
    ar_file_t *file = (ar_file_t *)ar_header.inodes[index];

    if (dirent.name != NULL) free(dirent.name);
    dirent.name = malloc(strlen(file->name)+1);
    strcpy(dirent.name, file->name);
    dirent.name[strlen(file->name)] = 0; // Make sure the string is NULL-terminated.
    dirent.ino = index;

    return &dirent;
}

static fs_node_t *ar_finddir(fs_node_t *node, char *name)
{
    int i;

    for (i = 0; i < ar_header.node_count; i++)
        if (!strcmp(name, ((ar_file_t *)ar_header.inodes[i])->name))
        {
            if (fs_ar_node != NULL) fs_destroy_node(fs_ar_node);

            fs_ar_node = fs_create_node(
                        ((ar_file_t *)ar_header.inodes[i])->name,
                        FS_FILE,
                        NULL,
                        ar_read,
                        NULL,
                        ar_open,
                        ar_close,
                        NULL,
                        NULL
                    );

            fs_node_setINode(fs_ar_node, i);

            return fs_ar_node;
        }

    return NULL;
}

#include "lineint.h"

fs_node_t *initialise_ar(uintptr_t location, uintptr_t end_adr)
{
    ar_location = location;

// Criando um nodo
    ar_root = fs_create_node(
                    "initrd",       /* Nome do arquivo */
                    FS_DIRECTORY,   /* Flags */
                    NULL,           /* Parente */
                    NULL,           /* Funções de leitura */
                    NULL,           /* Função de escrita */
                    NULL,           /* Função de abertura */
                    NULL,           /* Função de fechamento */
                    ar_readdir, /* Leitura de nodo */
                    ar_finddir  /* Procurando um nodo */
                );

    ar_make_header(location, end_adr);

    return ar_root;
}

static unsigned ar_name_sz(char *str)
{
    unsigned count = 0;


}

inline void ar_make_header(uintptr_t location, uintptr_t end_adr)
{
    ar_file_t *file;
    struct ar_hdr   *ar_hdr;
    unsigned offset = location;

    ar_header.node_count = 0;

    if (strncmp((char *)offset, ARMAG, SARMAG))
        panic("AR-INITRD está corrompida!\n");

    offset += SARMAG;

    while (offset < end_adr)
    {
        ar_hdr = (struct ar_hdr *)offset;

        if (strncmp(ar_hdr.ar_fmag, ARFMAG, 2))
            panic("AR-INITRD está corrompida!\n");

        file = (ar_file_t *)malloc(sizeof(ar_file_t));
        offset += sizeof(struct ar_hdr);

        if (file->magic != AR_MAGIC)
            panic("INITRD está corrompida!\n");

        ar_header.inodes[ar_header.node_count++] = (uintptr_t)file;

        offset += file->length;
    }
}

