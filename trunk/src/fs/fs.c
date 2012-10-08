#define _FS_C
#include <fs.h>

fs_node_t *fs_root = NULL;
fs_node_t *fs_home = NULL;

fs_node_t *fs_node_index(fs_node_t *fs_node, int index)
{
    fs_node_t *res_node = fs_node;

    if (fs_node == NULL)
        return NULL;

    int i;

    for (i = 0; (i < index) && (res_node != NULL); i++)
        res_node = res_node->fs_next_node;

    if (res_node == NULL && i != index)
        return NULL;

    return res_node;
}

fs_node_t *fs_link_node(fs_node_t *fs_node, fs_node_t *fs_new_node)
{
    if (fs_node == NULL)
    {
        return fs_new_node;
    }

    fs_node->fs_prior_node->fs_next_node = fs_new_node;
    fs_new_node->fs_prior_node = fs_node->fs_prior_node;
    fs_new_node->fs_next_node = fs_node;
    fs_node->fs_prior_node = fs_new_node;

    return fs_node;
}

fs_node_t *fs_unlink_node(fs_node_t *fs_node)
{
    if (fs_node == NULL)
        return NULL;

    if (fs_node->fs_next_node == fs_node)
        return fs_node->fs_parent_node = fs_node->fs_prior_node = fs_node->fs_next_node = NULL;

    fs_node_t *fs_next_node = fs_node->fs_next_node;
    fs_next_node->fs_prior_node = fs_node->fs_prior_node;
    fs_next_node->fs_prior_node->fs_next_node = fs_next_node;

    fs_node->fs_parent_node = fs_node->fs_prior_node = fs_node->fs_next_node = NULL;

    return fs_next_node;
}

inline fs_node_t *fs_create_node(
    char            *name,
    uint32_t        flags,
    fs_node_t       *fs_parent,
    read_type_t     read,
    write_type_t    write,
    open_type_t     open,
    close_type_t    close,
    readdir_type_t  readdir,
    finddir_type_t  finddir)
{
    fs_node_t *fs_node = zmalloc(sizeof(fs_node_t));

    fs_node->name = zmalloc(strlen(name)+1);
    strncpy(fs_node->name, name, FS_NAME_SZ);
    fs_node->flags = flags;
    fs_node->magic = FS_MAGIC;
    fs_node->read = read;
    fs_node->write = write;
    fs_node->open = open;
    fs_node->close = close;
    fs_node->readdir = readdir;
    fs_node->finddir = finddir;

    fs_node->fs_parent_node = fs_parent;
    fs_node->fs_prior_node = fs_node;
    fs_node->fs_next_node = fs_node;

    if (fs_parent != NULL)
        fs_parent->fs_child_nodes = fs_link_node(fs_parent->fs_child_nodes, fs_node);

    return fs_node;
}

inline fs_node_t *fs_clone_node(fs_node_t *fs_node)
{
    fs_node_t *fs_clone = zmalloc(sizeof(fs_node_t));

    memcpy(fs_clone, fs_node, sizeof(fs_node_t));

    fs_clone->name = zmalloc(strlen(fs_node->name)+1);
    strcpy(fs_clone->name, fs_node->name);

    fs_clone->fs_parent_node = NULL;
    fs_clone->fs_child_nodes = NULL;
    fs_clone->fs_prior_node = fs_clone->fs_next_node = fs_clone;

    return fs_clone;
}

inline fs_node_t *fs_destroy_node(fs_node_t *fs_node)
{
    if (fs_node == NULL)
        return NULL;

    fs_node_t *pnode;

    // Checando se é um nodo válido
    if (fs_node->magic != FS_MAGIC)
        return NULL;

//    if (fs_node == fs_node->fs_next_node)
//        pnode = NULL;
//    else
//        pnode = fs_node->fs_next_node;

    pnode = fs_unlink_node(fs_node);
    free(fs_node->name);
    free(fs_node);

    return pnode;
}

inline void fs_node_setINode(fs_node_t *fs_node, uint32_t inode)
{
    fs_node->inode = inode;
}
