#include <x86-common/memory.h>

global gdt_ptr
extern tss
extern tss_end

[SECTION .data]

gdt:
    GDT_NULL
    GDT_DESCRIPTOR gdt_tss, 103, 0, 0, 0x89, 0, 0
    GDT_DESCRIPTOR gdt_kcode, 0xFFFF, 0, 0, 0x9A, 0xCF, 0
    GDT_DESCRIPTOR gdt_kdata, 0xFFFF, 0, 0, 0x92, 0xCF, 0
    GDT_DESCRIPTOR gdt_ucode, 0xFFFF, 0, 0, 0xFA, 0xCF, 0
    GDT_DESCRIPTOR gdt_udata, 0xFFFF, 0, 0, 0xF2, 0xCF, 0
gdt_end:

gdt_ptr:
    dw gdt_end - gdt - 1
    dd gdt
