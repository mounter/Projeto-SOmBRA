global switch_to

extern task_curr

[BITS 64]
[SECTION .text]

switch_to:
	pop rax		// convert CALL stack frame (EIP only)...
	pushf		// ...to partial IRET (EIP, EFLAGS)
	push rax

	push r15	// callee-save registers used by C
	push r14
	push r13
	push r12
	push rbp
	push rbx

		cli	// interrupts OFF while switching stacks
		mov rax, [task_curr]

// store current kernel ESP in thread_t struct of current thread/task
		mov [rax], rsp

// get pointer (thread_t t) to new task/thread to run
		//mov rax, rdi

		mov rbx, [rdi + 16]
		cmp rbx, qword 0
		je no_cr3

// Carregando o novo pagedir
		mov cr3, rbx

no_cr3:

// make this the current thread
		mov [task_curr], rdi
// load new ESP
		mov rsp,[rdi]
	pop rbx	// pop C registers
	pop rbp
	pop r12
	pop r13
	pop r14
	pop r15

	pop rax	// convert partial IRET stack frame (EIP, EFLAGS)...
	pop rdi	// ...to full IRET stack frame (EIP, CS, EFLAGS)
	mov rsi, rsp
	xor rcx, rcx
	mov cx, ss
	push rcx
	push rsp
	push rdi
	xor rcx, rcx
	mov cx, cs
	push rcx
	push rax
	
	iretq		// IRET to new kernel thread
