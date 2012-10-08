#ifndef _X86_64_REGS_H
#define _X86_64_REGS_H

#include <kernel/types.h>

typedef uint8_t  bregs_t;
typedef uint16_t wregs_t;
typedef uint32_t eregs_t;
typedef uint64_t rregs_t;

// Estrutura para armazenar o contexto de uma aplicação(ring 3).
typedef struct {
	rregs_t gs, fs;
	rregs_t r15, r14, r13, r12, r11, r10, r9, r8;
	rregs_t rbp, rsi, rdi, rdx, rcx, rbx, rax;
	uint64_t which_int, err_code;
	rregs_t rip, cs, rflags, rsp, ss;
} uregs_t;

// Estrutura para armazenar o contexto de uma thread do kernel.
typedef struct{
	rregs_t      rbx, rbp, r12, r13, r14, r15; // callee-save
	rregs_t      rip, rflags;	    // CALL/IRET
} kregs_t;

#define REG_E(x) (eregs_t)(x & 0xFFFFFFFF)
#define REG_W(x) (wregs_t)(x & 0xFFFF)
#define REG_LB(x) (bregs_t)(x & 0xFF)
#define REG_HB(x) (bregs_t)((x >> 8) & 0xFF)

#endif

