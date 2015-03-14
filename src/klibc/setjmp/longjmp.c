#include <setjmp.h> /* jmp_buf */
/*****************************************************************************
*****************************************************************************/
void longjmp(jmp_buf buf, int ret_val)
{
	unsigned *esp;

/* make sure return value is not 0 */
	if(ret_val == 0)
		ret_val++;
/* EAX is used for return values, so store return value in jmp_buf.EAX */
	buf->eax = ret_val;
/* get ESP for new stack */
	esp = (unsigned *)buf->esp;
/* push EFLAGS on the new stack */
	esp--;
	*esp = buf->eflags;
/* push current CS on the new stack */
	esp--;
	__asm__ __volatile__(
		"mov %%cs,%0\n"
		: "=m"(*esp));
/* push EIP on the new stack */
	esp--;
	*esp = buf->eip;
/* new ESP is 12 bytes lower; update jmp_buf.ESP */
	buf->esp = (unsigned)esp;
/* now, briefly, make the jmp_buf struct our stack */
	__asm__ __volatile__(
		"movl %0,%%esp\n"
/* ESP now points to 8 general-purpose registers stored in jmp_buf
Pop them */
		"popa\n"
/* load new stack pointer from jmp_buf */
		"movl -20(%%esp),%%esp\n"
/* ESP now points to new stack, with the IRET frame (EIP, CS, EFLAGS)
we created just above. Pop these registers: */
		"iret\n"
		:
		: "m"(buf));
}
