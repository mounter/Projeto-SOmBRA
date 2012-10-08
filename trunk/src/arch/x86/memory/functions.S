[SECTION .text]

global cr0_read
global cr0_write
global cr2_read
global cr3_read
global cr3_write

cr0_read:
    mov eax, cr0
    ret

cr0_write:
    push ebp
    mov ebp, esp
    mov eax, dword [ebp + 8]
    mov cr0, eax
    pop ebp
    ret

cr2_read:
    mov eax, cr2
    ret

cr3_read:
    mov eax, cr3
    ret

cr3_write:
    push ebp
    mov ebp, esp
    mov eax, dword [ebp + 8]
    mov cr3, eax
    pop ebp
    ret

[SECTION .bss]

global heap

heap:
    resd    1
