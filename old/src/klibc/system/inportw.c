/*****************************************************************************
**                                                                          **
**	Arquivo: inportw.c                                                      **
**	Descrição: Rotinas de IO do Console                                     **
**	Autor: Leonardo Monteiro                                                **
**	Data: 17-07-2008                                                        **
**                                                                          **
*****************************************************************************/
#include <system.h>

// Lê um word de uma determina porta.
inline unsigned short inportw(uint32_t port){
	uint16_t ret;

	asm volatile ("inw %%dx,%%ax":"=a" (ret):"d"(port));

	return ret;
}
