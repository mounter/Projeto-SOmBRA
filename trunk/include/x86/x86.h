#ifndef _X86_COMMONS_H
#define _X86_COMMONS_H

#include "../kernel/types.h"
#include "../kernel/multiboot.h"

#define ISR_H           <x86/isr.h>
#define REGS_H          <x86/regs.h>
#define INTERRUPTS_H    <x86/interrupts.h>
#define MEMORY_H        <x86/memory.h>
#define PAGE_H		<x86/page.h>
#define ELF_H           <x86/elf.h>
#define COFF_H		<x86/coff.h>
#define DEBUG_H		<x86/debug.h>
#define V86_H		<x86/v86.h>
#define DOS_H		<x86/dos.h>
#define SYSCALL_H	<x86/syscall.h>
#define PCI_H		<x86/pci.h>

extern multiboot_info_t *multiboot_info;
//void __exit(void);

#endif
