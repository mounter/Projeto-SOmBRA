/*****************************************************************************
**                                                                          **
**	Arquivo: conio.c                                                        **
**	Descrição: Rotinas de IO do Console                                     **
**	Autor: Mauro Joel Schütz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#include <system.h>

// Desabilita as interrupções.
inline void disable(){
	asm volatile ("cli");
}
