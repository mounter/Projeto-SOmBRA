#ifndef _X86_REGS_H
#define _X86_REGS_H

#include <kernel/types.h>

typedef unsigned    regs_t;

// Estrutura para armazenar o contexto de uma aplicação(ring 3).
typedef struct{
	regs_t      edi, esi, ebp, esp, ebx, edx, ecx, eax;	// Para as instruções: PUSHA/POPA
	regs_t      ds, es, fs, gs;
	uint32_t    which_int, err_code;
	regs_t      eip, cs, eflags, user_esp, user_ss;		// Para as instruções: INT xx e IRET
	regs_t      v_es, v_ds, v_fs, v_gs;                 // Somente para modo v86
} uregs_t;

// Estrutura para armazenar o contexto de uma thread do kernel.
typedef struct{
	regs_t      ebx, esi, edi, ebp; // callee-save
	regs_t      eip, eflags;	    // CALL/IRET
} kregs_t;

#endif
