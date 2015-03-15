/*****************************************************************************
**                                                                                                               **
** Arquivo: start.S                                                                                       **
** Data: 30/12/2008                                                                                   **
** Autor: Mauro Joel Schutz                                                                         **
** Descrição: Arquivo de entrada do núcleo.                                                **
**                                                                                                               **
*****************************************************************************/

#include <kernel/multiboot.h>
#include <x86-common/memory.h>

#define DS_MAGIC    0x3544DA2A

[SECTION .dtext]
[BITS 32]

// Referencias globais
global multiboot_info
global heap
global SOmBRA
global goto_addr
global have_longmode
global enter_compatibility_mode

// Referencias externas
extern loader_main
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

// Atribuindo nossa pilha
    mov esp, stack

// Atribuindo as flags do processador
    push dword 0
    popf

// Chamando o código C
    call loader_main

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

have_longmode:
	push ecx
	push edx
	pushfd               // Store the FLAGS-register.
	pop eax              // Restore the A-register.
	mov ecx, eax         // Set the C-register to the A-register.
	xor eax, 1 << 21     // Flip the ID-bit, which is bit 21.
	push eax             // Store the A-register.
	popfd                // Restore the FLAGS-register.
	pushfd               // Store the FLAGS-register.
	pop eax              // Restore the A-register.
	push ecx             // Store the C-register.
	popfd                // Restore the FLAGS-register.
	xor eax, ecx         // Do a XOR-operation on the A-register and the C-register.
	jz .NoOk          // The zero flag is set, no CPUID.
	// CPUID is available for use.

	mov eax, 0x80000000    // Set the A-register to 0x80000000.
	cpuid                  // CPU identification.
	cmp eax, 0x80000001    // Compare the A-register with 0x80000001.
	jb .NoOk         // It is less, there is no long mode.

	mov eax, 0x80000001    // Set the A-register to 0x80000001.
	cpuid                  // CPU identification.
	test edx, 1 << 29      // Test if the LM-bit, which is bit 29, is set in the D-register.
	jz .NoOk         // They aren't, there is no long mode.

	xor eax, eax
	or eax, 1
	jmp .Ok

.NoOk:
	xor eax, eax
.Ok:
	pop edx
	pop ecx
	ret

enter_compatibility_mode:
	push ecx
	mov ecx, 0xC0000080          // Set the C-register to 0xC0000080, which is the EFER MSR.
	rdmsr                        // Read from the model-specific register.
	or eax, 1 << 8               // Set the LM-bit which is the 9th bit (bit 8).
	wrmsr                        // Write to the model-specific register
	mov eax, cr0                 // Set the A-register to control register 0.
	or eax, 1 << 31              // Set the PG-bit, which is the 32nd bit (bit 31).
	mov cr0, eax                 // Set control register 0 to the A-register.
	pop ecx
	ret

// Vai a um endereço da pilha
goto_addr:
	add esp, 4
	mov eax, MULTIBOOT_BOOTLOADER_MAGIC
	mov ebx, [multiboot_info]
	ret

[SECTION .data]

ds_magic:
	dd DS_MAGIC

[SECTION .bss]

multiboot_info:
    resd    1

stack_top:
    resb STACK_SIZE
stack:
