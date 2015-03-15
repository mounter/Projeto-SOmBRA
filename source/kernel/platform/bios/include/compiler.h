/**
 * Contem alguns defines e macros especificos
 * para cada compilador.
 *
 * @date 15-03-2015
 */

#ifndef __COMPILER_H
#define __COMPILER_H

#ifdef __GNUC__
# define __unused		       __attribute__((unused))
# define __used			       __attribute__((used))
# define __packed		       __attribute__((packed))
# define __aligned(a)		   __attribute__((aligned(a)))
# define __noreturn		       __attribute__((noreturn))
# define __malloc		       __attribute__((malloc))
# define __printf(a, b)		   __attribute__((format(printf, a, b)))
# define __deprecated		   __attribute__((deprecated))
# define __always_inline	   __attribute__((always_inline))
# ifdef __clang_analyzer__
#  define __init_text
#  define __init_data
#  define __section(s)
#  define __export
# else
#  define __init_text		    __attribute__((section(".init.text")))
#  define __init_data		    __attribute__((section(".init.data")))
#  define __section(s)		    __attribute__((section(s)))
#  define __hidden		        __attribute__((visibility("hidden")))
#  define __export		        __attribute__((visibility("default")))
# endif
# define __cacheline_aligned	__aligned(CPU_CACHE_SIZE)
# define likely(x)		        __builtin_expect(!!(x), 1)
# define unlikely(x)		    __builtin_expect(!!(x), 0)
# define compiler_barrier()	    __asm__ volatile("" ::: "memory")
#else
# error "SOmBRA does not currently support compilers other than GCC or CLang"
#endif

#define STRINGIFY(val)		#val
#define XSTRINGIFY(val)		STRINGIFY(val)

#endif // __COMPILER_H
