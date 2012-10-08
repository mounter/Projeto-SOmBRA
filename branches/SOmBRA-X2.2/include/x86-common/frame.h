#ifndef _X86_COMMON_FRAME_H
#define _X86_COMMON_FRAME_H

#include "_heap.h"

#ifdef __x86_64__
#	define NOFRAMES    0xFFF0000000000000
#else
#	define NOFRAMES    0xFFF00000
#endif

int frame_init();
frame_t frame_get(void);
void frame_free(frame_t f);
int frame_check(frame_t frame);
uint_arch_t frame_free_count(void);

#endif
