/*****************************************************************************
**                                                                          **
**	Arquivo: conio.c                                                        **
**	Descri��o: Rotinas de IO do Console                                     **
**	Autor: Mauro Joel Sch�tz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#include <system.h>

// Habilita as interrup��es.
inline void enable(){
	asm volatile("sti");
}
