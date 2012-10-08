#ifndef ASPACE_H
#define ASPACE_H

#include <kernel.h>

#define	AS_MAGIC	0xBABA11

aspace_t *aspace_create();
void aspace_destroy(aspace_t *as);
int aspace_section_add(aspace_t *as, unsigned adr, unsigned size, unsigned flags, unsigned long offset);

#endif
