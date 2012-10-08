#ifndef _KERNEL_ISR_H
#define _KERNEL_ISR_H

#define ISR_COUNT   256

#ifndef __ASSEMBLER__

#include "regs.h"
#include <string.h>

typedef int (*isr_call_t)(uregs_t*);
typedef isr_call_t isr_call_vector_t[ISR_COUNT];

// Estrutura para atribuir uma chamada a IDT
typedef struct
{
    unsigned    gate;
    isr_call_t  isr_call;
} isr_vector_t;

#ifndef _ARCH_X86_ISR_C
extern isr_call_vector_t    isr_call_vector;
extern isr_call_vector_t   *isr_call_ptr_vector;
#endif

void isr_init(void);
void isr_set_handler(int num, isr_vector_t *isr_vector);
void isr_get_handler(int num, isr_vector_t *isr_vector);

#endif

#endif
