#include <x86-common/isr.h>
#include <x86-common/memory.h>
#include "../x86-common/macros.h"

global tss_esp0
global isr_load_and_setup
global isr_get_handler
global isr_set_handler
global tss_iopb
global isr_to_user
global isr_common

extern gdt_tss
extern gdt_ucode
extern gdt_udata
extern isr_fault
extern isr_call_ptr_vector
extern v86_end
extern task_curr

[SECTION .text]

isr_load_and_setup:
    // Atribuindo nosso descritor de TSS, e então carregamos o registrador de tarefas
	lea eax, [tss]
	mov word [gdt_tss], tss_end - tss - 1
	mov [gdt_tss + 2],ax
	shr eax, 16
	mov [gdt_tss + 4],al
	mov [gdt_tss + 7],ah
	mov ax, GDT_TSS
	ltr ax

// Atribuindo os tratadores de interrupções, então carregamos nosso registrador de IDT
	mov ecx, ((idt_end - idt) / 8) // number of exception handlers
	mov ebx, idt
	mov edx, isr0
build_idt:
	mov eax, edx		// EAX=offset of entry point
	mov [ebx], ax		// set low 16 bits of gate offset
	shr eax, 16
	mov [ebx + 6], ax	// set high 16 bits of gate offset
	add ebx, byte 8		// 8 bytes per interrupt gate
	add edx, byte (isr1 - isr0) // N bytes per stub
	loop build_idt
	lidt [idt_ptr]
	ret

isr_handler:
// Criando o resto da estrutura uregs_t
    push gs
    push fs
    push es
    push ds
    pusha

// Mudando o segmento de dados
    mov ax, GDT_KDATA
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
	mov ebp, esp
	mov ebx, [ebp + 52]             // Qual interrupção foi feita?
	mov eax, [isr_call_ptr_vector]	// Pega o ponteiro para o array de funções

// Checando se o ponteiro é nulo!
	cmp eax, dword 0
	je __isr_ptr_null               // Aviso de erro padrão

	mov eax, [eax + ebx * 4]	        // Função que deve ser chamada

// Checando se o ponteiro é nulo!
	cmp eax, dword 0
	je __isr_ptr_null               // Aviso de erro padrão

	call eax
    jmp isr_common                  // Voltando de onde foi chamado

__isr_ptr_null:
    call isr_fault

isr_common:
	add esp, byte 4                 // Removendo o ponteiro da pilha

// For IRET to Ring 0 pmode, _tss_esp0 is not used,
// so this value doesn't matter
set_tss_esp0:
		mov [tss_esp0],eax
	popa			// pop EAX...EDI (7 regs; 8 dwords)
	pop es			// pop segment registers
	pop ds
	pop fs
	pop gs
	add esp, byte 8 // drop int_num and err_code
	iret			// pop EIP...GS (9 regs)

isr_get_handler:
    push ebp
    mov ebp, esp
    push eax
    push esi

    mov esi, [ebp + 12]

// get access byte from IDT[i]
    xor ebx, ebx
    xor eax, eax
    mov bl, [ebp + 8]
    shl ebx, 3
    mov al, [idt + ebx + 5]
    mov [esi], eax

// Salvando o ip na estrutura
    mov eax, [isr_call_ptr_vector]
    mov ebx, [ebp + 8]
    mov esi, [eax + ebx * 4]
    mov [esi + 4], esi

    pop esi
    pop eax
    pop ebp
    ret

isr_set_handler:
    push ebp
    mov ebp, esp
    push eax
    push esi

    mov esi, [ebp + 12]

// store access byte in IDT[i]
    mov eax, [esi]
    xor ebx,ebx
    mov bl, [ebp + 8]
    shl ebx,3
    mov [idt + ebx + 5], al

// Movendo para o vetor de interrupções
    mov eax, [isr_call_ptr_vector]
    mov ebx, [ebp + 8]
    mov esi, [esi + 4]
    mov [eax + ebx * 4], esi

    pop esi
    pop eax
    pop ebp
    ret

