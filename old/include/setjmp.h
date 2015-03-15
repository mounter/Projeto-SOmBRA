#ifndef _SETJMP_H
#define	_SETJMP_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
/* setjmp() and longjmp() rely on the order of these registers,
so do not re-arrange them */
	unsigned edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned eip, eflags;
} jmp_buf[1];

int setjmp(jmp_buf b);
void longjmp(jmp_buf b, int ret_val);

#ifdef __cplusplus
}
#endif

#endif
