#include <fs.h>
#include "lineint.h"

static struct dirent        dirent = { NULL, 0 };

uint32_t read_fs(fs_node_t *node, uint8_t *buffer, uint32_t size)
{
    if (node->read != NULL)
        return node->read(node, buffer, size);

    return 0;
}

uint32_t write_fs(fs_node_t *node, uint8_t *buffer, uint32_t size)
{
    if (node->write != NULL)
        return node->write(node, buffer, size);

    return 0;
}

void open_fs(fs_node_t *node, uint8_t read, uint8_t write)
{
    if (node->open != NULL)
        node->open(node, 0);
}

void close_fs(fs_node_t *node)
{
    if (node->close != NULL)
        node->close(node);
}

uint32_t select_fs(fs_node_t *node, uint8_t access, uint32_t *timeout)
{
    if (node->select != NULL)
        node->select(node, access, timeout);

    return 0;
}

struct dirent *readdir_fs(fs_node_t *node, uint32_t index)
{
    if ((node->flags & 0x7) == FS_DIRECTORY && node->readdir != NULL)
    {
        if (node->fs_child_nodes != NULL)
        {
            unsigned i = 0;
            fs_node_t *fs_node = node->fs_child_nodes;

            do
            {
                if (i == index)
                {
                    if (dirent.name != NULL) free(dirent.name);
                    dirent.name = malloc(strlen(fs_node->name));
                    strcpy(dirent.name, fs_node->name);
                    dirent.ino = fs_node->inode;

                    return &dirent;
                }

                fs_node = fs_node->fs_next_node;
                i++;
            } while (fs_node != node->fs_child_nodes);

            index -= i;
        }

        return node->readdir(node, index);
    }

    return NULL;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name)
{
    if ((node->flags & 0x7) == FS_DIRECTORY && node->finddir != NULL)
    {
        if (node->fs_child_nodes != NULL)
        {
            fs_node_t *fs_node = node->fs_child_nodes;

            do
            {
                if (!strcmp(fs_node->name, name))
                    return fs_node;

                fs_node = fs_node->fs_next_node;
            } while (fs_node != node->fs_child_nodes);
        }

        return node->finddir(node, name);
    }

    return NULL;
}

fs_node_t *search_path(fs_node_t *node, char *path)
{
    fs_node_t *fs_node_at = node;
    unsigned i;

    lineint_init(path);
    for (i = 0; i < lineint_count(); i++)
    {
        fs_node_at = finddir_fs(fs_node_at, lineint_index(i));

        if (fs_node_at == NULL)
            break;
    }

    return fs_node_at;
}
