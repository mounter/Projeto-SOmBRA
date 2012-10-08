#include <x86/v86.h>
#include <x86/memory.h>
#include <string.h>

/*****************************************************************************
Sets up V86 mode stack and initializes some registers
*****************************************************************************/
void v86_init_regs(uregs_t *regs)
{
/* V86 mode stack. This MUST be in conventional memory (below 1 meg)
but still be accessible to the pmode kernel: */
	static char _v86_stack[4096];
/**/
	char *v86_stack;
	unsigned s;

/* start with all registers zeroed */
	memset(regs, 0, sizeof(uregs_t));
/* point to BOTTOM (highest addess) of stack */
	v86_stack = _v86_stack + sizeof(_v86_stack);
/* v86_stack is virtual address. Convert it to physical address
and align it so bottom 4 bits == 0x0F
's' must be >= 0x10000 so we can set ESP=0xFFFF, below. */
	s = ((unsigned)v86_stack - 16) | 0x0F;
/* init stack */
	regs->user_esp = 0xFFFF;
	regs->user_ss = (s - 0xFFFF) / 16;
/* init kernel data segment registers */
	regs->ds = regs->es = regs->fs = regs->gs = GDT_KCODE;
/* init EFLAGS: set RF=0, NT=0, IF=0, and reserved bits */
#if 1
	regs->eflags = 0x00020002L; /* VM=1, IOPL=0 */
#else
	regs->eflags = 0x00023002L; /* VM=1, IOPL=3 */
#endif
}
/*****************************************************************************
Call real-mode interrupt handler in V86 mode
*****************************************************************************/
void v86_call_int(uregs_t *regs, unsigned int_num)
{
	unsigned ivt_off;

/* convert int_num to IVT index */
	ivt_off = (int_num & 0xFF) * 4;
/* fetch CS:IP of real-mode interrupt handler */
	regs->cs = peekw(0, ivt_off + 2);
	regs->eip = peekw(0, ivt_off + 0);
/* do it */
	v86_start(regs);
}
