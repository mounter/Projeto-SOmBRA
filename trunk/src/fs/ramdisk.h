#ifndef _RAMDISK_H_
#define _RAMDISK_H_

#include <fs.h>

#define INITRD_MAX_FILES    64
#define INITRD_FILE_NAME_SZ 64
#define INITRD_MAGIC        0xBF

// Estrutura que representa o cabeçalho de um arquivo
typedef struct
{
   unsigned char magic; // The magic number is there to check for consistency.
   char name[INITRD_FILE_NAME_SZ];
   unsigned int offset; // Offset in the initrd the file starts.
   unsigned int length; // Length of the file.
} initrd_file_t;

typedef struct
{
    size_t node_count;
    uintptr_t inodes[INITRD_MAX_FILES];
} initrd_header_t;

// Initialises the initial ramdisk. It gets passed the address of the multiboot module,
// and returns a completed filesystem node.
//fs_node_t *initialise_initrd(uintptr_t location);

#endif
