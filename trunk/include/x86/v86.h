#ifndef _X86_V86_H
#define _X86_V86_H

#include <x86/regs.h>

#define VM86_END    0xe8086

// Le de um seg:ofs
unsigned peekb(unsigned seg, unsigned off);
unsigned peekw(unsigned seg, unsigned off);
unsigned long peekl(unsigned seg, unsigned off);

// Escreve em um seg:ofs
void pokeb(unsigned seg, unsigned off, unsigned val);
void pokew(unsigned seg, unsigned off, unsigned val);
void pokel(unsigned seg, unsigned off, unsigned long val);

// Funções da vm86
void v86_int(uregs_t *regs, unsigned int_num);
int v86_emulate(uregs_t *regs);

void v86_start(uregs_t *regs);
void v86_end(void);

void v86_init_regs(uregs_t *regs);
void v86_call_int(uregs_t *regs, unsigned int_num);

void v86_isr_init(void);
int v86_isr_fault(uregs_t *regs);

void *v86_mem_read(void *dst, void *src, size_t sz);
void v86_video_write(void *ptr, unsigned ofs, size_t sz);

#endif
