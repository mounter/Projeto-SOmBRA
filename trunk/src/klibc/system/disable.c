/*****************************************************************************
**                                                                          **
**	Arquivo: conio.c                                                        **
**	Descri��o: Rotinas de IO do Console                                     **
**	Autor: Mauro Joel Sch�tz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#include <system.h>

// Desabilita as interrup��es.
inline void disable(){
	asm volatile ("cli");
}
