#define _ARCH_X86_ISR_C
#include <x86-common/isr.h>
#include <x86/debug.h>
#include <stdio.h>
#include <string.h>

// Vetor de chamadas de interrupções!
isr_call_vector_t   isr_call_vector;
isr_call_vector_t   *isr_call_ptr_vector = 0;

// Inicializando o controlador de ISR
void isr_init(void)
{
    memset(isr_call_vector, 0, sizeof(isr_call_vector_t));
    isr_call_ptr_vector = &isr_call_vector;
}

int isr_fault(uregs_t *regs)
{
// Mensagens de erro
	static const char * const msg[] =
	{
		"divide error", "debug exception", "NMI", "INT3",
		"INTO", "BOUND exception", "invalid opcode", "no coprocessor",
		"double fault", "coprocessor segment overrun",
			"bad TSS", "segment not present",
		"stack fault", "GPF", "page fault", "??",
		"coprocessor error", "alignment check", "??", "??",
		"??", "??", "??", "??",
		"??", "??", "??", "??",
		"??", "??", "??", "??",
		"IRQ0", "IRQ1", "IRQ2", "IRQ3",
		"IRQ4", "IRQ5", "IRQ6", "IRQ7",
		"IRQ8", "IRQ9", "IRQ10", "IRQ11",
		"IRQ12", "IRQ13", "IRQ14", "IRQ15",
		"syscall"
	};

	// Controla as interrupções
	switch(regs->which_int)
	{
		case 0x03:
			dump_uregs(regs);

			if (!(regs->cs & 3))
			{
				printf("\nFazendo um dump das instrucoes:\n\n");
				dump_eip(regs->eip, -11, 10);
			}
		break;
//		case 0x20:	/* timer IRQ 0 */
//			/* reset hardware interrupt at 8259 chip */
//			outportb(0x20, 0x20);
//			timer_irq();
//		break;
    		case 0x27:
			// Para interferencias no hardware (spurious interrupt)!
			// Ignorar está IRQ
        	break;
		default:
			// Vai para o console atual
			set_con(get_con());
//			if (!(regs->cs & 3))
			{
				printf("\nFazendo dump dos registradores:\n\n");
				dump_uregs(regs);
				//printf("\nFazendo um dump das instrucoes:\n\n");
				//dump_rip(regs->rip, -11, 10);
				if (regs->which_int <= (sizeof(msg)/sizeof(char*))){
					panic("Excecao #%d - %s no modo kernel.", regs->which_int, msg[regs->which_int]);
				}
				else{
					panic("Excecao #%d - ??? no modo kernel.", regs->which_int);
				}
			}

//			printf("O processo #%d executou uma operacao invalida e sera finalizado.\n", task_curr->pid);
//			if (regs->which_int<=(sizeof(msg)/sizeof(char*))){
//				printf("Erro: #%d - %s.\n", regs->which_int, msg[regs->which_int]);
//			}
//			else{
//				printf("Erro: #%d - ???.\n", regs->which_int);
//			}
//			printf("\nAbaixo uma listagem do conteudo dos registradores: \n");
//			dump_uregs(regs);
			// Finaliza o processo.
//			taskexit(-1);
		break;
	}

    return 0; // Tudo ok
}
