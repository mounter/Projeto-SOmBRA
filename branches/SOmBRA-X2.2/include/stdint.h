#ifndef _STDINT_H
#define _STDINT_H

// Tipos inteiros sem sinal
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;

#ifndef __x86_64__
typedef unsigned long long		uint64_t;
#else
typedef unsigned long			uint64_t;
#endif

// Tipos inteiros com sinal
typedef char                    int8_t;
typedef short int               int16_t;
typedef int                     int32_t;

#ifndef __x86_64__
typedef long long  				int64_t;
#else
typedef long 					int64_t;
#endif

// Tipos para ponteiros
typedef long             intptr_t;
typedef unsigned long    uintptr_t;

typedef long			int_arch_t;
typedef unsigned long	uint_arch_t;

#endif
