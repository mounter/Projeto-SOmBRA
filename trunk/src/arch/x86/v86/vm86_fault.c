#include <x86/x86.h>
#include V86_H
#include ISR_H
#include DEBUG_H
#include DOS_H
#include <stdio.h>
#include <setjmp.h>
#include <kernel.h>

extern jmp_buf g_oops;
extern int page_fault(uregs_t *regs);

// Vetor de chamadas de interrupções!
isr_call_vector_t   v86_isr_call_vector;
isr_call_vector_t   *v86_isr_aux;

extern int fault(uregs_t *regs);

// Inicializando o controlador de ISR
void v86_isr_init(void)
{
    int i;

    for (i = 0; i < 256; i++)
        v86_isr_call_vector[i] = v86_isr_fault;

//    v86_isr_call_vector[0x0E] = page_fault;
    v86_isr_call_vector[0x20] = dos_int20h;
    v86_isr_call_vector[0x21] = dos_int21h;
}

//void v86_isr_end(void)
//{
//    isr_call_ptr_vector = v86_isr_aux;
//}

int v86_isr_fault(uregs_t *regs)
{
    static const char *msg[] =
	{
		"divide error", "debug exception", "NMI", "INT3",
		"INTO", "BOUND exception", "invalid opcode", "no coprocessor",
		"double fault", "coprocessor segment overrun",
			"bad TSS", "segment not present",
		"stack fault", "GPF", "page fault", "??",
		"coprocessor error", "alignment check", "??", "??",
		"??", "??", "??", "??",
		"??", "??", "??", "??",
		"??", "??", "??", "??",
		"IRQ0", "IRQ1", "IRQ2", "IRQ3",
		"IRQ4", "IRQ5", "IRQ6", "IRQ7",
		"IRQ8", "IRQ9", "IRQ10", "IRQ11",
		"IRQ12", "IRQ13", "IRQ14", "IRQ15",
		"syscall"
	};
/**/
	int i;

/* "reflect" hardware interrupts (IRQs) to V86 mode */
	if(regs->which_int >= 0x20 && regs->which_int < 0x30)
	{
/* for timer interrupt, blink character in upper left corner of screen */
//		if(regs->which_int == 0x20)
//			blink();
/* IRQs 0-7 -> pmode exception numbers 20h-27h -> real-mode vectors 8-15 */
		if(regs->which_int < 0x28)
			i = (regs->which_int - 0x20) + 8;
/* IRQs 8-15 -> pmode exception numbers 28h-2Fh -> real-mode vectors 70h-77h */
		else
			i = (regs->which_int - 0x28) + 0x70;
		v86_int(regs, i);
		return 0;
	}
/* stack fault from V86 mode with SP=0xFFFF means we're done */
	else if(regs->which_int == 0x0C)
	{
		if((regs->eflags & 0x20000uL) && regs->user_esp == 0xFFFF)
			return VM86_END;
	}
/* GPF from V86 mode: emulate instructions */
	else if(regs->which_int == 0x0D)
	{
		if(regs->eflags & 0x20000uL)
		{
			i = v86_emulate(regs);
/* error */
			if(i < 0)
				longjmp(g_oops, 1);
/* pop from empty stack: end V86 session */
			else if(i > 0)
				return VM86_END;
			return 0;
		}
	}
/* for anything else, display exception number,
exception name, and CPU mode */
	printf("*** Exception #%u ", regs->which_int);
	if(regs->which_int <= sizeof(msg) / sizeof(char *))
		printf("(%s) ", msg[regs->which_int]);
	if(regs->eflags & 0x20000uL)
		printf("in V86");
	else if(regs->cs & 0x03)
		printf("in user");
	else
		printf("in kernel");
	printf(" mode ***\n");
/* dump registers and longjmp() back to main() */
	dump_uregs(regs);
	__exit();
/* not reached */
	return -1;
}