/*****************************************************************************
** IRQ/interrupt/exception handler "stubs"
** The interrupt/trap gates in the IDT must point to these
**
**     *** WARNING: The stubs must be consecutive, and each must be the same size
*****************************************************************************/

	INTR 0		// zero divide (fault)
	INTR 1		// debug/single step
	INTR 2		// non-maskable interrupt (trap)
	INTR 3		// INT3 (trap)
	INTR 4		// INTO (trap)
	INTR 5		// BOUND (fault)
	INTR 6		// invalid opcode (fault)
	INTR 7		// coprocessor not available (fault)
	INTR_EC 8	// double fault (abort w/ error code)
	INTR 9		// coproc segment overrun (abort// 386/486SX only)
	INTR_EC 0Ah	// bad TSS (fault w/ error code)
	INTR_EC 0Bh	// segment not present (fault w/ error code)
	INTR_EC 0Ch	// stack fault (fault w/ error code)
	INTR_EC 0Dh	// GPF (fault w/ error code)
	INTR_EC 0Eh	// page fault
	INTR 0Fh	// reserved
	INTR 10h	// FP exception/coprocessor error (trap)
	INTR 11h	// alignment check (trap// 486+ only)
	INTR 12h	// machine check (Pentium+ only)
	INTR 13h
	INTR 14h
	INTR 15h
	INTR 16h
	INTR 17h
	INTR 18h
	INTR 19h
	INTR 1Ah
	INTR 1Bh
	INTR 1Ch
	INTR 1Dh
	INTR 1Eh
	INTR 1Fh

// isr20 through isr2F are hardware interrupts. The 8259 programmable
// interrupt controller (PIC) chips must be reprogrammed to make these work.
	INTR 20h	// IRQ 0/timer interrupt
	INTR 21h	// IRQ 1/keyboard interrupt
	INTR 22h
	INTR 23h
	INTR 24h
	INTR 25h
	INTR 26h	// IRQ 6/floppy interrupt
	INTR 27h
	INTR 28h	// IRQ 8/real-time clock interrupt
	INTR 29h
	INTR 2Ah
	INTR 2Bh
	INTR 2Ch
	INTR 2Dh	// IRQ 13/math coprocessor interrupt
	INTR 2Eh	// IRQ 14/primary ATA ("IDE") drive interrupt
	INTR 2Fh	// IRQ 15/secondary ATA drive interrupt

// syscall software interrupt
	INTR 30h

// the other 207 vectors are undefined

%assign i 31h
%rep (0FFh - 30h)

	INTR i

%assign i (i + 1)
%endrep

[SECTION .data]

// we don't use the TSS for task-switching, but we still need it to store
// the kernel (ring 0) stack pointer while user (ring 3) code is running,
// and we also need it for the I/O permission bitmap
tss:
	dw 0, 0	                // back link
tss_esp0:
	dd 0		            // ESP0
	dw GDT_KDATA, 0	        // SS0, reserved

	dd 0		            // ESP1
	dw 0, 0		            // SS1, reserved

	dd 0		            // ESP2
	dw 0, 0		            // SS2, reserved

	dd 0		            // CR3
	dd 0, 0		            // EIP, EFLAGS
	dd 0, 0, 0, 0	        // EAX, ECX, EDX, EBX
	dd 0, 0, 0, 0	        // ESP, EBP, ESI, EDI
	dw 0, 0		            // ES, reserved
	dw 0, 0		            // CS, reserved
	dw 0, 0		            // SS, reserved
	dw 0, 0		            // DS, reserved
	dw 0, 0		            // FS, reserved
	dw 0, 0		            // GS, reserved
	dw 0, 0		            // LDT, reserved
	dw 0, tss_iopb - tss    // debug, IO permission bitmap (none)

tss_iopb:

// no I/O permitted
	times 8192 db 0FFh

// The TSS notes in section 12.5.2 of volume 1 of
// "IA-32 Intel Architecture Software Developer's Manual"
// are confusing as hell. I think they mean simply that the IOPB
// must contain an even number of bytes; so pad here if necessary.
	//dw 0FFFFh
tss_end:

idt:
%rep ISR_COUNT
	dw 0				// offset 15:0
	dw GDT_KCODE		// seletor
	db 0				// (always 0 for interrupt gates)
	db 0x8E				// present,ring 0,'386 interrupt gate
	dw 0				// offset 31:16
%endrep
idt_end:

idt_ptr:
	dw idt_end - idt - 1		// IDT limit
	dd idt
