#ifndef _SRC_X86_SYSCALLS_H
#define _SRC_X86_SYSCALLS_H

#include <x86/syscall.h>

int getprocess(ps_t *buffer, uint32_t n);
uintptr_t ubrk(int size);

#endif

