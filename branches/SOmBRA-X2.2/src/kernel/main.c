#include <arch.h>
#include <x86-common/drivers/video.h>
#include <x86-common/drivers/timer.h>
#include _INTERRUPTS_H_
#include _ISR_H_
#include _PAGE_H_
#include <stdio.h>
#include <string.h>
#include <kernel/mm.h>
#include <kernel/thread.h>

int main()
{
	init_video();
	isr_init();
	c8259s_init();
	interrupts_init();
	mm_init();
	init_8253();
	thread_init();
	
	#ifdef __x86_64__
	char *arch = "X86_64";
	#else
	char *arch = "X86";
	#endif

	printf("Projeto SOmBRA\n\tPrototipo para arquitetura %s\n", arch);
	
	return 0;
}
