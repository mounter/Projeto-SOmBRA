/*****************************************************************************
**                                                                          **
**	Arquivo: conio.h                                                        **
**	Descrição: Rotinas de IO do Console                                     **
**	Autor: Mauro Joel Schütz                                                **
**	Data: 17/01/2006                                                        **
**  Modificação: 13/08/2012 - acrescimo de outb, inb, ...                   **
**                                                                          **
*****************************************************************************/

#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <kernel/types.h>

inline void outportb(uint32_t port, uint8_t value);
inline void outportw(uint32_t port, uint32_t value);
inline void outportl(uint32_t port, uint32_t value);

#define outb(v, p)	outportb(p, v)
#define outw(v, p)	outportw(p, v)
#define outl(v, p)	outportl(p, v)

inline uint8_t inportb(uint32_t port);
inline uint16_t inportw(uint32_t port);
inline uint32_t inportl(uint32_t port);

#define inb(p)	inportb(p)
#define inw(p)	inportw(p)
#define inl(p)	inportl(p)

inline void enable();
inline void disable();

#endif
