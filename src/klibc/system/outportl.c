/*****************************************************************************
**                                                                          **
**	Arquivo: outportl.c                                                     **
**	Descrição: Rotinas de IO do Console                                     **
**	Autor: Leonardo Monteiro                                                **
**	Data: 17-07-2008                                                        **
**                                                                          **
*****************************************************************************/
#include <system.h>

// Escreve um dword em uma determinada porta.
inline void outportl(uint32_t port, uint32_t value)
{
	asm volatile ("outl %%eax,%%dx"::"d" (port), "a" (value));
}
