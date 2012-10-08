/*****************************************************************************
**                                                                          **
**	Arquivo: conio.c                                                        **
**	Descrição: Rotinas de IO do Console                                     **
**	Autor: Mauro Joel Schütz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#include <system.h>

// Lê um byte de uma determina porta.
inline unsigned char inportb(uint32_t port){
	uint8_t ret;

	asm volatile ("inb %%dx,%%al":"=a" (ret):"d"(port));

	return ret;
}
