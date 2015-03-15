/*****************************************************************************
**                                                                          **
** Arquivo: start.S                                                         **
** Data: 30/12/2008                                                         **
** Autor: Mauro Joel Schutz                                                 **
** Descrição: Arquivo de entrada do núcleo.                                 **
**                                                                          **
*****************************************************************************/
#include <arch.h>
#include <kernel/multiboot.h>
#include _MEMORY_H_

#define DS_MAGIC    0x3544DA2A

[SECTION .dtext]
[BITS 32]

// Referencias globais
global SOmBRA
global __exit
global multiboot_info
global get_eip

// Referencias externas
extern main
extern gdt_ptr
extern isr_load_and_setup
extern g_d_code
extern g_d_bss
extern g_bss_end
extern g_end

SOmBRA:

// checando se o segmento de data está correto
	cmp dword [ds_magic], DS_MAGIC
	je ds_ok

// Escrevendo um D
	mov word [0xB8000], 0x9F44
	jmp __exit

ds_ok:

// checando se foi carregado de um multiboot compatível
	cmp eax, MULTIBOOT_BOOTLOADER_MAGIC
	je mb_ok

// Escrevendo um B
	mov word [0xB8000], 0x9F42
	jmp __exit

mb_ok:

// Zera BSS
    mov ecx, g_bss_end
    sub ecx, g_d_bss
    mov eax, 0
zero_bss:
    mov [g_d_bss+eax], byte 0
    inc eax
    loop zero_bss

    mov dword [multiboot_info], ebx

// Carregando a GDT e arrumando os segmentos
    lgdt [gdt_ptr]
    jmp GDT_KCODE: new_code
new_code:
    mov ax, GDT_KDATA
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

// Atribuindo nossa pilha
    mov esp, stack

// Carregando a IDT e o TSS
    call isr_load_and_setup
	
// Atribuindo as flags do processador
    push dword 0
    popf

// Chamando o código C
    call main
// Conferindo se no main ocorreu tudo bem
    cmp eax, 0
    jne __exit
    sti // Ativando as interrupções e iniciando o scheduler

// Teste de interrupção!
//    mov eax, 0
//    idiv eax

__exit:
    hlt
    jmp __exit


/*****************************************************************************
** Cabeçalho Multiboot, com "aout kludge"
** GRUB não irá dar boot a menos que
** - está estrutura esteja presente nos primeiros 8192 byte do kernel
** - está estrutura esteja alinhada em 4-byte
** - o valor mágico Multiboot esteja presente na estrutura
** - o checksum na estrutura esteja correto
*****************************************************************************/

MBOOT_PAGE_ALIGN EQU 1<<0
MBOOT_MEMORY_INFO EQU 1<<1
MBOOT_AOUT_KLUDGE EQU 1<<16
MBOOT_HEADER_MAGIC EQU 0x1BADB002
#ifdef __ELF__
MBOOT_HEADER_FLAGS EQU MBOOT_PAGE_ALIGN | MBOOT_MEMORY_INFO
#else
MBOOT_HEADER_FLAGS EQU MBOOT_PAGE_ALIGN | MBOOT_MEMORY_INFO | MBOOT_AOUT_KLUDGE
#endif
MBOOT_CHECKSUM EQU -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)
#if 0
// paging or SBAT (ld -Ttext=0xC0000000 ...)
VIRT_TO_PHYS EQU (0x100000 - 0xC0000000)
#else
// no address translation (ld -Ttext=0x100000 ...)
VIRT_TO_PHYS EQU 0
#endif

ALIGN 4
mboot:
	dd MBOOT_HEADER_MAGIC
	dd MBOOT_HEADER_FLAGS
	dd MBOOT_CHECKSUM

// Se for um executavel ELF não precisamos desta parte
#ifndef __ELF__
// aout kludge
	dd mboot + VIRT_TO_PHYS
	dd g_d_code + VIRT_TO_PHYS
	dd g_d_bss + VIRT_TO_PHYS
	dd g_end + VIRT_TO_PHYS
	dd SOmBRA + VIRT_TO_PHYS
#endif

[SECTION .text]

get_eip:
	pop dword eax
	jmp eax

[SECTION .data]

ds_magic:
	dd DS_MAGIC

[SECTION .bss]

multiboot_info:
    resd    1

stack_top:
    resb STACK_SIZE
stack:
