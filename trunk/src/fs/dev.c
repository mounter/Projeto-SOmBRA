#include <fsdev.h>
#include <fs.h>
#include "condev.h"

static fs_node_t *fs_dev_node = NULL;

static struct dirent *fs_dev_readdir(fs_node_t *node, uint32_t index)
{
    return NULL;
}

static fs_node_t *fs_dev_finddir(fs_node_t *node, char *name)
{
    return NULL;
}

fs_node_t *dev_init_node(fs_node_t *fs_node)
{
    fs_dev_node = fs_create_node(
                    "dev",
                    FS_DIRECTORY,
                    fs_node,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    fs_dev_readdir,
                    fs_dev_finddir
                );

    condev_init(fs_dev_node);

    return fs_dev_node;
}
