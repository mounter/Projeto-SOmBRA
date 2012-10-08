/*****************************************************************************
**                                                                          **
**	Arquivo: conio.c                                                        **
**	Descrição: Rotinas de IO do Console                                     **
**	Autor: Mauro Joel Schütz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#include <system.h>

// Habilita as interrupções.
inline void enable(){
	asm volatile("sti");
}
