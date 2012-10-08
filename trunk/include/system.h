/*****************************************************************************
**                                                                          **
**	Arquivo: conio.h                                                        **
**	Descrição: Rotinas de IO do Console                                     **
**	Autor: Mauro Joel Schütz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/

#ifndef CONIO_H
#define CONIO_H

#include <kernel/types.h>

inline void outportb(uint32_t port, uint8_t value);
inline void outportw(uint32_t port, uint32_t value);
inline void outportl(uint32_t port, uint32_t value);

inline uint8_t inportb(uint32_t port);
inline uint16_t inportw(uint32_t port);
inline uint32_t inportl(uint32_t port);

inline void enable();
inline void disable();

#endif
