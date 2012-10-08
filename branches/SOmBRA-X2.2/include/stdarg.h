#ifndef __TL_STDARG_H
#define	__TL_STDARG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <_va_list.h>

#define va_start(v, f) __builtin_va_start(v, f)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v, a)   __builtin_va_arg(v, a)

#ifdef ___OK_

/* width of stack == width of int */
//#define	STACKITEM	int
#define STACKITEM long long int

/* round up width of objects pushed on stack. The expression before the
& ensures that we get 0 for objects of size 0. */
#define	VA_SIZE(TYPE)					\
	((sizeof(TYPE) + sizeof(STACKITEM) - 1)	\
		& ~(sizeof(STACKITEM) - 1))

/* &(LASTARG) points to the LEFTMOST argument of the function call
(before the ...) */
#define	va_start(AP, LASTARG)	\
	(AP=((va_list)&(LASTARG) + VA_SIZE(LASTARG)))

#define va_end(AP)	/* nothing */

#define va_arg(AP, TYPE)	\
	(AP += VA_SIZE(TYPE), *((TYPE *)(AP - VA_SIZE(TYPE))))

#endif // == _OK

#ifdef __cplusplus
}
#endif

#endif

