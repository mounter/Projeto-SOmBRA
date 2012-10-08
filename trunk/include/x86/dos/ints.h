#ifndef _X86_DOS_INTS_H
#define _X86_DOS_INTS_H

int dos_int20h(uregs_t *regs);
int dos_int21h(uregs_t *regs);

#endif
