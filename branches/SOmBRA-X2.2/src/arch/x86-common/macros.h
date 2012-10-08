#ifndef _ARCH_X86_MACROS_H_
#define _ARCH_X86_MACROS_H_

// I shouldn't have to do this!
%macro PUSHB 1
	db 6Ah
	db %1
%endmacro

%macro INTR 1				// (byte offset from start of stub)
global isr%1
isr%1:
    cli
	push byte 0			// ( 0) fake error code
	PUSHB %1			// ( 2) exception number
	jmp near isr_handler
%endmacro

%macro INTR_EC 1
global isr%1
isr%1:
    cli
	nop				// error code already pushed
	nop				// nop+nop=same length as push byte
	PUSHB %1			// ( 2) exception number
	jmp near isr_handler
%endmacro

#endif
