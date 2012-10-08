#ifndef _X86__HEAP_H
#define _X86__HEAP_H

#define HEAP_ALLOC_SPACE(heap, sz) (void*)heap; heap += sz
#define HEAP_FRAMES (heap & 0xFFF ? (heap >> 12) + 1 : (heap >> 12))
//#define HEAP_ROUND  (heap & 0xFFF ? heap + 0x1000 : heap)

extern unsigned heap;

#endif
