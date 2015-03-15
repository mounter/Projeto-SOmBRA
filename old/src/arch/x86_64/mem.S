#include <x86-common/memory.h>

global gdt_ptr

[SECTION .data]

gdt:
    GDT_NULL
    GDT_DESCRIPTOR gdt_kcode, 0xFFFF, 0, 0, 0x9A, 0xAF, 0
    GDT_DESCRIPTOR gdt_kdata, 0xFFFF, 0, 0, 0x92, 0xAF, 0
    GDT_DESCRIPTOR gdt_ucode, 0xFFFF, 0, 0, 0xFA, 0xAF, 0
    GDT_DESCRIPTOR gdt_udata, 0xFFFF, 0, 0, 0xF2, 0xAF, 0
    GDT64_DESCRIPTOR gdt_tss, 103, 0, 0, 0x89, 0x40, 0, 0
gdt_end:

gdt_ptr:
    dw gdt_end - gdt - 1
    dq gdt
