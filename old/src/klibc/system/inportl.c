/*****************************************************************************
**                                                                          **
**	Arquivo: outportl.c                                                     **
**	Descrição: Rotinas de IO do Console                                     **
**	Autor: Leonardo Monteiro                                                **
**	Data: 17-07-2008                                                        **
**                                                                          **
*****************************************************************************/
#include <system.h>

// Lê um dword de uma determina porta.
inline uint32_t inportl(uint32_t port){
	uint32_t ret;

	asm volatile ("inl %%dx,%%eax":"=a" (ret):"d"(port));

	return ret;
}
