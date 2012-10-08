/*****************************************************************************
**                                                                          **
**	Arquivo: conio.c                                                        **
**	Descrição: Rotinas de IO do Console                                     **
**	Autor: Mauro Joel Schütz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#include <system.h>

// Escreve um word em uma determinada porta.
inline void outportw(uint32_t port, uint32_t value){
	asm volatile ("outw %%ax,%%dx"::"d" (port), "a" (value));
}
