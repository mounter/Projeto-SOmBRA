#ifndef _ELF_H
#define _ELF_H

#include <elf/elf.h>

#define ELF_MAG_CHECK(ident) (ident[EI_MAG0] == ELFMAG0 && ident[EI_MAG1] == ELFMAG1 && \
				ident[EI_MAG2] == ELFMAG2 && ident[EI_MAG3] == ELFMAG3)
#define ELF_ISVERSION(ident) (ident[EI_VERSION] == EV_CURRENT)

#define ISELF(ehdr, class, dtype) (ehdr->e_ident[EI_CLASS] == class && ehdr->e_ident[EI_DATA] == dtype)
#define ISELF32(ehdr) ISELF(ehdr, ELFCLASS32, ELFDATA2LSB)
#define ISELF64(ehdr) ISELF(ehdr, ELFCLASS64, ELFDATA2LSB)

int elf64_is_valid(void *image);

#endif
