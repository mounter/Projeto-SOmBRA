global setjmp

// int setjmp(jmp_buf buf);
setjmp:
	push ebx
		mov ebx, [esp + 8]

		mov [ebx], edi		// buf->edi == 0(%ebx) == EDI
		mov [ebx+4], esi    // buf->esi == 4(%ebx) == ESI
		mov [ebx+8], ebp    // buf->ebp == 8(%ebx) == EBP

		mov [ebx + 20], edx // buf->edx == 20(%ebx) == EDX
		mov [ebx + 24], ecx // buf->ecx == 24(%ebx) == ECX
		mov [ebx + 28], eax // buf->eax == 28(%ebx) == EAX
// use EBX value saved on stack; not the current value
		mov eax, [esp]
		mov [ebx + 16], eax // buf->ebx == 16(%ebx) == EBX
// use ESP value after RET; not the current value
		lea eax, [esp + 8]
		mov [ebx + 12], eax // buf->esp == 32(%ebx) == ESP
/* use return address of this routine (EIP value saved on stack);
not the current value */
		mov eax, [esp + 4]
		mov [ebx+32], eax   // buf->eip == 36(%ebx) == EIP
// none of the PUSH or MOV instructions changed EFLAGS!
		pushf
		pop dword [ebx + 36]    // buf->eflags == 40(%ebx) == EFLAGS
	pop ebx
	xor eax,eax
	ret
