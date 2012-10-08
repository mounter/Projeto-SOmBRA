#ifndef _LINEINT_H
#define _LINEINT_H

#include <kernel/types.h>

void lineint_init(char *path);
void lineint_intr(void);
size_t lineint_count(void);
char *lineint_index(unsigned index);

#endif
