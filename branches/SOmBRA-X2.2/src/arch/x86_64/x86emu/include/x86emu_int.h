/****************************************************************************
*
*						Realmode X86 Emulator Library
*
*            	Copyright (C) 1996-1999 SciTech Software, Inc.
* 				     Copyright (C) David Mosberger-Tang
* 					   Copyright (C) 1999 Egbert Eich
*
*  ========================================================================
*
*  Permission to use, copy, modify, distribute, and sell this software and
*  its documentation for any purpose is hereby granted without fee,
*  provided that the above copyright notice appear in all copies and that
*  both that copyright notice and this permission notice appear in
*  supporting documentation, and that the name of the authors not be used
*  in advertising or publicity pertaining to distribution of the software
*  without specific, written prior permission.  The authors makes no
*  representations about the suitability of this software for any purpose.
*  It is provided "as is" without express or implied warranty.
*
*  THE AUTHORS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
*  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
*  EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
*  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
*  USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
*  OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
*  PERFORMANCE OF THIS SOFTWARE.
*
*  ========================================================================
*
* Language:		ANSI C
* Environment:	Any
* Developer:    Kendall Bennett
*
* Description:  Header file for system specific functions. These functions
*				are always compiled and linked in the OS depedent libraries,
*				and never in a binary portable driver.
*
****************************************************************************/


#ifndef __X86EMU_X86EMU_INT_H
#define __X86EMU_X86EMU_INT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define L_SYM			__attribute__((visibility("internal")))

#define M x86emu

#include "x86emu.h"
#include "decode.h"
#include "ops.h"
#include "prim_ops.h"
#include "mem.h"

// global emulator state
extern x86emu_t M;

#define INTR_RAISE_DIV0(a)	x86emu_intr_raise(a, 0, INTR_TYPE_SOFT | INTR_MODE_RESTART, 0)
#define INTR_RAISE_SOFT(a, n)	x86emu_intr_raise(a, n, INTR_TYPE_SOFT, 0)
#define INTR_RAISE_GP(a, err)	x86emu_intr_raise(a, 0x0d, INTR_TYPE_FAULT | INTR_MODE_RESTART | INTR_MODE_ERRCODE, err)
#define INTR_RAISE_UD(a)	x86emu_intr_raise(a, 0x06, INTR_TYPE_FAULT | INTR_MODE_RESTART, 0)

#define MODE_REPE		(M.x86.mode & _MODE_REPE)
#define MODE_REPNE		(M.x86.mode & _MODE_REPNE)
#define MODE_REP		(M.x86.mode & (_MODE_REPE | _MODE_REPNE))
#define MODE_DATA32		(M.x86.mode & _MODE_DATA32)
#define MODE_ADDR32		(M.x86.mode & _MODE_ADDR32)
#define MODE_STACK32		(M.x86.mode & _MODE_STACK32)
#define MODE_CODE32		(M.x86.mode & _MODE_CODE32)
#define MODE_HALTED		(M.x86.mode & _MODE_HALTED)

#define MODE_PROTECTED(a)	((a)->x86.R_CR0 & 1)
#define MODE_REAL(a)		(!MODE_PROTECTED(a))

#define TOGGLE_FLAG(flag)     	(M.x86.R_FLG ^= (flag))
#define SET_FLAG(flag)        	(M.x86.R_FLG |= (flag))
#define CLEAR_FLAG(flag)      	(M.x86.R_FLG &= ~(flag))
#define ACCESS_FLAG(flag)     	(M.x86.R_FLG & (flag))
#define CLEARALL_FLAG(m)    	(M.x86.R_FLG = 0)

#define CONDITIONAL_SET_FLAG(COND,FLAG) \
  if(COND) SET_FLAG(FLAG); else CLEAR_FLAG(FLAG)

#define LOG_STR(a) memcpy(*p, a, sizeof (a) - 1), *p += sizeof (a) - 1
#define LOG_FREE(emu) ((emu)->log.size + (emu)->log.buf - (emu)->log.ptr)

#if defined(__i386__) || defined (__x86_64__)
#define WITH_TSC	1
#define WITH_IOPL	1
#else
#define WITH_TSC	0
#define WITH_IOPL	0
#endif


#if WITH_TSC
#if defined(__i386__)
static inline u64 tsc()
{
  register u64 tsc asm ("%eax");

  asm (
    "rdtsc"
    : "=r" (tsc)
  );

  return tsc;
}
#endif

#if defined (__x86_64__)
static inline u64 tsc()
{
  register u64 tsc asm ("%rax");

  asm (
    "push %%rdx\n"
    "rdtsc\n"
    "xchg %%edx,%%eax\n"
    "shl $32,%%rax\n"
    "add %%rdx,%%rax\n"
    "pop %%rdx"
    : "=r" (tsc)
  );

  return tsc;
}
#endif

#endif


#if WITH_IOPL
#if defined(__i386__)
static inline unsigned getiopl() 
{
  register u32 i asm ("%eax");
  
  asm(
    "pushf\n"
    "pop %%eax"
    : "=r" (i)
  );

  i = (i >> 12) & 3;

  return i;
}
#endif

#if defined (__x86_64__)
static inline unsigned getiopl()
{
  register unsigned i asm ("%rax");

  asm(
    "pushf\n"
    "pop %%rax"
    : "=r" (i)
  );

  i = (i >> 12) & 3;

  return i;
}
#endif

#endif


#endif /* __X86EMU_X86EMU_INT_H */

