#ifndef _X86_COMMON_PAGE_H
#define _X86_COMMON_PAGE_H

#ifdef __x86_64__
#	include <x86_64/page.h>
#else
#	include <x86/page.h>
#endif

#endif
