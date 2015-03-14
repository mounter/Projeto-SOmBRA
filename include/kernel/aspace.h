#ifndef ASPACE_H
#define ASPACE_H

#include <kernel.h>

#define	AS_MAGIC	0xBABA11

aspace_t *aspace_create();
void aspace_destroy(aspace_t *as);
int aspace_section_add(aspace_t *as, uint_arch_t adr, uint_arch_t size, uint_arch_t flags, uint_arch_t offset);

#endif
