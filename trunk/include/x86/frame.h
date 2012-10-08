#ifndef _X86_FRAME_H
#define _X86_FRAME_H

#include "_heap.h"

#define NOFRAMES    0xFFF00000

typedef unsigned    frame_t;

int frame_init();
unsigned frame_get(void);
void frame_free(unsigned f);
int frame_check(unsigned frame);
unsigned frame_free_count(void);

#endif
