/*****************************************************************************
**                                                                          **
**	Arquivo: conio.c                                                        **
**	Descri��o: Rotinas de IO do Console                                     **
**	Autor: Mauro Joel Sch�tz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#include <system.h>

// Escreve um byte em uma determina porta.
inline void outportb(uint32_t port, uint8_t value){
	asm volatile ("outb %%al,%%dx"::"d" (port), "a" (value));
}
