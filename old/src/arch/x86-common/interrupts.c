#include <x86-common/interrupts.h>
#include <system.h>

static uint16_t ocw1 = 0xFFFB;

void interrupts_init(void)
{
    asm ("sti");
}

// Configura o controlador de interrupções(8253).
void c8259s_init(void)
{
// Envia ICW1
	outportb(PIC1, ICW1);
	outportb(PIC2, ICW1);

// Envia ICW2
	outportb(PIC1 + 1, 0x20);	// Remap
	outportb(PIC2 + 1, 0x28);	// Pics

// Envia ICW3
	outportb(PIC1 + 1, 4);		// IRQ 2 - Conexão em cascata.
	outportb(PIC2 + 1, 2);

// Envia ICW4
	outportb(PIC1 + 1, ICW4);
	outportb(PIC2 + 1, ICW4);

// Desabilita todas as interrupções.
	outportb(PIC1 + 1, 0xFF);
	outportb(PIC2 + 1, 0xFF);
}

// Habilita uma linha de interrupção.
void irq_enable(int irq)
{

	ocw1 &= ~(1 << irq);

	if (irq < 8)
		outportb(PIC1 + 1, ocw1 & 0xFF);
	else
		outportb(PIC2 + 1, ocw1 >> 8);
}

void irq_disable(int irq)
{

	ocw1 |= (1 << irq);

	if (irq < 8)
		outportb(PIC1 + 1, ocw1 & 0xFF);
	else
		outportb(PIC2 + 1, ocw1 >> 8);
}
