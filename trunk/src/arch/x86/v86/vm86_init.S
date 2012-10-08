[BITS 32]

global v86_start
global v86_end

extern isr_to_user
extern v86_isr_call_vector
extern v86_isr_aux
extern isr_call_ptr_vector

[SECTION .text]

v86_start:

// save registers used by C: EBP, EBX, ESI, EDI
// Actually, just use PUSHA to save them all
	pusha

    cli
    mov eax, [isr_call_ptr_vector]
    mov [v86_isr_aux], eax
    mov dword [isr_call_ptr_vector], v86_isr_call_vector

// copy V86 mode registers to top of stack
	sub esp,92
		mov esi,[esp + 128] // (uregs_t *regs)
		mov edi,esp
		mov ecx,92 // sizeof(uregs_t)
		cld
		rep movsb

		jmp near isr_to_user

v86_end:

// get modified V86 mode registers
		mov esi,esp
		mov edi,[esp + 128]
		mov ecx,92
		cld
		rep movsb

// remove V86 registers from stack
		add esp,92

    mov eax, [v86_isr_aux]
    mov [isr_call_ptr_vector], eax

// restore C registers and return to pmode kernel
	popa
	ret
