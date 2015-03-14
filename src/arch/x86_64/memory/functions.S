/******************************************************
** Arquivo: functions.S
** Data: 22/05/2011
** Autor: Mauro Joel Schutz
** Descrição: Funções de controle do registro do processador.
******************************************************/
[SECTION .text]

global cr0_read
global cr0_write
global cr2_read
global cr3_read
global cr3_write

cr0_read:
    mov rax, cr0
    ret

cr0_write:
    mov rax, rdi
    mov cr0, rax
    ret

cr2_read:
    mov rax, cr2
    ret

cr3_read:
    mov rax, cr3
    ret

cr3_write:
    mov rax, rdi
    mov cr3, rax
    ret

[SECTION .bss]

global heap
heap:
    resq    1

