/*****************************************************************************
**                                                                          **
** Arquivo: start.S                                                         **
** Data: 12/05/2011                                                         **
** Autor: Mauro Joel Schutz                                                 **
** Descrição: Arquivo de entrada do núcleo.                                 **
**                                                                          **
*****************************************************************************/
#include <arch.h>
#include <kernel/multiboot.h>
#include _MEMORY_H_

[SECTION .dtext]

// Referencias globais
global SOmBRA
global __exit
global multiboot_info

// Referencias externas
extern main
extern gdt_ptr
extern isr_load_and_setup
extern g_d_bss
extern g_bss_end

[BITS 32]

SOmBRA:

// Ativar a GDT64
   lgdt [gdt_ptr]         // Load the 64-bit global descriptor table.
   jmp GDT64_KCODE:start64       // Set the code segment and enter 64-bit long mode.
    
[BITS 64]

// Entra em 64-bit
start64:

    cli                           // Clear the interrupt flag.
    mov cx, GDT64_KDATA           // Set the A-register to the data descriptor.
    mov ds, cx                    // Set the data segment to the A-register.
    mov es, cx                    // Set the extra segment to the A-register.
    mov ss, cx
    mov fs, cx                    // Set the F-segment to the A-register.
    mov gs, cx                    // Set the G-segment to the A-register.

// checando se foi carregado de um multiboot compatível
	cmp eax, MULTIBOOT_BOOTLOADER_MAGIC
	je mb_ok

// Escrevendo um B
	mov word [0xB8000], 0x9F42
	jmp __exit

mb_ok:
	xor rax, rax
	mov eax, ebx
	mov [multiboot_info], rax
    
// Zera BSS
    mov rcx, g_bss_end
    sub rcx, g_d_bss
    mov rax, 0
zero_bss:
    mov [g_d_bss+rax], byte 0
    inc rax
    loop zero_bss

// Atribuindo nossa pilha
    mov rsp, stack

// Carregando a IDT e o TSS
    call isr_load_and_setup

// Atribuindo as flags do processador
    push qword 0
    popf

// Chamando o código C
    call main
	
// Conferindo se no main ocorreu tudo bem
    cmp rax, 0
    jne __exit
    sti // Ativando as interrupções e iniciando o scheduler

__exit:
    hlt
    jmp __exit

[SECTION .data]

multiboot_info:
	dq 0

[SECTION .bss]

stack_top:
    resb STACK_SIZE
stack:

