#ifndef _X86_COMMON_TYPES_H
#define _X86_COMMON_TYPES_H

#include <stdint.h>

#ifdef __x86_64__
#	define PAGE_ENTRY_COUNT	512
#else
#	define PAGE_ENTRY_COUNT	1024
#endif

// Definições e tipos para páginação!
typedef int_arch_t	page_t;
typedef uint_arch_t	frame_t;

#endif

