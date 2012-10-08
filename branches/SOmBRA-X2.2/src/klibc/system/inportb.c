/*****************************************************************************
**                                                                          **
**	Arquivo: conio.c                                                        **
**	Descri��o: Rotinas de IO do Console                                     **
**	Autor: Mauro Joel Sch�tz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#include <system.h>

// L� um byte de uma determina porta.
inline unsigned char inportb(uint32_t port){
	uint8_t ret;

	asm volatile ("inb %%dx,%%al":"=a" (ret):"d"(port));

	return ret;
}
