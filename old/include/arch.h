/************************************************************
** Arquivo: arch.h
** Data: 22/05/2011
** Autor: Mauro Joel Schütz
** Descrição: Arquivo de compatibilidade entre plataformas.
************************************************************/
#ifndef _ARCH_H
#define _ARCH_H

#if defined(__amd64__) || defined(__x86_64__)
#	define _REGS_H_	<x86_64/regs.h>
#	define _PAGE_H_	<x86_64/page.h>
#else
#	define _REGS_H_	<x86/regs.h>
#	define _PAGE_H_	<x86/page.h>
#endif

#define _ISR_H_		<x86-common/isr.h>
#define _MEMORY_H_	<x86-common/memory.h>
#define _INTERRUPTS_H_	<x86-common/interrupts.h>

#endif

