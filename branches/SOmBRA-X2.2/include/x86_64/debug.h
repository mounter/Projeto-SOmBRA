#ifndef _X86_64_DEBUG_H
#define _X86_64_DEBUG_H

#include <x86_64/regs.h>

#define dump_int	asm ("int3\n")

void dump_panic(uregs_t *regs, const char *fmt, ...);
void dump(void *data_p, unsigned count);
void dump_iopb(void);
void dump_uregs(uregs_t *regs);
void dump_rip(rregs_t eip, int skip, unsigned inst_count);

#endif
