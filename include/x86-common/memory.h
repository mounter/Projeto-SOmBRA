#ifndef _X86_MEMORY_H
#define _X86_MEMORY_H

#define STACK_SIZE 4096

// Definição de segmentos
#define GDT_D_LDT           0x200	// LDT segment
#define GDT_D_TASK          0x500	// Task gate
#define GDT_D_TSS           0x900	// TSS
#define GDT_D_CALL          0xC00	// 386 call gate
#define GDT_D_INT           0xE00	// 386 interrupt gate
#define GDT_D_TRAP          0xF00	// 386 trap gate
#define GDT_D_DATA          0x1000	// Data segment
#define GDT_D_CODE          0x1800	// Code segment

// Descriptors may include the following as appropriate:
#define GDT_D_DPL3          0x6000  // DPL3 or mask for DPL
#define GDT_D_DPL2          0x4000
#define GDT_D_DPL1          0x2000
#define GDT_D_PRESENT       0x8000  // Present
#define GDT_D_NOT_PRESENT   0x8000	// Not Present

// Segment descriptors (not gates) may include:
#define GDT_D_ACC           0x100	// Accessed (Data or Code)
#define GDT_D_WRITE         0x200	// Writable (Data segments only)
#define GDT_D_READ          0x200	// Readable (Code segments only)
#define GDT_D_BUSY          0x200	// Busy (TSS only)
#define GDT_D_EXDOWN        0x400	// Expand down (Data segments only)
#define GDT_D_CONFORM       0x400	// Conforming (Code segments only)

#define GDT_D_BIG           0x40	// Default to 32 bit mode (USE32)
#define GDT_D_BIG_LIM       0x80	// Limit is in 4K units

#ifdef __ASSEMBLER__

%macro GDT_DESCRIPTOR   7
global %1
%1:
	dw %2		// limit 15:0
	dw %3		// base 15:0
	db %4		// base 23:16
	db %5		// type
	db %6		// limit 19:16, flags
	db %7		// base 31:24
%endmacro

%macro GDT64_DESCRIPTOR   8
global %1
%1:
	dw %2		// limit 15:0
	dw %3		// base 15:0
	db %4		// base 23:16
	db %5		// type
	db %6		// limit 19:16, flags
	db %7		// base 31:24
	dd %8		// base 63:32
	dd 0		// reserved
%endmacro

#define GDT_NULL    GDT_DESCRIPTOR gdt_null, 0, 0, 0, 0, 0, 0

#endif

// Segmentos do nosso núcleo
#define GDT_TSS     0x08
#define GDT_KCODE   0x10
#define GDT_KDATA   0x18
#define GDT_UCODE   (0x20 | 3)
#define GDT_UDATA   (0x28 | 3)

// Segmentos do nosso núcleo X86_64
// Faltando segmentos para binarios de 32-bit
#define GDT64_KCODE   0x08
#define GDT64_KDATA   0x10
#define GDT64_UCODE   (0x18 | 3)
#define GDT64_UDATA   (0x20 | 3)
#define GDT64_TSS     0x28

#endif
