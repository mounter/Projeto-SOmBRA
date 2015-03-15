#include <x86/asm.h>

/* Assegura que este codigo é linkado em
 * primeiro lugar */
.section .init.text, "ax", @progbits

/* Ponto de entrada */
FUNCTION_START(SOmBRA)
    cli

    /* escreve um D só para teste :) */
    movw    $0x9F44, 0xB8000

    hlt
FUNCTION_END(SOmBRA)
