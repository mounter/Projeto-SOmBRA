#ifndef _X86_INTERRUPTS_H
#define _X86_INTERRUPTS_H

// Constantes do chip 8259s
#define PIC1	0x20
#define PIC2	0xA0
#define ICW1	0x11
#define ICW4	0x01

// Constantes para IRQs para evitar mudanças futuras!
#define IRQ0    0
#define IRQ1    1
#define IRQ2    2
#define IRQ3    3
#define IRQ4    4
#define IRQ5    5
#define IRQ6    6
#define IRQ7    7
#define IRQ8    8
#define IRQ9    9
#define IRQ10   10
#define IRQ11   11
#define IRQ12   12
#define IRQ13   13
#define IRQ14   14
#define IRQ15   15

void interrupts_init(void);
void c8259s_init(void);
void irq_enable(int irq);
void irq_disable(int irq);

#endif
