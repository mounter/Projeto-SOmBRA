#include <arch.h>
#include _MEMORY_H_
#include _ISR_H_
#include "../x86-common/macros.h"

global isr_load_and_setup
global tss_iopb
global isr_get_handler
global isr_set_handler

extern isr_fault
extern gdt_tss
extern isr_call_ptr_vector
extern task_curr

[BITS 64]
[SECTION .dtext]

isr_load_and_setup:
    // Atribuindo nosso descritor de TSS, e então carregamos o registrador de tarefas
	lea rax, [tss]
	mov word [gdt_tss], tss_end - tss - 1
	mov [gdt_tss + 2],ax
	shr rax, 16
	mov [gdt_tss + 4],al
	mov [gdt_tss + 7],ah
	shr rax, 16	// A entrada da TSS deve possuir 16 bytes
	mov [gdt_tss + 8], eax
	mov ax, GDT64_TSS
	ltr ax

    // Atribuindo os tratadores de interrupções, então carregamos nosso registrador de IDT
	mov rcx, ((idt_end - idt) / 16) // number of exception handlers
	mov rbx, idt
	mov rdx, isr00h
build_idt:
	mov rax, rdx		// EAX=offset of entry point
	mov [rbx], ax		// set low 16 bits of gate offset
	shr rax, 16
	mov [rbx + 6], ax	// set high 16 bits of gate offset
	shr rax, 16
	mov [rbx + 8], eax	// set high 32 bits of gate offset
	add ebx, byte 16	// 16 bytes per interrupt gate
	add edx, byte (isr01h - isr00h) // N bytes per stub
	loop build_idt
	lidt [idt_ptr]
	ret

[SECTION .text]

isr_get_handler:
	// edi - num
	// rsi - vector

// get access byte from IDT[i]
    xor rcx, rcx
    xor rax, rax
    mov ecx, edi
	and ecx, 0xFF
    shl rcx, 4
    mov al, [idt + rcx + 5]
    mov [rsi], eax

// Salvando o ip na estrutura
	xor rcx, rcx
    mov rax, [isr_call_ptr_vector]
    mov ecx, edi
	and ecx, 0xFF
    mov rdx, [rax + rcx * 8]
    mov [rsi + 4], rdx
    ret

isr_set_handler:

	// edi - num
	// rsi - vector

// store access byte in IDT[i]
	xor rax, rax
	xor rcx, rcx
    mov eax, [rsi]
    mov ecx, edi
    and rcx, 0xFF
    shl rcx, 4
    mov [idt + rcx + 5], al

// Movendo para o vetor de interrupções
	xor rcx, rcx
    mov rax, [isr_call_ptr_vector]
    mov ecx, edi
	and rcx, 0xFF
    mov rdx, [rsi + 4]
    mov [rax + rcx * 8], rdx
    ret

/*****************************************************************************
** IRQ/interrupt/exception handler "stubs"
** The interrupt/trap gates in the IDT must point to these
**
**     *** WARNING: The stubs must be consecutive, and each must be the same size
*****************************************************************************/
	align 16,db 0x90	// Alinhar os stubs em 16-byte devido ao cache

	INTR 00h		// zero divide (fault)
	INTR 01h		// debug/single step
	INTR 02h		// non-maskable interrupt (trap)
	INTR 03h		// INT3 (trap)
	INTR 04h		// INTO (trap)
	INTR 05h		// BOUND (fault)
	INTR 06h		// invalid opcode (fault)
	INTR 07h		// coprocessor not available (fault)
	INTR_EC 08h	// double fault (abort w/ error code)
	INTR 09h		// coproc segment overrun (abort// 386/486SX only)
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

// Aqui as interrupções serão tratadas!
isr_handler:
	push rax
	push rbx
	push rcx
	push rdx
	push rdi
	push rsi
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	push fs
	push gs
	
	mov ax, GDT64_KDATA
	mov ds, ax
	mov fs, ax
	mov gs, ax
	
	mov qword rdi, rsp

	mov rbx, [rdi + 0x88]             // Qual interrupção foi feita?
	mov rax, [isr_call_ptr_vector]	// Pega o ponteiro para o array de funções

// Checando se o ponteiro é nulo!
	cmp rax, qword 0
	je __isr_ptr_null               // Aviso de erro padrão

	mov rax, [rax + rbx * 8]	        // Função que deve ser chamada

// Checando se o ponteiro é nulo!
	cmp rax, qword 0
	je __isr_ptr_null               // Aviso de erro padrão

	call rax
    jmp isr_common                  // Voltando de onde foi chamado

__isr_ptr_null:
    call isr_fault
	
isr_common:

	pop	gs
	pop	fs
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	rbp
	pop	rsi
	pop	rdi
	pop	rdx
	pop	rcx
	pop	rbx
	pop	rax
	
	add qword rsp, 0x10	// remove which_int e err_code da pilha
	iretq


[SECTION .data]

tss:
	dd 0	// Reservado
	dq intr_stack//rsp0 - Pilha do RING0
	dq 0	//rsp1 - Pilha do RING1
	dq 0	//rsp2 - Pilha do RING2
	dq 0	// Reservado
	dq 0	// IST1
	dq 0	// IST2
	dq 0	// IST3
	dq 0	// IST4
	dq 0	// IST5
	dq 0	// IST6
	dq 0	// IST7
	dq 0	// Reservado
	dw 0, tss_iopb - tss    // Reservado, IO permission bitmap (none)
tss_iopb:
// no I/O permitted
	times 8192 db 0FFh
tss_end:

idt:
%rep ISR_COUNT
	dw 0				// offset 15:0
	dw GDT64_KCODE		// seletor
	db 0				// ist and (always 0 for interrupt gates)
	db 0x8E				// present,ring 0,'386 interrupt gate
	dw 0				// offset 31:16
	dd 0				// offset 63:32
	dd 0				// reserved
%endrep
idt_end:

idt_ptr:
	dw idt_end - idt - 1		// IDT limit
	dq idt

[SECTION .bss]

intr_stack_top:
    resb STACK_SIZE
intr_stack:

